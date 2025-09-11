/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "lib/gtpu/gtpu_tunnel_logger.h"
#include "srsran/gtpu/gtpu_tunnel_common_rx.h"
#include "srsran/gtpu/gtpu_tunnel_nru_factory.h"
#include "srsran/gtpu/gtpu_tunnel_nru_rx.h"
#include "srsran/gtpu/gtpu_tunnel_nru_tx.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/executors/task_worker.h"
#include <gtest/gtest.h>
#include <sys/socket.h>

using namespace srsran;

const std::array<uint8_t, 8> pdcp_pdu1_algo1_count0_snlen12 = {0x80, 0x00, 0x28, 0xb7, 0xe0, 0xc5, 0x10, 0x48};

static std::string pcap_dir;

class gtpu_tunnel_rx_lower_dummy : public gtpu_tunnel_nru_rx_lower_layer_notifier
{
  void on_new_sdu(nru_dl_message dl_message) final { last_dl_msg = std::move(dl_message); }
  void on_new_sdu(nru_ul_message ul_message) final { last_ul_msg = std::move(ul_message); }

public:
  nru_dl_message last_dl_msg;
  nru_ul_message last_ul_msg;
};

class gtpu_tunnel_tx_upper_dummy : public gtpu_tunnel_common_tx_upper_layer_notifier
{
  void on_new_pdu(byte_buffer buf, const ::sockaddr_storage& dest_addr) final
  {
    last_tx   = std::move(buf);
    last_addr = dest_addr;
  }

public:
  byte_buffer      last_tx;
  sockaddr_storage last_addr = {};
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

/// This class serves as a wrapper for a DLT PCAP object that allows to control wheter PCAP files shall actually be
/// written or not. This is useful for development of unit tests in order to export the test vectors for further
/// evaluation with 3rd party tools like wireshark.
/// By default (i.e. \c output_dir is empty) a dummy object with no file output is used. If \c output_dir is set, a
/// regular PCAP writer is used which writes a PCAP file to the given directory. The file name is automatically derived
/// from the test suite name and the test name.
class dlt_pcap_helper
{
public:
  dlt_pcap_helper(std::string output_dir)
  {
    if (output_dir.empty()) {
      pcap = std::make_unique<null_dlt_pcap>();
      return;
    }
    pcap_worker   = std::make_unique<task_worker>("thread", 128, os_thread_realtime_priority::no_realtime());
    pcap_executor = make_task_executor_ptr(*pcap_worker);
    pcap          = create_gtpu_pcap(fmt::format("{}/{}", output_dir, generate_pcap_filename()), *pcap_executor);
  }

  ~dlt_pcap_helper()
  {
    // finish PCAP
    if (pcap_worker) {
      pcap_worker->wait_pending_tasks();
    }
  }

  /// \brief Get a reference to the internal DLT PCAP object, which is either a real DLT PCAP or a dummy.
  /// \return Reference to DLT PCAP object.
  dlt_pcap& get_pcap() const { return *pcap; }

private:
  /// \brief Generate a PCAP filename according to the test suite name and test name.
  /// \return A per-test filename for the PCAP.
  static std::string generate_pcap_filename()
  {
    return fmt::format("{}.{}.pcap",
                       testing::UnitTest::GetInstance()->current_test_info()->test_suite_name(),
                       testing::UnitTest::GetInstance()->current_test_info()->name());
  }

  std::unique_ptr<task_worker>   pcap_worker;
  std::unique_ptr<task_executor> pcap_executor;
  std::unique_ptr<dlt_pcap>      pcap;
};

/// Fixture class for GTP-U tunnel NR-U tests
class gtpu_tunnel_nru_test : public ::testing::Test
{
public:
  gtpu_tunnel_nru_test() :
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
    srslog::flush();
  }

  // Test logger
  srslog::basic_logger& logger;

  // GTP-U logger
  srslog::basic_logger& gtpu_logger;
  gtpu_tunnel_logger    gtpu_rx_logger{"GTPU", {srs_cu_up::ue_index_t{}, gtpu_teid_t{1}, "DL"}};
  gtpu_tunnel_logger    gtpu_tx_logger{"GTPU", {srs_cu_up::ue_index_t{}, gtpu_teid_t{1}, "UL"}};

  // PCAP
  dlt_pcap_helper pcap_helper{pcap_dir};

  // Timers
  manual_task_worker worker{64};
  timer_manager      timers_manager;
  timer_factory      timers{timers_manager, worker};

  // GTP-U tunnel entity
  std::unique_ptr<gtpu_tunnel_nru> gtpu;

  // Surrounding tester
  gtpu_tunnel_rx_lower_dummy gtpu_rx = {};
  gtpu_tunnel_tx_upper_dummy gtpu_tx = {};
};

