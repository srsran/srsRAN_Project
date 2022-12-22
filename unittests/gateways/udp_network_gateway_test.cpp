/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "test_helpers.h"
#include "srsgnb/gateways/network_gateway_factory.h"

using namespace srsgnb;

class udp_network_gateway_tester : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // init GW logger
    srslog::fetch_basic_logger("UDP-NW-GW", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("UDP-NW-GW", false).set_hex_dump_max_size(100);
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

  void set_config(network_gateway_config server_config, network_gateway_config client_config)
  {
    server = create_network_gateway({server_config, server_control_notifier, server_data_notifier});
    ASSERT_NE(server, nullptr);
    client = create_network_gateway({client_config, client_control_notifier, client_data_notifier});
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

  void send_to_server(const byte_buffer& pdu) { client->handle_pdu(pdu); }

protected:
  dummy_network_gateway_control_notifier server_control_notifier;
  dummy_network_gateway_control_notifier client_control_notifier;

  dummy_network_gateway_data_notifier server_data_notifier;
  dummy_network_gateway_data_notifier client_data_notifier;

  std::unique_ptr<network_gateway> server, client;

private:
  std::thread       rx_thread;
  std::atomic<bool> stop_token = {false};
};

TEST_F(udp_network_gateway_tester, when_binding_on_bogus_address_then_bind_fails)
{
  network_gateway_config config;
  config.type         = network_gateway_type::udp;
  config.bind_address = "1.1.1.1";
  config.bind_port    = 8888;
  config.reuse_addr   = true;
  set_config(config, config);
  ASSERT_FALSE(server->create_and_bind());
}

TEST_F(udp_network_gateway_tester, when_binding_on_bogus_v6_address_then_bind_fails)
{
  network_gateway_config config;
  config.type         = network_gateway_type::udp;
  config.bind_address = "1:1::";
  config.bind_port    = 8888;
  config.reuse_addr   = true;
  set_config(config, config);
  ASSERT_FALSE(server->create_and_bind());
}

TEST_F(udp_network_gateway_tester, when_binding_on_localhost_then_bind_succeeds)
{
  network_gateway_config config;
  config.type         = network_gateway_type::udp;
  config.bind_address = "127.0.0.1";
  config.bind_port    = 8888;
  config.reuse_addr   = true;
  set_config(config, config);
  ASSERT_TRUE(server->create_and_bind());
}

TEST_F(udp_network_gateway_tester, when_binding_on_v6_localhost_then_bind_succeeds)
{
  network_gateway_config config;
  config.type         = network_gateway_type::udp;
  config.bind_address = "::1";
  config.bind_port    = 8888;
  config.reuse_addr   = true;
  set_config(config, config);
  ASSERT_TRUE(server->create_and_bind());
}

TEST_F(udp_network_gateway_tester, when_config_valid_then_trx_succeeds)
{
  network_gateway_config server_config;
  server_config.type              = network_gateway_type::udp;
  server_config.bind_address      = "127.0.0.1";
  server_config.bind_port         = 7777;
  server_config.connect_address   = "127.0.1.1";
  server_config.connect_port      = 7777;
  server_config.non_blocking_mode = true;

  network_gateway_config client_config;
  client_config.type              = network_gateway_type::udp;
  client_config.bind_address      = "127.0.1.1";
  client_config.bind_port         = 7777;
  client_config.connect_address   = "127.0.0.1";
  client_config.connect_port      = 7777;
  client_config.non_blocking_mode = true;
  set_config(server_config, client_config);

  ASSERT_TRUE(server->create_and_bind());
  ASSERT_TRUE(client->create_and_bind());
  start_receive_thread();

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

TEST_F(udp_network_gateway_tester, when_v6_config_valid_then_trx_succeeds)
{
  network_gateway_config server_config;
  server_config.type              = network_gateway_type::udp;
  server_config.bind_address      = "::1";
  server_config.bind_port         = 7777;
  server_config.connect_address   = "::1";
  server_config.connect_port      = 7778;
  server_config.non_blocking_mode = true;

  network_gateway_config client_config;
  client_config.type              = network_gateway_type::udp;
  client_config.bind_address      = "::1";
  client_config.bind_port         = 7778;
  client_config.connect_address   = "::1";
  client_config.connect_port      = 7777;
  client_config.non_blocking_mode = true;
  set_config(server_config, client_config);

  ASSERT_TRUE(server->create_and_bind());
  ASSERT_TRUE(client->create_and_bind());
  start_receive_thread();

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

TEST_F(udp_network_gateway_tester, when_hostname_resolved_then_trx_succeeds)
{
  network_gateway_config server_config;
  server_config.type              = network_gateway_type::udp;
  server_config.bind_address      = "localhost";
  server_config.bind_port         = 5555;
  server_config.connect_address   = "localhost";
  server_config.connect_port      = 5556;
  server_config.non_blocking_mode = true;
  server_config.reuse_addr        = true;

  network_gateway_config client_config;
  client_config.type              = network_gateway_type::udp;
  client_config.bind_address      = "localhost";
  client_config.bind_port         = 5556;
  client_config.connect_address   = "localhost";
  client_config.connect_port      = 5555;
  client_config.non_blocking_mode = true;
  set_config(server_config, client_config);

  ASSERT_TRUE(server->create_and_bind());
  ASSERT_TRUE(client->create_and_bind());
  start_receive_thread();

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

TEST_F(udp_network_gateway_tester, when_connection_loss_then_reconnect)
{
  // TODO: Add test for reconnect
}
