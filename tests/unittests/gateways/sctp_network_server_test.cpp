/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/gateways/sctp_network_server_factory.h"
#include "srsran/support/io/io_broker.h"
#include "srsran/support/io/sctp_socket.h"
#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <netinet/sctp.h>

using namespace srsran;

namespace {

class dummy_io_broker : public io_broker
{
public:
  bool             accept_next_fd     = true;
  int              last_registered_fd = -1;
  recv_callback_t  handle_receive;
  error_callback_t handle_error;
  int              last_unregistered_fd = -1;

  subscriber register_fd(
      int              fd,
      recv_callback_t  handler_,
      error_callback_t err_handler_ = [](error_code) {}) override
  {
    last_registered_fd = fd;
    if (not accept_next_fd) {
      return {};
    }
    handle_receive = handler_;
    handle_error   = err_handler_;
    return subscriber{*this, fd};
  }

  bool unregister_fd(int fd) override
  {
    last_unregistered_fd = fd;
    return true;
  }
};

class association_factory : public sctp_network_association_factory
{
public:
  byte_buffer last_pdu;

  class dummy_sctp_recv_notifier : public sctp_association_pdu_notifier
  {
  public:
    dummy_sctp_recv_notifier(association_factory& parent_) : parent(parent_) {}
    ~dummy_sctp_recv_notifier() { parent.association_destroyed = true; }

    void on_new_pdu(byte_buffer pdu) override { parent.last_pdu = std::move(pdu); }

  private:
    association_factory& parent;
  };

  bool association_created   = false;
  bool association_destroyed = false;

  std::unique_ptr<sctp_association_pdu_notifier> create() override
  {
    association_created = true;
    return std::make_unique<dummy_sctp_recv_notifier>(*this);
  }
};

class base_sctp_network_test
{
protected:
  base_sctp_network_test()
  {
    srslog::fetch_basic_logger("SCTP-GW").set_level(srslog::basic_levels::debug);
    srslog::init();
  }
  ~base_sctp_network_test() { srslog::flush(); }

  dummy_io_broker     broker;
  association_factory assoc_factory;
};

} // namespace

class sctp_network_server_test : public base_sctp_network_test, public ::testing::Test
{
protected:
  sctp_network_server_test()
  {
    server_cfg.sctp.ppid         = NGAP_PPID;
    server_cfg.sctp.bind_address = "127.0.0.1";
    server_cfg.sctp.bind_port    = 38412;
  }

  bool connect_client()
  {
    auto result = sctp_socket::create(sctp_socket_params{AF_INET, SOCK_STREAM});
    EXPECT_TRUE(result.has_value());
    client = std::move(result.value());
    sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(server_cfg.sctp.bind_port);
    addr.sin_addr.s_addr = ::inet_addr(server_cfg.sctp.bind_address.c_str());
    if (not client.connect((struct sockaddr&)addr, sizeof(addr))) {
      client.close();
      return false;
    }
    broker.handle_receive();
    return true;
  }

  bool close_client(bool broker_triggered = true)
  {
    bool ret = client.close();
    if (broker_triggered) {
      broker.handle_receive();
    }
    return ret;
  }

  bool send_data(const std::vector<uint8_t>& bytes, bool broker_triggered = true)
  {
    sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(server_cfg.sctp.bind_port);
    addr.sin_addr.s_addr = ::inet_addr(server_cfg.sctp.bind_address.c_str());
    int bytes_sent       = ::sctp_sendmsg(client.fd().value(),
                                    bytes.data(),
                                    bytes.size(),
                                    (struct sockaddr*)&addr,
                                    sizeof(addr),
                                    htonl(server_cfg.sctp.ppid),
                                    0,
                                    0,
                                    0,
                                    0);
    if (broker_triggered) {
      broker.handle_receive();
    }
    return bytes_sent == (int)bytes.size();
  }

