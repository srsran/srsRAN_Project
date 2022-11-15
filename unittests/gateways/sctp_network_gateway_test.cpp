/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include <functional> // for std::function/std::bind
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>

#include "srsgnb/gateways/network_gateway_factory.h"

using namespace srsgnb;

static const std::string tx_buf = "Hello World!";

byte_buffer make_tx_byte_buffer()
{
  byte_buffer pdu{};
  for (unsigned i = 0; i != tx_buf.size(); ++i) {
    pdu.append((uint8_t)tx_buf[i]);
  }
  return pdu;
}

class dummy_network_gateway_control_notifier : public network_gateway_control_notifier
{
public:
  dummy_network_gateway_control_notifier() = default;

  void on_connection_loss() override { conn_dropped = true; }
  void on_connection_established() override { conn_established = true; }
  bool get_connection_dropped() { return conn_dropped; }
  bool get_connection_established() { return conn_established; }

private:
  bool conn_established = false;
  bool conn_dropped     = false;
};

class dummy_network_gateway_data_notifier : public network_gateway_data_notifier
{
public:
  dummy_network_gateway_data_notifier() = default;
  void on_new_pdu(byte_buffer pdu) override
  {
    // printf("Received PDU\n");
    rx_bytes += pdu.length();
  }

  unsigned get_rx_bytes() { return rx_bytes; }

private:
  unsigned rx_bytes = 0;
};

class sctp_network_gateway_tester : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // init GW logger
    srslog::fetch_basic_logger("SCTP-NW-GW", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("SCTP-NW-GW", false).set_hex_dump_max_size(100);
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

  bool bind_and_listen()
  {
    if (server->create_and_bind() != true) {
      return false;
    }

    if (server->listen() != true) {
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
  std::unique_ptr<network_gateway> server, client;

  std::thread       rx_thread;
  std::atomic<bool> stop_token = {false};
};

TEST_F(sctp_network_gateway_tester, when_binding_on_bogus_address_then_bind_fails)
{
  network_gateway_config config;
  config.bind_address = "1.1.1.1";
  config.bind_port    = 8888;
  config.reuse_addr   = true;
  set_config(config, config);
  ASSERT_FALSE(bind_and_listen());
}

TEST_F(sctp_network_gateway_tester, when_binding_on_bogus_v6_address_then_bind_fails)
{
  network_gateway_config config;
  config.bind_address = "1:1::";
  config.bind_port    = 8888;
  config.reuse_addr   = true;
  set_config(config, config);
  ASSERT_FALSE(bind_and_listen());
}

TEST_F(sctp_network_gateway_tester, when_binding_on_localhost_then_bind_succeeds)
{
  network_gateway_config config;
  config.bind_address = "127.0.0.1";
  config.bind_port    = 8888;
  config.reuse_addr   = true;
  set_config(config, config);
  ASSERT_TRUE(bind_and_listen());
}

TEST_F(sctp_network_gateway_tester, when_binding_on_v6_localhost_then_bind_succeeds)
{
  network_gateway_config config;
  config.bind_address = "::1";
  config.bind_port    = 8888;
  config.reuse_addr   = true;
  set_config(config, config);
  ASSERT_TRUE(bind_and_listen());
}

TEST_F(sctp_network_gateway_tester, when_socket_not_exists_then_connect_fails)
{
  ASSERT_FALSE(client_control_notifier.get_connection_dropped());

  network_gateway_config config;
  config.connect_address   = "127.0.0.1";
  config.connect_port      = 6666;
  config.non_blocking_mode = true;
  config.reuse_addr        = true;
  set_config(config, config);

  ASSERT_FALSE(connect());
}

TEST_F(sctp_network_gateway_tester, when_v6_socket_not_exists_then_connect_fails)
{
  ASSERT_FALSE(client_control_notifier.get_connection_dropped());

  network_gateway_config config;
  config.connect_address   = "::1";
  config.connect_port      = 6666;
  config.non_blocking_mode = true;
  config.reuse_addr        = true;
  set_config(config, config);

  ASSERT_FALSE(connect());
}

TEST_F(sctp_network_gateway_tester, when_config_valid_then_trx_succeeds)
{
  network_gateway_config server_config;
  server_config.bind_address = "127.0.0.1";
  server_config.bind_port    = 7777;
  server_config.reuse_addr   = true;

  network_gateway_config client_config;
  client_config.connect_address   = "127.0.0.1";
  client_config.connect_port      = 7777;
  client_config.non_blocking_mode = true;
  set_config(server_config, client_config);

  ASSERT_TRUE(bind_and_listen());
  start_receive_thread();
  ASSERT_TRUE(connect());

  byte_buffer pdu(make_tx_byte_buffer());
  send_to_server(pdu);

  // let the Rx thread pick up the message
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  ASSERT_EQ(server_data_notifier.get_rx_bytes(), tx_buf.size());
}

TEST_F(sctp_network_gateway_tester, when_v6_config_valid_then_trx_succeeds)
{
  network_gateway_config server_config;
  server_config.bind_address = "::1";
  server_config.bind_port    = 7777;
  server_config.reuse_addr   = true;

  network_gateway_config client_config;
  client_config.connect_address   = "::1";
  client_config.connect_port      = 7777;
  client_config.non_blocking_mode = true;
  set_config(server_config, client_config);

  ASSERT_TRUE(bind_and_listen());
  start_receive_thread();
  ASSERT_TRUE(connect());

  byte_buffer pdu(make_tx_byte_buffer());
  send_to_server(pdu);

  // let the Rx thread pick up the message
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  ASSERT_EQ(server_data_notifier.get_rx_bytes(), tx_buf.size());
}

TEST_F(sctp_network_gateway_tester, when_hostname_resolved_then_trx_succeeds)
{
  network_gateway_config server_config;
  server_config.bind_address = "localhost";
  server_config.bind_port    = 5555;
  server_config.reuse_addr   = true;

  network_gateway_config client_config;
  client_config.connect_address   = "localhost";
  client_config.connect_port      = 5555;
  client_config.non_blocking_mode = true;
  set_config(server_config, client_config);

  ASSERT_TRUE(bind_and_listen());
  start_receive_thread();
  ASSERT_TRUE(connect());

  byte_buffer pdu(make_tx_byte_buffer());
  send_to_server(pdu);

  // let the Rx thread pick up the message
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  ASSERT_EQ(server_data_notifier.get_rx_bytes(), tx_buf.size());
}

TEST_F(sctp_network_gateway_tester, when_connection_loss_then_reconnect)
{
  // TODO: Add test for reconnect
}