/// \brief Test correct creation of GTP-U entity
TEST_F(gtpu_tunnel_nru_test, entity_creation)
{
  // init GTP-U entity
  gtpu_tunnel_nru_creation_message msg = {};
  msg.cfg.rx.node                      = nru_node::cu_up;
  msg.cfg.rx.local_teid                = gtpu_teid_t{0x1};
  msg.cfg.tx.peer_teid                 = gtpu_teid_t{0x2};
  msg.cfg.tx.peer_addr                 = "127.0.0.1";
  msg.gtpu_pcap                        = &pcap_helper.get_pcap();
  msg.rx_lower                         = &gtpu_rx;
  msg.tx_upper                         = &gtpu_tx;
  gtpu                                 = create_gtpu_tunnel_nru(msg);

  ASSERT_NE(gtpu, nullptr);
}

/// \brief Test correct reception and transmission of a NR-U DL message
TEST_F(gtpu_tunnel_nru_test, rx_tx_nru_dl_msg)
{
  // init GTP-U entity
  gtpu_tunnel_nru_creation_message msg = {};
  msg.cfg.rx.node                      = nru_node::du;
  msg.cfg.rx.local_teid                = gtpu_teid_t{0x1};
  msg.cfg.tx.peer_teid                 = gtpu_teid_t{0x2};
  msg.cfg.tx.peer_addr                 = "127.0.0.1";
  msg.gtpu_pcap                        = &pcap_helper.get_pcap();
  msg.rx_lower                         = &gtpu_rx;
  msg.tx_upper                         = &gtpu_tx;
  gtpu                                 = create_gtpu_tunnel_nru(msg);

  sockaddr_storage orig_addr     = {};
  const uint8_t orig_pdu_bytes[] = {0x34, 0xff, 0x00, 0x14, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x84, 0x04, 0x04,
                                    0x00, 0x11, 0x22, 0x33, 0x02, 0x44, 0x55, 0x66, 0x77, 0xaa, 0xbb, 0xcc, 0xdd, 0x00};
  byte_buffer   orig_pdu;
  ASSERT_TRUE(orig_pdu.append(orig_pdu_bytes));
  pcap_helper.get_pcap().push_pdu(orig_pdu.deep_copy().value());

  // RX
  gtpu_tunnel_common_rx_upper_layer_interface* rx = gtpu->get_rx_upper_layer_interface();
  rx->handle_pdu(orig_pdu.deep_copy().value(), orig_addr);

  nru_dl_message exp_msg              = {};
  exp_msg.dl_user_data.nru_sn         = 0x112233;
  exp_msg.dl_user_data.discard_blocks = nru_pdcp_sn_discard_blocks{{0x445566, 0x77}, {0xaabbcc, 0xdd}};
  ASSERT_EQ(gtpu_rx.last_dl_msg, exp_msg);

  // TX
  gtpu_tunnel_nru_tx_lower_layer_interface* tx = gtpu->get_tx_lower_layer_interface();
  tx->handle_sdu(std::move(gtpu_rx.last_dl_msg));

  ASSERT_EQ(gtpu_tx.last_tx, orig_pdu);
}

/// \brief Test correct transmission and reception of a NR-U DL message (with T-PDU)
TEST_F(gtpu_tunnel_nru_test, tx_rx_nru_dl_msg_with_t_pdu)
{
  // init GTP-U entity
  gtpu_tunnel_nru_creation_message msg = {};
  msg.cfg.rx.node                      = nru_node::du;
  msg.cfg.rx.local_teid                = gtpu_teid_t{0x1};
  msg.cfg.tx.peer_teid                 = gtpu_teid_t{0x2};
  msg.cfg.tx.peer_addr                 = "127.0.0.1";
  msg.gtpu_pcap                        = &pcap_helper.get_pcap();
  msg.rx_lower                         = &gtpu_rx;
  msg.tx_upper                         = &gtpu_tx;
  gtpu                                 = create_gtpu_tunnel_nru(msg);

  sockaddr_storage orig_addr         = {};
  nru_dl_message   tx_msg            = {};
  tx_msg.dl_user_data.nru_sn         = 0x112233;
  tx_msg.dl_user_data.discard_blocks = nru_pdcp_sn_discard_blocks{{0x445566, 0x77}, {0xaabbcc, 0xdd}};
  tx_msg.t_pdu                       = byte_buffer::create(pdcp_pdu1_algo1_count0_snlen12).value();

  // TX
  gtpu_tunnel_nru_tx_lower_layer_interface* tx          = gtpu->get_tx_lower_layer_interface();
  auto                                      tx_msg_copy = tx_msg.deep_copy();
  ASSERT_TRUE(tx_msg_copy.has_value());
  tx->handle_sdu(std::move(tx_msg_copy.value()));

  byte_buffer exp_pdu =
      make_byte_buffer("34ff001c00000002000000840404001122330244556677aabbccdd00800028b7e0c51048").value();
  ASSERT_EQ(exp_pdu, gtpu_tx.last_tx);

  // RX
  gtpu_tunnel_common_rx_upper_layer_interface* rx = gtpu->get_rx_upper_layer_interface();
  rx->handle_pdu(std::move(gtpu_tx.last_tx), orig_addr);
  ASSERT_EQ(gtpu_rx.last_dl_msg, tx_msg);
}

