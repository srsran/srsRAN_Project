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
#include "lib/gtpu/gtpu_tunnel_logger.h"
#include "srsran/gtpu/gtpu_echo_factory.h"
#include "srsran/gtpu/gtpu_tunnel_rx.h"
#include "srsran/gtpu/gtpu_tunnel_tx.h"
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

/// Fixture class for GTP-U echo tests
class gtpu_echo_test : public ::testing::Test
{
public:
  gtpu_echo_test() :
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
  gtpu_tunnel_logger    gtpu_rx_logger{"GTPU", {srs_cu_up::ue_index_t{}, gtpu_teid_t{0}, "DL"}};
  gtpu_tunnel_logger    gtpu_tx_logger{"GTPU", {srs_cu_up::ue_index_t{}, gtpu_teid_t{0}, "UL"}};

  // GTP-U echo entity
  std::unique_ptr<gtpu_echo> echo;

  // Surrounding tester
  gtpu_tunnel_rx_lower_dummy gtpu_rx = {};
  gtpu_tunnel_tx_upper_dummy gtpu_tx = {};
};

/// \brief Test correct creation of echo entity
TEST_F(gtpu_echo_test, entity_creation)
{
  null_dlt_pcap dummy_pcap;
  // init echo entity
  gtpu_echo_creation_message msg = {};
  msg.gtpu_pcap                  = &dummy_pcap;
  msg.tx_upper                   = &gtpu_tx;
  echo                           = create_gtpu_echo(msg);

  ASSERT_NE(echo, nullptr);
};

/// \brief Test correct reception of an echo message and check for the response
TEST_F(gtpu_echo_test, rx_echo_req_tx_echo_rep)
{
  null_dlt_pcap dummy_pcap;
  // init echo entity
  gtpu_echo_creation_message msg = {};
  msg.gtpu_pcap                  = &dummy_pcap;
  msg.tx_upper                   = &gtpu_tx;
  echo                           = create_gtpu_echo(msg);

  sockaddr_storage orig_addr = {};
  byte_buffer      echo_req  = byte_buffer::create(gtpu_echo_request_sn_65535).value();

  gtpu_tunnel_rx_upper_layer_interface* rx = echo->get_rx_upper_layer_interface();
  rx->handle_pdu(std::move(echo_req), orig_addr);

  ASSERT_FALSE(gtpu_tx.last_tx.empty());
  byte_buffer echo_rep = byte_buffer::create(gtpu_echo_response_sn_65535).value();
  ASSERT_EQ(gtpu_tx.last_tx, echo_rep);
};

/// \brief Test correct reception of an error indication message
TEST_F(gtpu_echo_test, rx_error_indication)
{
  null_dlt_pcap dummy_pcap;
  // init echo entity
  gtpu_echo_creation_message msg = {};
  msg.gtpu_pcap                  = &dummy_pcap;
  msg.tx_upper                   = &gtpu_tx;
  echo                           = create_gtpu_echo(msg);

  sockaddr_storage orig_addr        = {};
  byte_buffer      error_indication = byte_buffer::create(gtpu_error_indication).value();

  gtpu_tunnel_rx_upper_layer_interface* rx = echo->get_rx_upper_layer_interface();
  rx->handle_pdu(std::move(error_indication), orig_addr);

  ASSERT_TRUE(gtpu_tx.last_tx.empty());
};

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
