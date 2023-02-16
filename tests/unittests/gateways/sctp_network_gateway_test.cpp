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
#include "srsgnb/gateways/sctp_network_gateway_factory.h"

using namespace srsgnb;

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

  void set_config(sctp_network_gateway_config server_config, sctp_network_gateway_config client_config)
  {
    server = create_sctp_network_gateway({server_config, server_control_notifier, server_data_notifier});
    ASSERT_NE(server, nullptr);
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

protected:
  dummy_network_gateway_control_notifier server_control_notifier;
  dummy_network_gateway_control_notifier client_control_notifier;

  dummy_network_gateway_data_notifier server_data_notifier;
  dummy_network_gateway_data_notifier client_data_notifier;

private:
  std::unique_ptr<sctp_network_gateway> server, client;

  std::thread       rx_thread;
  std::atomic<bool> stop_token = {false};
};

TEST_F(sctp_network_gateway_tester, when_binding_on_bogus_address_then_bind_fails)
{
  sctp_network_gateway_config config;
  config.bind_address = "1.1.1.1";
  config.bind_port    = get_unused_sctp_port("0.0.0.0");
  config.reuse_addr   = true;
  ASSERT_NE(config.bind_port, 0);
  set_config(config, config);
  ASSERT_FALSE(bind_and_listen());
}

TEST_F(sctp_network_gateway_tester, when_binding_on_bogus_v6_address_then_bind_fails)
{
  sctp_network_gateway_config config;
  config.bind_address = "1:1::";
  config.bind_port    = get_unused_sctp_port("::1");
  config.reuse_addr   = true;
  ASSERT_NE(config.bind_port, 0);
  set_config(config, config);
  ASSERT_FALSE(bind_and_listen());
}

TEST_F(sctp_network_gateway_tester, when_binding_on_localhost_then_bind_succeeds)
{
  sctp_network_gateway_config config;
  config.bind_address = "127.0.0.1";
  config.bind_port    = get_unused_sctp_port(config.bind_address);
  config.reuse_addr   = true;
  ASSERT_NE(config.bind_port, 0);
  set_config(config, config);
  ASSERT_TRUE(bind_and_listen());
}

TEST_F(sctp_network_gateway_tester, when_binding_on_v6_localhost_then_bind_succeeds)
{
  sctp_network_gateway_config config;
  config.bind_address = "::1";
  config.bind_port    = get_unused_sctp_port(config.bind_address);
  config.reuse_addr   = true;
  ASSERT_NE(config.bind_port, 0);
  set_config(config, config);
  ASSERT_TRUE(bind_and_listen());
}

TEST_F(sctp_network_gateway_tester, when_socket_not_exists_then_connect_fails)
{
  ASSERT_FALSE(client_control_notifier.get_connection_dropped());

  sctp_network_gateway_config config;
  config.connect_address   = "127.0.0.1";
  config.connect_port      = get_unused_sctp_port(config.connect_address);
  config.non_blocking_mode = true;
  config.reuse_addr        = true;
  ASSERT_NE(config.connect_port, 0);
  set_config(config, config);

  ASSERT_FALSE(connect());
}

TEST_F(sctp_network_gateway_tester, when_v6_socket_not_exists_then_connect_fails)
{
  ASSERT_FALSE(client_control_notifier.get_connection_dropped());

  sctp_network_gateway_config config;
  config.connect_address   = "::1";
  config.connect_port      = get_unused_sctp_port(config.connect_address);
  config.non_blocking_mode = true;
  config.reuse_addr        = true;
  ASSERT_NE(config.connect_port, 0);
  set_config(config, config);

  ASSERT_FALSE(connect());
}

TEST_F(sctp_network_gateway_tester, when_config_valid_then_trx_succeeds)
{
  sctp_network_gateway_config server_config;
  server_config.bind_address = "127.0.0.1";
  server_config.bind_port    = get_unused_sctp_port(server_config.bind_address);
  server_config.reuse_addr   = true;
  ASSERT_NE(server_config.bind_port, 0);

  sctp_network_gateway_config client_config;
  client_config.connect_address   = server_config.bind_address;
  client_config.connect_port      = server_config.bind_port;
  client_config.non_blocking_mode = true;
  set_config(server_config, client_config);

  ASSERT_TRUE(bind_and_listen());
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
  server_config.bind_port    = get_unused_sctp_port(server_config.bind_address);
  server_config.reuse_addr   = true;
  ASSERT_NE(server_config.bind_port, 0);

  sctp_network_gateway_config client_config;
  client_config.connect_address   = server_config.bind_address;
  client_config.connect_port      = server_config.bind_port;
  client_config.non_blocking_mode = true;
  set_config(server_config, client_config);

  ASSERT_TRUE(bind_and_listen());
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
  server_config.bind_port    = get_unused_sctp_port(server_config.bind_address);
  server_config.reuse_addr   = true;
  ASSERT_NE(server_config.bind_port, 0);

  sctp_network_gateway_config client_config;
  client_config.connect_address   = server_config.bind_address;
  client_config.connect_port      = server_config.bind_port;
  client_config.non_blocking_mode = true;
  set_config(server_config, client_config);

  ASSERT_TRUE(bind_and_listen());
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

TEST_F(sctp_network_gateway_tester, when_connection_loss_then_reconnect)
{
  // TODO: Add test for reconnect
}
