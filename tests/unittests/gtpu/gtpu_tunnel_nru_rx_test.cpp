/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "lib/gtpu/gtpu_pdu.h"
#include "lib/gtpu/gtpu_tunnel_nru_rx_impl.h"
#include "lib/gtpu/gtpu_tunnel_nru_tx_impl.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <sys/socket.h>

using namespace srsran;

class gtpu_pdu_generator
{
  class gtpu_tunnel_tx_upper_dummy : public gtpu_tunnel_common_tx_upper_layer_notifier
  {
    void on_new_pdu(byte_buffer buf, const ::sockaddr_storage& dest_addr) final { parent.gen_pdu = std::move(buf); }
    gtpu_pdu_generator& parent;

  public:
    gtpu_tunnel_tx_upper_dummy(gtpu_pdu_generator& parent_) : parent(parent_) {}
  };

public:
  gtpu_pdu_generator(gtpu_teid_t teid) : tx_upper_dummy(*this)
  {
    gtpu_tunnel_nru_config::gtpu_tunnel_nru_tx_config cfg = {};
    cfg.peer_teid                                         = teid;
    cfg.peer_addr                                         = "127.0.0.1";

    tx =
        std::make_unique<gtpu_tunnel_nru_tx_impl>(srs_cu_up::ue_index_t::MIN_UE_INDEX, cfg, dummy_pcap, tx_upper_dummy);
  }

  byte_buffer create_gtpu_pdu(byte_buffer buf, gtpu_teid_t teid, uint32_t nru_sn, std::optional<uint16_t> gtpu_sn)
  {
    gtpu_header hdr         = {};
    hdr.flags.version       = GTPU_FLAGS_VERSION_V1;
    hdr.flags.protocol_type = GTPU_FLAGS_GTP_PROTOCOL;
    hdr.flags.seq_number    = gtpu_sn.has_value();
    hdr.flags.ext_hdr       = true;
    hdr.message_type        = GTPU_MSG_DATA_PDU;
    hdr.length              = buf.length() + 4 + 4;
    hdr.teid                = teid;
    hdr.next_ext_hdr_type   = gtpu_extension_header_type::nr_ran_container;

    // Put NR-U DL user data into NR RAN container
    nru_dl_user_data dl_user_data;
    dl_user_data.nru_sn = nru_sn;
    nru_packing packer(srslog::fetch_basic_logger("GTP-U", false));
    byte_buffer ext_buf;
    packer.pack(ext_buf, dl_user_data);

    // Write NR RAN container
    gtpu_extension_header ext;
    ext.extension_header_type = gtpu_extension_header_type::nr_ran_container;
    ext.container             = ext_buf;

    hdr.ext_list.push_back(ext);

    if (gtpu_sn.has_value()) {
      hdr.flags.seq_number = true;
      hdr.seq_number       = gtpu_sn.value();
    }

    bool write_ok = gtpu_write_header(buf, hdr, gtpu_logger);

    if (!write_ok) {
      gtpu_logger.log_error("Dropped SDU, error writing GTP-U header. teid={}", hdr.teid);
      return {};
    }

    return buf;
  }

private:
  null_dlt_pcap                            dummy_pcap = {};
  gtpu_tunnel_tx_upper_dummy               tx_upper_dummy;
  std::unique_ptr<gtpu_tunnel_nru_tx_impl> tx;
  byte_buffer                              gen_pdu;
  gtpu_tunnel_logger                       gtpu_logger{"GTPU", {srs_cu_up::ue_index_t{}, gtpu_teid_t{1}, "DL"}};

public:
};

class gtpu_tunnel_rx_lower_dummy : public gtpu_tunnel_nru_rx_lower_layer_notifier
{
  void on_new_sdu(nru_dl_message dl_message) final { rx_dl_sdus.push_back(std::move(dl_message.t_pdu)); }
  void on_new_sdu(nru_ul_message ul_message) final
  {
    if (ul_message.t_pdu.has_value()) {
      rx_ul_sdus.push_back(std::move(ul_message.t_pdu.value()));
    }
  }

public:
  void clear()
  {
    rx_dl_sdus.clear();
    rx_ul_sdus.clear();
  }

