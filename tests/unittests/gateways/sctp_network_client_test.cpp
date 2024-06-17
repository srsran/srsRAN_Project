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

#include "sctp_test_helpers.h"
#include "srsran/gateways/sctp_network_client_factory.h"
#include "srsran/support/executors/unique_thread.h"
#include "srsran/support/io/sctp_socket.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {

class sctp_recv_notifier_factory
{
public:
  bool        destroyed = false;
  byte_buffer last_sdu;

  class dummy_sctp_recv_notifier : public sctp_association_sdu_notifier
  {
  public:
    dummy_sctp_recv_notifier(sctp_recv_notifier_factory& parent_) : parent(parent_) {}
    ~dummy_sctp_recv_notifier() { parent.destroyed = true; }

    bool on_new_sdu(byte_buffer sdu) override
    {
      parent.last_sdu = std::move(sdu);
      return true;
    }

  private:
    sctp_recv_notifier_factory& parent;
  };

  std::unique_ptr<sctp_association_sdu_notifier> create() { return std::make_unique<dummy_sctp_recv_notifier>(*this); }
};

class dummy_sctp_server : public dummy_sctp_node
{
public:
  dummy_sctp_server() : dummy_sctp_node("SERVER")
  {
    auto result = sctp_socket::create(sctp_socket_params{"SERVER", AF_INET, SOCK_SEQPACKET});
    report_fatal_error_if_not(result.has_value(), "Failed to create SCTP socket");
    socket = std::move(result.value());

    sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(0);
    addr.sin_addr.s_addr = ::inet_addr(address.c_str());
    report_fatal_error_if_not(socket.bind((struct sockaddr&)addr, sizeof(addr), ""), "Failed to bind server socket");
    report_fatal_error_if_not(socket.listen(), "Failed to listen to new connections");
    report_fatal_error_if_not(socket.set_non_blocking(), "Failed to set as non-blocking");
    bind_port = socket.get_listen_port().value();
  }

  const std::string address   = "127.0.0.1";
  int               bind_port = -1;
};

} // namespace

class sctp_network_client_test : public ::testing::Test
{
public:
  sctp_network_client_test()
  {
    srslog::fetch_basic_logger("SCTP-GW").set_level(srslog::basic_levels::debug);
    srslog::init();

    client_cfg.sctp.ppid = NGAP_PPID;
  }
  ~sctp_network_client_test()
  {
    close_client();
    srslog::flush();
  }

  // Forces client shutdown.
  void close_client()
  {
    // We may need io_broker running asynchronously to complete SCTP event handling and shutdown.
    if (client_sender != nullptr) {
      std::atomic<bool> finished{false};
      // Handle SCTP SHUTDOWN
      unique_thread t("io_broker", [this, &finished]() {
        while (not finished and broker.handle_receive) {
          broker.handle_receive();
          std::this_thread::sleep_for(std::chrono::microseconds{10});
        }
      });
      // client dtor waits for SCTP SHUTDOWN COMP before returning.
      client.reset();
      finished = true;
      t.join();
    }
  }

  void trigger_broker() { broker.handle_receive(); }

  bool connect_to_server(const std::string& server_addr, int server_port)
  {
    auto sender = client->connect_to("server", server_addr, server_port, recv_notifier_factory.create());
    if (sender != nullptr) {
      client_sender = std::move(sender);
      return true;
    }
    return false;
  }

  bool send_data(const std::vector<uint8_t>& data)
  {
    return client_sender->on_new_sdu(byte_buffer::create(data).value());
  }

protected:
  dummy_io_broker            broker;
  sctp_network_client_config client_cfg{{.if_name = "client"}, broker};

  sctp_recv_notifier_factory recv_notifier_factory;

  dummy_sctp_server server;

  std::unique_ptr<sctp_network_client> client;

  std::unique_ptr<sctp_association_sdu_notifier> client_sender;
};

TEST_F(sctp_network_client_test, when_bind_address_is_valid_then_client_is_created_successfully)
{
  client_cfg.sctp.bind_address = "127.0.0.2";
  client_cfg.sctp.bind_port    = server.bind_port;
  client                       = create_sctp_network_client(client_cfg);
  ASSERT_NE(client, nullptr);
}

TEST_F(sctp_network_client_test, when_bind_address_not_provided_then_client_is_created_successfully)
{
  client = create_sctp_network_client(client_cfg);
  ASSERT_NE(client, nullptr);
}

TEST_F(sctp_network_client_test, when_bind_interface_is_invalid_then_server_is_not_created)
{
  client_cfg.sctp.bind_interface = "invalid";
  client_cfg.sctp.bind_address   = "127.0.0.1";
  client_cfg.sctp.bind_port      = server.bind_port;
  client                         = create_sctp_network_client(client_cfg);
  ASSERT_EQ(client, nullptr);
}

TEST_F(sctp_network_client_test, when_server_does_not_exist_then_connection_fails)
{
  client = create_sctp_network_client(client_cfg);

  ASSERT_FALSE(connect_to_server("127.0.0.2", server.bind_port));
  ASSERT_EQ(broker.last_registered_fd, -1);
  ASSERT_EQ(client_sender, nullptr);
  ASSERT_EQ(broker.last_unregistered_fd, -1);
}

TEST_F(sctp_network_client_test, when_broker_rejects_subscriber_then_connect_fails)
{
  broker.accept_next_fd = false;
  client                = create_sctp_network_client(client_cfg);

  ASSERT_FALSE(connect_to_server(server.address, server.bind_port));
  ASSERT_GE(broker.last_registered_fd, 0);
  ASSERT_EQ(broker.last_unregistered_fd, -1) << "If the subscription fails, no deregister should be called";
}