/// \brief Test correct transmission and reception of a NR-U UL message (no T-PDU)
TEST_F(gtpu_tunnel_nru_test, tx_rx_nru_ul_msg)
{
  // init GTP-U entity
  gtpu_tunnel_nru_creation_message msg = {};
  msg.cfg.rx.node                      = nru_node::cu_up;
  msg.cfg.rx.local_teid                = gtpu_teid_t{0x1};
  msg.cfg.tx.peer_teid                 = gtpu_teid_t{0x2};
  msg.cfg.tx.peer_addr                 = "127.0.0.1";
  msg.gtpu_pcap                        = &pcap_helper.get_pcap();
  msg.rx_lower                         = &gtpu_rx;
  msg.tx_upper                         = &gtpu_tx;
  gtpu                                 = create_gtpu_tunnel_nru(msg);

  sockaddr_storage orig_addr            = {};
  nru_ul_message   tx_msg               = {};
  auto&            tx_status            = tx_msg.data_delivery_status.emplace();
  tx_status.desired_buffer_size_for_drb = 11223344;
  tx_status.lost_nru_sn_ranges          = nru_lost_nru_sn_ranges{{2000001, 2000009}, {5000100, 5000321}};

  // TX
  gtpu_tunnel_nru_tx_lower_layer_interface* tx          = gtpu->get_tx_lower_layer_interface();
  auto                                      tx_msg_copy = tx_msg.deep_copy();
  ASSERT_TRUE(tx_msg_copy.has_value());
  tx->handle_sdu(std::move(tx_msg_copy.value()));

  byte_buffer exp_pdu =
      make_byte_buffer("34ff001c000000020000008406110000ab4130021e84811e84894c4ba44c4c8100000000").value();
  ASSERT_EQ(exp_pdu, gtpu_tx.last_tx);

  // RX
  gtpu_tunnel_common_rx_upper_layer_interface* rx = gtpu->get_rx_upper_layer_interface();
  rx->handle_pdu(std::move(gtpu_tx.last_tx), orig_addr);
  ASSERT_EQ(gtpu_rx.last_ul_msg, tx_msg);
}

/// \brief Test correct transmission and reception of a NR-U UL message (with T-PDU)
TEST_F(gtpu_tunnel_nru_test, tx_rx_nru_ul_msg_with_t_pdu)
{
  // init GTP-U entity
  gtpu_tunnel_nru_creation_message msg = {};
  msg.cfg.rx.node                      = nru_node::cu_up;
  msg.cfg.rx.local_teid                = gtpu_teid_t{0x1};
  msg.cfg.tx.peer_teid                 = gtpu_teid_t{0x2};
  msg.cfg.tx.peer_addr                 = "127.0.0.1";
  msg.gtpu_pcap                        = &pcap_helper.get_pcap();
  msg.rx_lower                         = &gtpu_rx;
  msg.tx_upper                         = &gtpu_tx;
  gtpu                                 = create_gtpu_tunnel_nru(msg);

  sockaddr_storage orig_addr            = {};
  nru_ul_message   tx_msg               = {};
  auto&            tx_status            = tx_msg.data_delivery_status.emplace();
  tx_status.desired_buffer_size_for_drb = 22334455;
  tx_status.lost_nru_sn_ranges          = nru_lost_nru_sn_ranges{{2000001, 2000009}};
  tx_msg.t_pdu = byte_buffer_chain::create(byte_buffer::create(pdcp_pdu1_algo1_count0_snlen12).value()).value();

  // TX
  gtpu_tunnel_nru_tx_lower_layer_interface* tx          = gtpu->get_tx_lower_layer_interface();
  auto                                      tx_msg_copy = tx_msg.deep_copy();
  ASSERT_TRUE(tx_msg_copy.has_value());
  tx->handle_sdu(std::move(tx_msg_copy.value()));

  byte_buffer exp_pdu =
      make_byte_buffer("34ff001c00000002000000840411000154cbf7011e84811e84890000800028b7e0c51048").value();
  ASSERT_EQ(exp_pdu, gtpu_tx.last_tx);

  // RX
  gtpu_tunnel_common_rx_upper_layer_interface* rx = gtpu->get_rx_upper_layer_interface();
  rx->handle_pdu(std::move(gtpu_tx.last_tx), orig_addr);
  ASSERT_EQ(gtpu_rx.last_ul_msg, tx_msg);
}

void usage(const char* prog)
{
  fmt::print("Usage: {} [-p DIR]\n", prog);
  fmt::print("\t-p Set DIR for writing PCAPs [Off by default]\n");
  fmt::print("\t-h Show this message\n");
}

void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "p:h")) != -1) {
    switch (opt) {
      case 'p':
        pcap_dir = std::string(optarg);
        break;
      case 'h':
      default:
        usage(argv[0]);
        std::exit(0);
    }
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  parse_args(argc, argv);
  return RUN_ALL_TESTS();
}
