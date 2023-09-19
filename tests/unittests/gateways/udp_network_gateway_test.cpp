/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "test_helpers.h"
#include "srsran/gateways/udp_network_gateway_factory.h"

using namespace srsran;

class udp_network_gateway_tester : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // init GW logger
    srslog::fetch_basic_logger("UDP-GW", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("UDP-GW", false).set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();

    stop_token.store(true, std::memory_order_relaxed);
    if (rx_thread.joinable()) {
      rx_thread.join();
    }
  }

  void set_config(udp_network_gateway_config server_config, udp_network_gateway_config client_config)
  {
    server = create_udp_network_gateway({server_config, server_data_notifier});
    ASSERT_NE(server, nullptr);
    client = create_udp_network_gateway({client_config, client_data_notifier});
    ASSERT_NE(client, nullptr);
  }

  // spawn a thread to receive data
  void start_receive_thread()
  {
    rx_thread = std::thread([this]() {
      stop_token.store(false);
      while (not stop_token.load(std::memory_order_relaxed)) {
        // call receive() on socket
        server->receive();

        std::this_thread::sleep_for(std::chrono::microseconds(100));
      }
    });
  }

  void run_client_receive() { client->receive(); }

  void send_to_server(const byte_buffer& pdu, std::string dest_addr, uint16_t port)
  {
    in_addr          inaddr_v4    = {};
    in6_addr         inaddr_v6    = {};
    sockaddr_storage addr_storage = {};

    if (inet_pton(AF_INET, dest_addr.c_str(), &inaddr_v4) == 1) {
      sockaddr_in* tmp = (sockaddr_in*)&addr_storage;
      tmp->sin_family  = AF_INET;
      tmp->sin_addr    = inaddr_v4;
      tmp->sin_port    = htons(port);
    } else if (inet_pton(AF_INET6, dest_addr.c_str(), &inaddr_v6) == 1) {
      sockaddr_in6* tmp = (sockaddr_in6*)&addr_storage;
      tmp->sin6_family  = AF_INET6;
      tmp->sin6_addr    = inaddr_v6;
      tmp->sin6_port    = htons(port);
    } else {
      FAIL();
    }
    client->handle_pdu(pdu, addr_storage);
  }

protected:
  dummy_network_gateway_control_notifier server_control_notifier;
  dummy_network_gateway_control_notifier client_control_notifier;

  dummy_network_gateway_data_notifier_with_src_addr server_data_notifier;
  dummy_network_gateway_data_notifier_with_src_addr client_data_notifier;

  std::unique_ptr<udp_network_gateway> server, client;

  std::string server_address_v4 = "127.0.0.1";
  std::string client_address_v4 = "127.0.1.1";

  std::string server_address_v6 = "::1";
  std::string client_address_v6 = "::1";

  std::string server_hostname = "localhost";
  std::string client_hostname = "localhost";

private:
  std::thread       rx_thread;
  std::atomic<bool> stop_token = {false};
};

TEST_F(udp_network_gateway_tester, when_binding_on_bogus_address_then_bind_fails)
{
  udp_network_gateway_config config;
  config.bind_address = "1.1.1.1";
  config.bind_port    = get_unused_udp_port("0.0.0.0");
  config.reuse_addr   = true;
  ASSERT_NE(config.bind_port, 0);
  set_config(config, config);
  ASSERT_FALSE(server->create_and_bind());
}

TEST_F(udp_network_gateway_tester, when_binding_on_bogus_v6_address_then_bind_fails)
{
  udp_network_gateway_config config;
  config.bind_address = "1:1::";
  config.bind_port    = get_unused_udp_port("::1");
  config.reuse_addr   = true;
  ASSERT_NE(config.bind_port, 0);
  set_config(config, config);
  ASSERT_FALSE(server->create_and_bind());
}

TEST_F(udp_network_gateway_tester, when_binding_on_localhost_then_bind_succeeds)
{
  udp_network_gateway_config config;
  config.bind_address = "127.0.0.1";
  config.bind_port    = get_unused_udp_port(config.bind_address);
  config.reuse_addr   = true;
  ASSERT_NE(config.bind_port, 0);
  set_config(config, config);
  ASSERT_TRUE(server->create_and_bind());
}

TEST_F(udp_network_gateway_tester, when_binding_on_v6_localhost_then_bind_succeeds)
{
  udp_network_gateway_config config;
  config.bind_address = "::1";
  config.bind_port    = get_unused_udp_port(config.bind_address);
  config.reuse_addr   = true;
  ASSERT_NE(config.bind_port, 0);
  set_config(config, config);
  ASSERT_TRUE(server->create_and_bind());
}

