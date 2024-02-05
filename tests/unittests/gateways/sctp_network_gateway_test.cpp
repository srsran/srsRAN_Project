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

#include "test_helpers.h"
#include "srsran/gateways/sctp_network_gateway_factory.h"
#include <linux/sctp.h>
#include <netinet/sctp.h>

using namespace srsran;

class sctp_network_gateway_tester : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // init GW logger
    srslog::fetch_basic_logger("SCTP-GW", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("SCTP-GW", false).set_hex_dump_max_size(100);
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

  void create_server(const sctp_network_gateway_config& server_config)
  {
    server = create_sctp_network_gateway({server_config, server_control_notifier, server_data_notifier});
    ASSERT_NE(server, nullptr);
  }

  void create_client(const sctp_network_gateway_config& client_config)
  {
    client = create_sctp_network_gateway({client_config, client_control_notifier, client_data_notifier});
    ASSERT_NE(client, nullptr);
  }

  bool bind_and_listen()
  {
    if (!server->create_and_bind()) {
      return false;
    }

    if (!server->listen()) {
      return false;
    }

    return true;
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

  bool connect() { return client->create_and_connect(); }

  void run_client_receive() { client->receive(); }

  void send_to_server(const byte_buffer& pdu) { client->handle_pdu(pdu); }

  dummy_network_gateway_control_notifier server_control_notifier;
  dummy_network_gateway_control_notifier client_control_notifier;

  dummy_network_gateway_data_notifier server_data_notifier;
  dummy_network_gateway_data_notifier client_data_notifier;

  std::unique_ptr<sctp_network_gateway> server, client;

private:
  std::thread       rx_thread;
  std::atomic<bool> stop_token = {false};
};

TEST_F(sctp_network_gateway_tester, when_binding_on_bogus_address_then_bind_fails)
{
  sctp_network_gateway_config config;
  config.bind_address = "1.1.1.1";
  config.bind_port    = 0;
  config.reuse_addr   = true;
  create_server(config);
  ASSERT_FALSE(bind_and_listen());
  optional<uint16_t> server_port = server->get_listen_port();
  ASSERT_FALSE(server_port.has_value());
}

TEST_F(sctp_network_gateway_tester, when_binding_on_bogus_v6_address_then_bind_fails)
{
  sctp_network_gateway_config config;
  config.bind_address = "1:1::";
  config.bind_port    = 0;
  config.reuse_addr   = true;
  create_server(config);
  ASSERT_FALSE(bind_and_listen());
  optional<uint16_t> server_port = server->get_listen_port();
  ASSERT_FALSE(server_port.has_value());
}

TEST_F(sctp_network_gateway_tester, when_binding_on_localhost_then_bind_succeeds)
{
  sctp_network_gateway_config config;
  config.bind_address = "127.0.0.1";
  config.bind_port    = 0;
  config.reuse_addr   = true;
  create_server(config);
  ASSERT_TRUE(bind_and_listen());
  optional<uint16_t> server_port = server->get_listen_port();
  ASSERT_TRUE(server_port.has_value());
  ASSERT_NE(server_port.value(), 0);
}

TEST_F(sctp_network_gateway_tester, when_binding_on_v6_localhost_then_bind_succeeds)
{
  sctp_network_gateway_config config;
  config.bind_address = "::1";
  config.bind_port    = 0;
  config.reuse_addr   = true;
  create_server(config);
  ASSERT_TRUE(bind_and_listen());
  optional<uint16_t> server_port = server->get_listen_port();
  ASSERT_TRUE(server_port.has_value());
  ASSERT_NE(server_port.value(), 0);
}

TEST_F(sctp_network_gateway_tester, when_socket_not_exists_then_connect_fails)
{
  ASSERT_FALSE(client_control_notifier.get_connection_dropped());

  sctp_network_gateway_config config;
  config.connection_name   = "TEST";
  config.connect_address   = "127.0.0.1";
  config.connect_port      = 0; // attempt to connect to port 0 which should always fail
  config.non_blocking_mode = true;
  config.reuse_addr        = true;
  create_client(config);
  ASSERT_FALSE(connect());
}

TEST_F(sctp_network_gateway_tester, when_v6_socket_not_exists_then_connect_fails)
{
  ASSERT_FALSE(client_control_notifier.get_connection_dropped());

  sctp_network_gateway_config config;
  config.connection_name   = "TEST";
  config.connect_address   = "::1";
  config.connect_port      = 0; // attempt to connect to port 0 which should always fail
  config.non_blocking_mode = true;
  config.reuse_addr        = true;
  create_client(config);
  ASSERT_FALSE(connect());
}

TEST_F(sctp_network_gateway_tester, when_config_valid_then_trx_succeeds)
{
  sctp_network_gateway_config server_config;
  server_config.bind_address = "127.0.0.1";
  server_config.bind_port    = 0;
  server_config.reuse_addr   = true;

  create_server(server_config);
  ASSERT_TRUE(bind_and_listen());
  optional<uint16_t> server_port = server->get_listen_port();
  ASSERT_TRUE(server_port.has_value());
  ASSERT_NE(server_port.value(), 0);

  sctp_network_gateway_config client_config;
  client_config.connection_name   = "TEST";
  client_config.connect_address   = server_config.bind_address;
  client_config.connect_port      = server_port.value();
  client_config.non_blocking_mode = true;

  create_client(client_config);
  start_receive_thread();
  ASSERT_TRUE(connect());

  byte_buffer pdu_small(make_small_tx_byte_buffer());
  send_to_server(pdu_small);
  byte_buffer pdu_large(make_large_tx_byte_buffer());
  send_to_server(pdu_large);
  byte_buffer pdu_oversized(make_oversized_tx_byte_buffer());
  send_to_server(pdu_oversized);

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

TEST_F(sctp_network_gateway_tester, when_v6_config_valid_then_trx_succeeds)
{
  sctp_network_gateway_config server_config;
  server_config.bind_address = "::1";
  server_config.bind_port    = 0;
  server_config.reuse_addr   = true;

  create_server(server_config);
  ASSERT_TRUE(bind_and_listen());
  optional<uint16_t> server_port = server->get_listen_port();
  ASSERT_TRUE(server_port.has_value());
  ASSERT_NE(server_port.value(), 0);

  sctp_network_gateway_config client_config;
  client_config.connection_name   = "TEST";
  client_config.connect_address   = server_config.bind_address;
  client_config.connect_port      = server_port.value();
  client_config.non_blocking_mode = true;

  create_client(client_config);
  start_receive_thread();
  ASSERT_TRUE(connect());

  byte_buffer pdu_small(make_small_tx_byte_buffer());
  send_to_server(pdu_small);
  byte_buffer pdu_large(make_large_tx_byte_buffer());
  send_to_server(pdu_large);
  byte_buffer pdu_oversized(make_oversized_tx_byte_buffer());
  send_to_server(pdu_oversized);

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

TEST_F(sctp_network_gateway_tester, when_hostname_resolved_then_trx_succeeds)
{
  sctp_network_gateway_config server_config;
  server_config.bind_address = "localhost";
  server_config.bind_port    = 0;
  server_config.reuse_addr   = true;

  create_server(server_config);
  ASSERT_TRUE(bind_and_listen());
  optional<uint16_t> server_port = server->get_listen_port();
  ASSERT_TRUE(server_port.has_value());
  ASSERT_NE(server_port.value(), 0);

  sctp_network_gateway_config client_config;
  client_config.connection_name   = "TEST";
  client_config.connect_address   = server_config.bind_address;
  client_config.connect_port      = server_port.value();
  client_config.non_blocking_mode = true;

  create_client(client_config);
  start_receive_thread();
  ASSERT_TRUE(connect());

  byte_buffer pdu_small(make_small_tx_byte_buffer());
  send_to_server(pdu_small);
  byte_buffer pdu_large(make_large_tx_byte_buffer());
  send_to_server(pdu_large);
  byte_buffer pdu_oversized(make_oversized_tx_byte_buffer());
  send_to_server(pdu_oversized);

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

TEST_F(sctp_network_gateway_tester, when_rto_is_set_then_rto_changes)
{
  // Test RTO values
  uint32_t rto_init = 120;
  uint32_t rto_min  = 120;
  uint32_t rto_max  = 800;

  sctp_network_gateway_config server_config;
  server_config.bind_address = "127.0.0.1";
  server_config.bind_port    = 0;
  server_config.reuse_addr   = true;
  server_config.rto_initial  = rto_init;
  server_config.rto_min      = rto_min;
  server_config.rto_max      = rto_max;

  create_server(server_config);

  ASSERT_TRUE(server->create_and_bind());
  int fd = server->get_socket_fd();
  fmt::print("{}\n", fd);

  // Check used RTO values
  sctp_rtoinfo rto_opts  = {};
  socklen_t    rto_sz    = sizeof(sctp_rtoinfo);
  rto_opts.srto_assoc_id = 0;
  ASSERT_EQ(getsockopt(fd, SOL_SCTP, SCTP_RTOINFO, &rto_opts, &rto_sz), 0) << strerror(errno);
  ASSERT_EQ(rto_opts.srto_initial, rto_init);
  ASSERT_EQ(rto_opts.srto_min, rto_min);
  ASSERT_EQ(rto_opts.srto_max, rto_max);
}

TEST_F(sctp_network_gateway_tester, when_init_msg_is_set_then_init_msg_changes)
{
  // Test RTO values
  uint32_t init_max_attempts = 1;
  uint32_t max_init_timeo    = 120;

  sctp_network_gateway_config server_config;
  server_config.bind_address      = "127.0.0.1";
  server_config.bind_port         = 0;
  server_config.reuse_addr        = true;
  server_config.init_max_attempts = init_max_attempts;
  server_config.max_init_timeo    = max_init_timeo;

  create_server(server_config);

  ASSERT_TRUE(server->create_and_bind());
  int fd = server->get_socket_fd();
  fmt::print("{}\n", fd);

  // Check used SCTP_INITMSG values
  sctp_initmsg init_opts = {};
  socklen_t    init_sz   = sizeof(sctp_initmsg);
  ASSERT_EQ(getsockopt(fd, SOL_SCTP, SCTP_INITMSG, &init_opts, &init_sz), 0);

  ASSERT_EQ(init_opts.sinit_max_attempts, init_max_attempts);
  ASSERT_EQ(init_opts.sinit_max_init_timeo, max_init_timeo);
}

TEST_F(sctp_network_gateway_tester, when_connection_loss_then_reconnect)
{
  // TODO: Add test for reconnect
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