TEST_F(sctp_network_client_test, when_server_exists_then_connection_succeeds)
{
  client = create_sctp_network_client(client_cfg);

  ASSERT_TRUE(connect_to_server(server.address, server.bind_port));
  ASSERT_EQ(broker.last_registered_fd, client->get_socket_fd());
  ASSERT_NE(client_sender, nullptr);
  ASSERT_EQ(broker.last_unregistered_fd, -1);

  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_TRUE(server_recv.has_value());
  ASSERT_TRUE(server_recv.value().has_notification());
  ASSERT_EQ(server_recv.value().sctp_notification(), SCTP_ASSOC_CHANGE);
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Server does not receive more data.
  server_recv = server.receive();
  ASSERT_FALSE(server_recv.has_value());

  // Client receives SCTP_COMM_UP
  trigger_broker();

  // Client sends EOF
  client_sender = nullptr;

  // Client receives SCTP_COMM_COMP and closes connection.
  trigger_broker();
}

TEST_F(sctp_network_client_test, when_client_binds_address_then_connection_succeeds)
{
  client_cfg.sctp.bind_address = "127.0.0.2";
  client                       = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server(server.address, server.bind_port));
}

TEST_F(sctp_network_client_test, when_client_sends_data_then_server_receives_it)
{
  client = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server(server.address, server.bind_port));

  std::vector<uint8_t> sent_data = {0x1, 0x2, 0x3};
  ASSERT_TRUE(send_data(sent_data));

  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Server receives data.
  server_recv = server.receive();
  ASSERT_TRUE(server_recv.has_value());
  ASSERT_TRUE(server_recv.value().has_data());
  ASSERT_EQ(server_recv.value().data, sent_data);
}

TEST_F(sctp_network_client_test, when_server_sends_data_then_client_receives_it)
{
  client = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server(server.address, server.bind_port));

  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Client receives SCTP_COMM_UP
  trigger_broker();

  std::vector<uint8_t> sent_data = {0x1, 0x2, 0x3};
  ASSERT_TRUE(server.send_data(sent_data,
                               client_cfg.sctp.ppid,
                               (const struct sockaddr&)server_recv->msg_src_addr,
                               server_recv->msg_src_addrlen));

  // Client receives data.
  ASSERT_TRUE(recv_notifier_factory.last_sdu.empty());
  trigger_broker();
  ASSERT_EQ(recv_notifier_factory.last_sdu, sent_data);
}

TEST_F(sctp_network_client_test, when_client_sender_is_destroyed_then_client_sends_eof)
{
  client = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server(server.address, server.bind_port));

  // Client processes SCTP COMM UP
  trigger_broker();

  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Client wants to shut down connection.
  client_sender.reset();

  // Server receives SCTP COMM SHUTDOWN
  server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_notification(), SCTP_SHUTDOWN_EVENT);

  // Server receives SCTP SHUTDOWN COMP
  server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_SHUTDOWN_COMP);

  // Client receives an SCTP SHUTDOWN COMP
  trigger_broker();
  ASSERT_TRUE(recv_notifier_factory.destroyed);
}

TEST_F(sctp_network_client_test, when_client_is_destroyed_then_server_gets_eof)
{
  client = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server(server.address, server.bind_port));
  // Client processes SCTP COMM UP
  trigger_broker();
  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  close_client();

  // Server receives SCTP COMM SHUTDOWN
  server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_notification(), SCTP_SHUTDOWN_EVENT);

  // Server receives SCTP SHUTDOWN COMP
  server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_SHUTDOWN_COMP);
}

TEST_F(sctp_network_client_test, when_server_is_destroyed_then_client_receives_sctp_shutdown_event)
{
  client = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server(server.address, server.bind_port));
  // Client processes SCTP COMM UP
  trigger_broker();
  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  server.close();

  // Client receives an SCTP SHUTDOWN EVENT
  trigger_broker();
  ASSERT_FALSE(recv_notifier_factory.destroyed);

  // Client receives an SCTP SHUTDOWN COMP
  trigger_broker();
  ASSERT_TRUE(recv_notifier_factory.destroyed);
}

TEST_F(sctp_network_client_test, when_server_sends_eof_then_client_receives_sctp_shutdown_event)
{
  client = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server(server.address, server.bind_port));
  // Client processes SCTP COMM UP
  trigger_broker();
  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Server sends EOF
  ASSERT_TRUE(server.send_eof(
      client_cfg.sctp.ppid, (const struct sockaddr&)server_recv->msg_src_addr, server_recv->msg_src_addrlen));

  // Client receives an SCTP SHUTDOWN EVENT
  trigger_broker();
  ASSERT_FALSE(recv_notifier_factory.destroyed);

  // Client receives an SCTP SHUTDOWN COMP
  trigger_broker();
  ASSERT_TRUE(recv_notifier_factory.destroyed);

  // Server receives SCTP SHUTDOWN COMP
  server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_SHUTDOWN_COMP);
}

TEST_F(sctp_network_client_test, when_client_sends_eof_before_processing_incoming_eof_then_shutdown_is_successful)
{
  client = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server(server.address, server.bind_port));
  // Client processes SCTP COMM UP
  trigger_broker();
  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Server sends EOF
  ASSERT_TRUE(server.send_eof(
      client_cfg.sctp.ppid, (const struct sockaddr&)server_recv->msg_src_addr, server_recv->msg_src_addrlen));

  // Client sends EOF
  client_sender.reset();

  // Client receives an SCTP SHUTDOWN EVENT
  trigger_broker();
  ASSERT_FALSE(recv_notifier_factory.destroyed);

  // Client receives an SCTP SHUTDOWN COMP
  trigger_broker();
  ASSERT_TRUE(recv_notifier_factory.destroyed);

  // Server receives SCTP SHUTDOWN COMP
  server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_SHUTDOWN_COMP);
}
