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
#include "lib/gtpu/gtpu_tunnel_ngu_rx_impl.h"
#include "lib/gtpu/gtpu_tunnel_ngu_tx_impl.h"
#include "tests/unittests/gtpu/gtpu_test_shared.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <sys/socket.h>

using namespace srsran;

class gtpu_tunnel_tx_upper_dummy : public gtpu_tunnel_common_tx_upper_layer_notifier
{
  void on_new_pdu(byte_buffer buf, const ::sockaddr_storage& dest_addr) final { tx_ul_pdus.push_back(buf); }

public:
  void clear() { tx_ul_pdus.clear(); }

  std::vector<byte_buffer> tx_ul_pdus;
};

/// Fixture class for GTP-U tunnel NG-U Rx tests
class gtpu_tunnel_ngu_tx_test : public ::testing::Test
{
public:
  gtpu_tunnel_ngu_tx_test() :
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
    tx_upper.clear();
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
  std::unique_ptr<gtpu_tunnel_ngu_tx_impl> tx;

  // Surrounding tester
  gtpu_tunnel_tx_upper_dummy tx_upper = {};

  null_dlt_pcap dummy_pcap;
};

/// \brief Test correct creation of Rx entity
TEST_F(gtpu_tunnel_ngu_tx_test, entity_creation)
{
  // create Tx entity
  gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_tx_config tx_cfg = {};
  tx_cfg.peer_addr                                         = "127.0.0.1";
  tx_cfg.peer_teid                                         = gtpu_teid_t{0x1};

  tx = std::make_unique<gtpu_tunnel_ngu_tx_impl>(srs_cu_up::ue_index_t::MIN_UE_INDEX, tx_cfg, dummy_pcap, tx_upper);

  ASSERT_NE(tx, nullptr);
};

/// \brief Test reception of PDUs with no SN
TEST_F(gtpu_tunnel_ngu_tx_test, tx_sdus)
{
  // create Rx entity
  gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_tx_config tx_cfg = {};
  tx_cfg.peer_addr                                         = "127.0.0.1";
  tx_cfg.peer_teid                                         = gtpu_teid_t{0x2};

  tx = std::make_unique<gtpu_tunnel_ngu_tx_impl>(srs_cu_up::ue_index_t::MIN_UE_INDEX, tx_cfg, dummy_pcap, tx_upper);
  ASSERT_NE(tx, nullptr);

  for (unsigned i = 0; i < 3; i++) {
    byte_buffer sdu = byte_buffer::create(gtpu_ping_sdu).value();
    byte_buffer pdu = byte_buffer::create(gtpu_ping_vec_teid_2_qfi_1_ul).value();

    tx->handle_sdu(std::move(sdu), uint_to_qos_flow_id(1));
    ASSERT_EQ(pdu, tx_upper.tx_ul_pdus[i]);
  }
};

/// \brief Test in-order reception of PDUs
TEST_F(gtpu_tunnel_ngu_tx_test, tx_stop)
{
  // create Rx entity
  gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_tx_config tx_cfg = {};
  tx_cfg.peer_addr                                         = "127.0.0.1";
  tx_cfg.peer_teid                                         = gtpu_teid_t{0x2};

  tx = std::make_unique<gtpu_tunnel_ngu_tx_impl>(srs_cu_up::ue_index_t::MIN_UE_INDEX, tx_cfg, dummy_pcap, tx_upper);
  ASSERT_NE(tx, nullptr);

  for (unsigned i = 0; i < 3; i++) {
    byte_buffer sdu = byte_buffer::create(gtpu_ping_sdu).value();
    byte_buffer pdu = byte_buffer::create(gtpu_ping_vec_teid_2_qfi_1_ul).value();

    tx->handle_sdu(std::move(sdu), uint_to_qos_flow_id(1));
    ASSERT_EQ(pdu, tx_upper.tx_ul_pdus[i]);
  }
  tx->stop();
  tx_upper.tx_ul_pdus.clear();

  // No more PDUs should be accepted.
  for (unsigned i = 0; i < 3; i++) {
    byte_buffer sdu = byte_buffer::create(gtpu_ping_sdu).value();
    tx->handle_sdu(std::move(sdu), uint_to_qos_flow_id(1));
    ASSERT_TRUE(tx_upper.tx_ul_pdus.empty());
  }
};

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