TEST_F(udp_network_gateway_tester, when_config_valid_then_trx_succeeds)
{
  // create server config
  uint16_t server_port = get_unused_udp_port(server_address_v4);
  ASSERT_NE(server_port, 0);

  udp_network_gateway_config server_config;
  server_config.bind_address      = server_address_v4;
  server_config.bind_port         = server_port;
  server_config.non_blocking_mode = true;

  // create client config
  uint16_t client_port = get_unused_udp_port(client_address_v4);
  ASSERT_NE(client_port, 0);

  udp_network_gateway_config client_config;
  client_config.bind_address      = client_address_v4;
  client_config.bind_port         = client_port;
  client_config.non_blocking_mode = true;

  // set configs
  set_config(server_config, client_config);

  // create and bind gateways
  ASSERT_TRUE(server->create_and_bind());
  ASSERT_TRUE(client->create_and_bind());
  start_receive_thread();

  byte_buffer pdu_small(make_small_tx_byte_buffer());
  send_to_server(pdu_small, server_address_v4, server_port);
  byte_buffer pdu_large(make_large_tx_byte_buffer());
  send_to_server(pdu_large, server_address_v4, server_port);
  byte_buffer pdu_oversized(make_oversized_tx_byte_buffer());
  send_to_server(pdu_oversized, server_address_v4, server_port);

  // let the Rx thread pick up the message
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  // check reception of small PDU
  ASSERT_FALSE(server_data_notifier.pdu_queue.empty());
  ASSERT_EQ(server_data_notifier.pdu_queue.front(), pdu_small);
  server_data_notifier.pdu_queue.pop();
  // check reception of large PDU
  ASSERT_FALSE(server_data_notifier.pdu_queue.empty());
  ASSERT_EQ(server_data_notifier.pdu_queue.front(), pdu_large);
  server_data_notifier.pdu_queue.pop();
  // oversized PDU not expected to be received
  ASSERT_TRUE(server_data_notifier.pdu_queue.empty());
}

TEST_F(udp_network_gateway_tester, when_v6_config_valid_then_trx_succeeds)
{
  // create server config
  uint16_t server_port = get_unused_udp_port(server_address_v6);
  ASSERT_NE(server_port, 0);

  udp_network_gateway_config server_config;
  server_config.bind_address      = server_address_v6;
  server_config.bind_port         = server_port;
  server_config.non_blocking_mode = true;

  uint16_t client_port = get_unused_udp_port(client_address_v6);
  ASSERT_NE(client_port, 0);

  udp_network_gateway_config client_config;
  client_config.bind_address      = client_address_v6;
  client_config.bind_port         = client_port;
  client_config.non_blocking_mode = true;

  set_config(server_config, client_config);

  ASSERT_TRUE(server->create_and_bind());
  ASSERT_TRUE(client->create_and_bind());
  start_receive_thread();

  byte_buffer pdu_small(make_small_tx_byte_buffer());
  send_to_server(pdu_small, server_address_v6, server_port);
  byte_buffer pdu_large(make_large_tx_byte_buffer());
  send_to_server(pdu_large, server_address_v6, server_port);
  byte_buffer pdu_oversized(make_oversized_tx_byte_buffer());
  send_to_server(pdu_oversized, server_address_v6, server_port);

  // let the Rx thread pick up the message
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  // check reception of small PDU
  ASSERT_FALSE(server_data_notifier.pdu_queue.empty());
  ASSERT_EQ(server_data_notifier.pdu_queue.front(), pdu_small);
  server_data_notifier.pdu_queue.pop();
  // check reception of large PDU
  ASSERT_FALSE(server_data_notifier.pdu_queue.empty());
  ASSERT_EQ(server_data_notifier.pdu_queue.front(), pdu_large);
  server_data_notifier.pdu_queue.pop();
  // oversized PDU not expected to be received
  ASSERT_TRUE(server_data_notifier.pdu_queue.empty());
}

TEST_F(udp_network_gateway_tester, when_hostname_resolved_then_trx_succeeds)
{
  // create server config
  uint16_t server_port = get_unused_udp_port(server_hostname);
  ASSERT_NE(server_port, 0);

  udp_network_gateway_config server_config;
  server_config.bind_address      = server_hostname;
  server_config.bind_port         = server_port;
  server_config.non_blocking_mode = true;

  // create client config
  uint16_t client_port = get_unused_udp_port(client_hostname);
  ASSERT_NE(client_port, 0);
  udp_network_gateway_config client_config;
  client_config.bind_address      = client_hostname;
  client_config.bind_port         = client_port;
  client_config.non_blocking_mode = true;

  // set client and server configs
  set_config(server_config, client_config);

  ASSERT_TRUE(server->create_and_bind());
  ASSERT_TRUE(client->create_and_bind());
  start_receive_thread();

  std::string server_address = {};
  ASSERT_TRUE(server->get_bind_address(server_address));
  byte_buffer pdu_small(make_small_tx_byte_buffer());
  send_to_server(pdu_small, server_address, server_port);
  byte_buffer pdu_large(make_large_tx_byte_buffer());
  send_to_server(pdu_large, server_address, server_port);
  byte_buffer pdu_oversized(make_oversized_tx_byte_buffer());
  send_to_server(pdu_oversized, server_address, server_port);

  // let the Rx thread pick up the message
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  // check reception of small PDU
  ASSERT_FALSE(server_data_notifier.pdu_queue.empty());
  ASSERT_EQ(server_data_notifier.pdu_queue.front(), pdu_small);
  server_data_notifier.pdu_queue.pop();
  // check reception of large PDU
  ASSERT_FALSE(server_data_notifier.pdu_queue.empty());
  ASSERT_EQ(server_data_notifier.pdu_queue.front(), pdu_large);
  server_data_notifier.pdu_queue.pop();
  // oversized PDU not expected to be received
  ASSERT_TRUE(server_data_notifier.pdu_queue.empty());
}
