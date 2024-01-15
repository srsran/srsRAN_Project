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

#include "gtpu_test_shared.h"
#include "lib/gtpu/gtpu_pdu.h"
#include "srsran/gtpu/gtpu_tunnel_ngu_factory.h"
#include "srsran/gtpu/gtpu_tunnel_rx.h"
#include "srsran/gtpu/gtpu_tunnel_tx.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <queue>
#include <sys/socket.h>

using namespace srsran;

class gtpu_tunnel_rx_lower_dummy : public gtpu_tunnel_ngu_rx_lower_layer_notifier
{
  void on_new_sdu(byte_buffer sdu, qos_flow_id_t qos_flow_id) final
  {
    last_rx             = std::move(sdu);
    last_rx_qos_flow_id = qos_flow_id;
  }

public:
  byte_buffer   last_rx;
  qos_flow_id_t last_rx_qos_flow_id;
};
class gtpu_tunnel_tx_upper_dummy : public gtpu_tunnel_tx_upper_layer_notifier
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

class gtpu_tunnel_rx_upper_dummy : public gtpu_tunnel_rx_upper_layer_interface
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

/// Fixture class for GTP-U tunnel NG-U tests
class gtpu_tunnel_ngu_test : public ::testing::Test
{
public:
  gtpu_tunnel_ngu_test() :
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

  // Timers
  manual_task_worker worker{64};
  timer_manager      timers_manager;
  timer_factory      timers{timers_manager, worker};

  // GTP-U tunnel entity
  std::unique_ptr<gtpu_tunnel_ngu> gtpu;

  // Surrounding tester
  gtpu_tunnel_rx_lower_dummy gtpu_rx = {};
  gtpu_tunnel_tx_upper_dummy gtpu_tx = {};
};

/// \brief Test correct creation of GTP-U entity
TEST_F(gtpu_tunnel_ngu_test, entity_creation)
{
  null_dlt_pcap dummy_pcap;
  // init GTP-U entity
  gtpu_tunnel_ngu_creation_message msg = {};
  msg.cfg.rx.local_teid                = gtpu_teid_t{0x1};
  msg.cfg.tx.peer_teid                 = gtpu_teid_t{0x2};
  msg.cfg.tx.peer_addr                 = "127.0.0.1";
  msg.gtpu_pcap                        = &dummy_pcap;
  msg.rx_lower                         = &gtpu_rx;
  msg.tx_upper                         = &gtpu_tx;
  msg.timers                           = timers;
  gtpu                                 = create_gtpu_tunnel_ngu(msg);

  ASSERT_NE(gtpu, nullptr);
};

/// \brief Test correct reception of GTP-U packet with PDU Session Container
TEST_F(gtpu_tunnel_ngu_test, rx_sdu)
{
  null_dlt_pcap dummy_pcap;
  // init GTP-U entity
  gtpu_tunnel_ngu_creation_message msg = {};
  msg.cfg.rx.local_teid                = gtpu_teid_t{0x2};
  msg.cfg.tx.peer_teid                 = gtpu_teid_t{0xbc1e3be9};
  msg.cfg.tx.peer_addr                 = "127.0.0.1";
  msg.gtpu_pcap                        = &dummy_pcap;
  msg.rx_lower                         = &gtpu_rx;
  msg.tx_upper                         = &gtpu_tx;
  msg.timers                           = timers;
  gtpu                                 = create_gtpu_tunnel_ngu(msg);

  sockaddr_storage   orig_addr = {};
  byte_buffer        orig_vec  = make_byte_buffer(gtpu_ping_vec_teid_2_qfi_1_dl);
  byte_buffer        strip_vec = make_byte_buffer(gtpu_ping_vec_teid_2_qfi_1_dl);
  gtpu_dissected_pdu dissected_pdu;
  bool               read_ok = gtpu_dissect_pdu(dissected_pdu, strip_vec.deep_copy(), gtpu_rx_logger);
  ASSERT_EQ(read_ok, true);

  gtpu_tunnel_rx_upper_layer_interface* rx = gtpu->get_rx_upper_layer_interface();
  rx->handle_pdu(std::move(orig_vec), orig_addr);
  ASSERT_EQ(gtpu_extract_t_pdu(std::move(dissected_pdu)), gtpu_rx.last_rx);
  ASSERT_EQ(uint_to_qos_flow_id(1), gtpu_rx.last_rx_qos_flow_id);
};

/// \brief Test correct transmission of GTP-U packet
TEST_F(gtpu_tunnel_ngu_test, tx_pdu)
{
  null_dlt_pcap dummy_pcap;
  // init GTP-U entity
  gtpu_tunnel_ngu_creation_message msg = {};
  msg.cfg.rx.local_teid                = gtpu_teid_t{0x1};
  msg.cfg.tx.peer_teid                 = gtpu_teid_t{0x2};
  msg.cfg.tx.peer_addr                 = "127.0.0.1";
  msg.gtpu_pcap                        = &dummy_pcap;
  msg.rx_lower                         = &gtpu_rx;
  msg.tx_upper                         = &gtpu_tx;
  msg.timers                           = timers;
  gtpu                                 = create_gtpu_tunnel_ngu(msg);

  byte_buffer sdu{gtpu_ping_sdu};
  byte_buffer pdu{gtpu_ping_vec_teid_2_qfi_1_ul};

  gtpu_tunnel_tx_lower_layer_interface* tx = gtpu->get_tx_lower_layer_interface();
  tx->handle_sdu(std::move(sdu), uint_to_qos_flow_id(1));
  ASSERT_EQ(pdu, gtpu_tx.last_tx);
};

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