  std::vector<byte_buffer>       rx_dl_sdus;
  std::vector<byte_buffer_chain> rx_ul_sdus;
};

class gtpu_tunnel_rx_upper_dummy : public gtpu_tunnel_common_rx_upper_layer_interface
{
public:
  void handle_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) final
  {
    last_rx   = std::move(pdu);
    last_addr = src_addr;
  }

  byte_buffer      last_rx;
  sockaddr_storage last_addr = {};
};

/// Fixture class for GTP-U tunnel NR-U Rx tests
class gtpu_tunnel_nru_rx_test : public ::testing::Test
{
public:
  gtpu_tunnel_nru_rx_test() :
    logger(srslog::fetch_basic_logger("TEST", false)), gtpu_logger(srslog::fetch_basic_logger("GTPU", false))
  {
  }

protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init GTP-U logger
    gtpu_logger.set_level(srslog::basic_levels::debug);
    gtpu_logger.set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    rx_lower.clear();
    srslog::flush();
  }

  /// \brief Helper to advance the timers
  /// \param nof_tick Number of ticks to advance timers
  void tick_all(uint32_t nof_ticks)
  {
    for (uint32_t i = 0; i < nof_ticks; i++) {
      timers_manager.tick();
      worker.run_pending_tasks();
    }
  }

  gtpu_pdu_generator pdu_generator{gtpu_teid_t{0x1}};

  // Test logger
  srslog::basic_logger& logger;

  // GTP-U logger
  srslog::basic_logger& gtpu_logger;
  gtpu_tunnel_logger    gtpu_rx_logger{"GTPU", {srs_cu_up::ue_index_t{}, gtpu_teid_t{1}, "DL"}};

  // Timers
  manual_task_worker worker{64};
  timer_manager      timers_manager;
  timer_factory      timers{timers_manager, worker};

  // GTP-U tunnel Rx entity
  std::unique_ptr<gtpu_tunnel_nru_rx_impl> rx;

  // Surrounding tester
  gtpu_tunnel_rx_lower_dummy rx_lower = {};
};

/// \brief Test correct creation of Rx entity
TEST_F(gtpu_tunnel_nru_rx_test, entity_creation)
{
  // create Rx entity
  gtpu_tunnel_nru_config::gtpu_tunnel_nru_rx_config rx_cfg = {};
  rx_cfg.local_teid                                        = gtpu_teid_t{0x1};

  rx = std::make_unique<gtpu_tunnel_nru_rx_impl>(srs_cu_up::ue_index_t::MIN_UE_INDEX, rx_cfg, rx_lower);

  ASSERT_NE(rx, nullptr);
};

/// \brief Test reception of PDUs with no SN
TEST_F(gtpu_tunnel_nru_rx_test, rx_no_sn)
{
  // create Rx entity
  gtpu_tunnel_nru_config::gtpu_tunnel_nru_rx_config rx_cfg = {};
  rx_cfg.local_teid                                        = gtpu_teid_t{0x1};

  rx = std::make_unique<gtpu_tunnel_nru_rx_impl>(srs_cu_up::ue_index_t::MIN_UE_INDEX, rx_cfg, rx_lower);
  ASSERT_NE(rx, nullptr);

  sockaddr_storage src_addr;

  for (unsigned i = 0; i < 3; i++) {
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x11));
    byte_buffer          pdu     = pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), rx_cfg.local_teid, i, {});
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    ASSERT_EQ(rx_lower.rx_dl_sdus.size(), i + 1);
    EXPECT_EQ(rx_lower.rx_dl_sdus[i], sdu);
  }
};

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
