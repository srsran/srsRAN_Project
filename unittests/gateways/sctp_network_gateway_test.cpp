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

static const std::string tx_buf = "hello world!";

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
  void set_config(network_gateway_config server_config, network_gateway_config client_config)
  {
    server = create_network_gateway({server_config, server_control_notifier, server_data_notifier});
    ASSERT_NE(server, nullptr);
    client = create_network_gateway({client_config, client_control_notifier, client_data_notifier});
    ASSERT_NE(client, nullptr);
  }

  void TearDown() override
  {
    stop_token.store(true, std::memory_order_relaxed);
    if (rx_thread.joinable()) {
      rx_thread.join();
    }
  }

  bool bind_and_listen()
  {
    if (server->bind() != true) {
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

  bool connect() { return client->connect(); }

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

TEST_F(sctp_network_gateway_tester, bind_fails_on_bogus_address)
{
  network_gateway_config config;
  config.bind_address = "1.1.1.1";
  config.connect_port = 8888;
  set_config(config, config);
  ASSERT_FALSE(bind_and_listen());
}

TEST_F(sctp_network_gateway_tester, bind_succeeds_on_localhost)
{
  network_gateway_config config;
  config.bind_address = "127.0.0.1";
  config.connect_port = 8888;
  set_config(config, config);
  ASSERT_TRUE(bind_and_listen());
}

TEST_F(sctp_network_gateway_tester, connect_fails_on_unexisting_socket)
{
  ASSERT_FALSE(client_control_notifier.get_connection_dropped());

  // in non-blocking mode, connects returns immediately
  network_gateway_config config;
  config.connect_address   = "127.0.0.1";
  config.connect_port      = 6666;
  config.non_blocking_mode = true;
  set_config(config, config);

  ASSERT_TRUE(connect());

  // wait for connection to timeout
  run_client_receive();

  ASSERT_TRUE(client_control_notifier.get_connection_dropped());
}

TEST_F(sctp_network_gateway_tester, connection_notification)
{
  network_gateway_config server_config;
  server_config.bind_address = "127.0.0.1";
  server_config.bind_port    = 9999;

  network_gateway_config client_config;
  client_config.connect_address   = "127.0.0.1";
  client_config.connect_port      = 9999;
  client_config.non_blocking_mode = true;
  set_config(server_config, client_config);

  ASSERT_TRUE(bind_and_listen());
  start_receive_thread();
  ASSERT_TRUE(connect());

  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  ASSERT_TRUE(server_control_notifier.get_connection_established());
}

TEST_F(sctp_network_gateway_tester, basic_trx_test)
{
  network_gateway_config server_config;
  server_config.bind_address = "127.0.0.1";
  server_config.bind_port    = 7777;

  network_gateway_config client_config;
  client_config.connect_address   = "127.0.0.1";
  client_config.connect_port      = 7777;
  client_config.non_blocking_mode = true;
  set_config(server_config, client_config);

  ASSERT_TRUE(bind_and_listen());
  start_receive_thread();
  ASSERT_TRUE(connect());

  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  ASSERT_TRUE(server_control_notifier.get_connection_established());

  byte_buffer pdu(tx_buf.begin(), tx_buf.end());
  send_to_server(pdu);

  // let the Rx thread pick up the message
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  ASSERT_EQ(server_data_notifier.get_rx_bytes(), tx_buf.length());
}

TEST_F(sctp_network_gateway_tester, basic_trx_test_v6)
{
  network_gateway_config server_config;
  server_config.bind_address = "::1";
  server_config.bind_port    = 4444;
  network_gateway_config client_config;
  client_config.connect_address   = "::1";
  client_config.connect_port      = 4444;
  client_config.non_blocking_mode = true;
  set_config(server_config, client_config);

  ASSERT_TRUE(bind_and_listen());
  start_receive_thread();
  ASSERT_TRUE(connect());

  byte_buffer pdu(tx_buf.begin(), tx_buf.end());
  send_to_server(pdu);

  // let the Rx thread pick up the message
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  ASSERT_EQ(server_data_notifier.get_rx_bytes(), tx_buf.length());
}