  sctp_network_server_config server_cfg{{}, broker, assoc_factory};

  std::unique_ptr<sctp_network_server> server;
  sctp_socket                          client;
};

TEST_F(sctp_network_server_test, when_config_is_valid_then_server_is_created_successfully)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_NE(server, nullptr);
}

TEST_F(sctp_network_server_test, when_bind_interface_is_invalid_then_server_is_not_created)
{
  server_cfg.sctp.bind_interface = "invalid";
  server                         = create_sctp_network_server(server_cfg);
  ASSERT_EQ(server, nullptr);
}

TEST_F(sctp_network_server_test, when_bind_address_is_empty_then_server_is_not_created)
{
  server_cfg.sctp.bind_address = "";
  server                       = create_sctp_network_server(server_cfg);
  ASSERT_EQ(server, nullptr);
}

TEST_F(sctp_network_server_test, when_broker_rejects_subscriber_then_listen_fails)
{
  broker.accept_next_fd = false;

  server = create_sctp_network_server(server_cfg);

  ASSERT_FALSE(server->listen());
  ASSERT_EQ(broker.last_registered_fd, server->get_socket_fd());
  ASSERT_EQ(broker.last_unregistered_fd, -1) << "If the subscription fails, no deregister should be called";
}

TEST_F(sctp_network_server_test, when_broker_accepts_subscriber_then_listen_succeeds)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());

  ASSERT_EQ(broker.last_registered_fd, server->get_socket_fd());
}

TEST_F(sctp_network_server_test, when_server_is_shutdown_then_fd_is_deregistered_from_broker)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());

  int fd = server->get_socket_fd();
  ASSERT_EQ(broker.last_unregistered_fd, -1);
  server.reset();
  ASSERT_EQ(broker.last_registered_fd, fd);
}

TEST_F(sctp_network_server_test, when_peer_connects_then_server_request_new_sctp_association_handler_creation)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());

  // Connect client to server.
  ASSERT_TRUE(connect_client());

  // Ensure SCTP server requested the creation of a new SCTP association handler.
  ASSERT_TRUE(assoc_factory.association_created);
  ASSERT_FALSE(assoc_factory.association_destroyed);
}

TEST_F(sctp_network_server_test, when_peer_disconnects_then_server_deletes_association_handler)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());
  ASSERT_TRUE(connect_client());

  ASSERT_FALSE(assoc_factory.association_destroyed);
  ASSERT_TRUE(close_client());
  ASSERT_TRUE(assoc_factory.association_destroyed);
}

TEST_F(sctp_network_server_test, when_peer_sends_sctp_message_then_message_is_forwarded_to_association_handler)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());
  ASSERT_TRUE(connect_client());

  ASSERT_EQ(assoc_factory.last_pdu.length(), 0);
  std::vector<uint8_t> bytes = {0x01, 0x02, 0x03, 0x04};
  ASSERT_TRUE(send_data(bytes));

  // Ensure SCTP server forwarded the message to the association handler.
  ASSERT_EQ(assoc_factory.last_pdu, bytes);
}

TEST_F(sctp_network_server_test,
       when_peer_sends_sctp_message_and_closes_before_server_handles_events_then_events_are_still_handled)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());
  ASSERT_TRUE(connect_client());
  std::vector<uint8_t> bytes = {0x01, 0x02, 0x03, 0x04};

  ASSERT_TRUE(send_data(bytes, false));
  ASSERT_TRUE(close_client(false));

  ASSERT_EQ(assoc_factory.last_pdu.length(), 0);
  broker.handle_receive(); // Should handle packet receive
  ASSERT_EQ(assoc_factory.last_pdu, bytes);
  ASSERT_FALSE(assoc_factory.association_destroyed) << "Association Handler was destroyed too early";
  broker.handle_receive(); // Should close connection
  ASSERT_TRUE(assoc_factory.association_destroyed) << "Association Handler was not destroyed";
}
