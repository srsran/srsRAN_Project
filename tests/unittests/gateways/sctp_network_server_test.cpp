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
    dummy_sctp_recv_notifier(association_factory& parent_, sctp_association_pdu_notifier* send_notifier_) :
      parent(parent_), send_notifier(send_notifier_)
    {
    }
    ~dummy_sctp_recv_notifier()
    {
      for (auto& sender : parent.association_senders) {
        if (sender.get() == send_notifier) {
          parent.association_senders.erase(parent.association_senders.begin());
          break;
        }
      }
      parent.association_destroyed = true;
    }

    bool on_new_pdu(byte_buffer pdu) override
    {
      parent.last_pdu = std::move(pdu);
      return true;
    }

  private:
    association_factory&           parent;
    sctp_association_pdu_notifier* send_notifier;
  };

  std::vector<std::unique_ptr<sctp_association_pdu_notifier>> association_senders;
  bool                                                        association_created   = false;
  bool                                                        association_destroyed = false;

  std::unique_ptr<sctp_association_pdu_notifier>
  create(std::unique_ptr<sctp_association_pdu_notifier> send_notifier) override
  {
    association_created = true;
    association_senders.push_back(std::move(send_notifier));
    return std::make_unique<dummy_sctp_recv_notifier>(*this, association_senders.back().get());
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

struct recv_data {
  struct sctp_sndrcvinfo sri       = {};
  int                    msg_flags = 0;
  sockaddr_storage       msg_src_addr;
  // fromlen is an in/out variable in sctp_recvmsg.
  socklen_t            msg_src_addrlen = sizeof(msg_src_addr);
  std::vector<uint8_t> data;

  bool has_notification() const { return msg_flags & MSG_NOTIFICATION; }
  int  sctp_notification() const
  {
    srsran_assert(has_notification(), "bad access");
    const auto* notif = reinterpret_cast<const union sctp_notification*>(data.data());
    return notif->sn_header.sn_type;
  }
  const struct sctp_assoc_change& sctp_assoc_change() const
  {
    srsran_assert(has_notification() and sctp_notification() == SCTP_ASSOC_CHANGE, "bad access");
    const auto* notif = reinterpret_cast<const union sctp_notification*>(data.data());
    return notif->sn_assoc_change;
  }
};

class dummy_sctp_client
{
public:
  dummy_sctp_client() { logger.set_level(srslog::basic_levels::debug); }
  ~dummy_sctp_client() { close(); }

  bool close()
  {
    if (socket.is_open()) {
      socket.close();
      logger.info("Client shut down");
      return true;
    }
    return false;
  }

  void set_dest(const sctp_network_node_config& server_cfg_) { server_cfg = server_cfg_; }

  bool connect()
  {
    auto result = sctp_socket::create(sctp_socket_params{AF_INET, SOCK_STREAM});
    if (not result.has_value()) {
      return false;
    }
    socket = std::move(result.value());
    sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(server_cfg.bind_port);
    addr.sin_addr.s_addr = ::inet_addr(server_cfg.bind_address.c_str());
    if (not socket.connect((struct sockaddr&)addr, sizeof(addr))) {
      socket.close();
      return false;
    }
    logger.info("Client connected to {}:{}", server_cfg.bind_address, server_cfg.bind_port);
    return true;
  }

  optional<recv_data> receive()
  {
    constexpr static uint32_t network_gateway_sctp_max_len = 9100;

    recv_data data;

    std::array<uint8_t, network_gateway_sctp_max_len> temp_buf;
    int                                               rx_bytes = ::sctp_recvmsg(socket.fd().value(),
                                  temp_buf.data(),
                                  temp_buf.size(),
                                  (struct sockaddr*)&data.msg_src_addr,
                                  &data.msg_src_addrlen,
                                  &data.sri,
                                  &data.msg_flags);
    if (rx_bytes < 0) {
      return nullopt;
    }

    data.data.assign(temp_buf.begin(), temp_buf.begin() + rx_bytes);
    return data;
  }

  bool send_data(const std::vector<uint8_t>& bytes)
  {
    sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(server_cfg.bind_port);
    addr.sin_addr.s_addr = ::inet_addr(server_cfg.bind_address.c_str());
    int bytes_sent       = ::sctp_sendmsg(socket.fd().value(),
                                    bytes.data(),
                                    bytes.size(),
                                    (struct sockaddr*)&addr,
                                    sizeof(addr),
                                    htonl(server_cfg.ppid),
                                    0,
                                    0,
                                    0,
                                    0);
    return bytes_sent == (int)bytes.size();
  }

  sctp_socket              socket;
  sctp_network_node_config server_cfg;
  srslog::basic_logger&    logger = srslog::fetch_basic_logger("CLIENT");
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

    client.set_dest(server_cfg.sctp);
  }

  bool connect_client(bool broker_trigger_required = true)
  {
    if (not client.connect()) {
      return false;
    }
    if (broker_trigger_required) {
      trigger_broker();
    }
    return true;
  }

  bool close_client(bool broker_trigger_required = true)
  {
    bool ret = client.close();
    if (broker_trigger_required) {
      trigger_broker();
    }
    return ret;
  }

  bool send_data(const std::vector<uint8_t>& bytes, bool broker_triggered = true)
  {
    if (not client.send_data(bytes)) {
      return false;
    }
    if (broker_triggered) {
      trigger_broker();
    }
    return true;
  }

  void trigger_broker() { broker.handle_receive(); }

  sctp_network_server_config server_cfg{{}, broker, assoc_factory};

  std::unique_ptr<sctp_network_server> server;
  dummy_sctp_client                    client;
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

TEST_F(sctp_network_server_test, when_client_connects_then_server_request_new_sctp_association_handler_creation)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());

  // Connect client to server.
  ASSERT_TRUE(connect_client());

  // Ensure SCTP server requested the creation of a new SCTP association handler.
  ASSERT_TRUE(assoc_factory.association_created);
  ASSERT_FALSE(assoc_factory.association_destroyed);
}

TEST_F(sctp_network_server_test, when_client_connects_then_client_receives_sctp_comm_up_notification)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());

  // Connect client to server.
  ASSERT_TRUE(connect_client(false));

  // Ensure SCTP ASSOC COMM UP is received by the client, even before the broker handles the SCTP association on the
  // server side.
  auto result = client.receive();
  ASSERT_TRUE(result.has_value());
  ASSERT_TRUE(result->has_notification());
  ASSERT_EQ(result->sctp_notification(), SCTP_ASSOC_CHANGE);
  ASSERT_EQ(result->sctp_assoc_change().sac_state, SCTP_COMM_UP);
  client.logger.info("Client received SCTP COMM UP notification for assoc_id={}",
                     result->sctp_assoc_change().sac_assoc_id);

  // server handles now the SCTP association event.
  trigger_broker();
}

TEST_F(sctp_network_server_test, when_client_disconnects_then_server_deletes_association_handler)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());
  ASSERT_TRUE(connect_client());

  ASSERT_FALSE(assoc_factory.association_destroyed);
  ASSERT_TRUE(close_client());
  ASSERT_TRUE(assoc_factory.association_destroyed);
}

TEST_F(sctp_network_server_test, when_client_sends_sctp_message_then_message_is_forwarded_to_association_handler)
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
       when_client_sends_sctp_message_and_closes_before_server_handles_events_then_events_are_still_handled)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());
  ASSERT_TRUE(connect_client());
  std::vector<uint8_t> bytes = {0x01, 0x02, 0x03, 0x04};

  ASSERT_TRUE(send_data(bytes, false));
  ASSERT_TRUE(close_client(false));

  ASSERT_EQ(assoc_factory.last_pdu.length(), 0);
  trigger_broker(); // Should handle packet receive
  ASSERT_EQ(assoc_factory.last_pdu, bytes);
  ASSERT_FALSE(assoc_factory.association_destroyed) << "Association Handler was destroyed too early";
  trigger_broker(); // Should close connection
  ASSERT_TRUE(assoc_factory.association_destroyed) << "Association Handler was not destroyed";
}

TEST_F(sctp_network_server_test, when_association_handler_closes_connection_then_sctp_eof_is_sent_to_client)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());
  ASSERT_TRUE(connect_client());
  // Client receives SCTP COMM UP
  auto result = client.receive();

  // Server-side association handler closes, signalling that the server wants to close association.
  assoc_factory.association_senders.clear();

  // Ensure SCTP EOF is sent to client.
  result = client.receive();
  ASSERT_TRUE(result.has_value());
  ASSERT_TRUE(result->has_notification());
  ASSERT_EQ(result->sctp_notification(), SCTP_SHUTDOWN_EVENT);

  // IO broker in the server handles SCTP_SHUTDOWN_COMP
  trigger_broker();
}

TEST_F(sctp_network_server_test, when_multiple_clients_connect_then_multiple_association_handlers_are_created)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());

  // First client connect.
  ASSERT_TRUE(connect_client());

  // Client 2 connects.
  assoc_factory.association_created = false;
  dummy_sctp_client client2;
  client2.set_dest(server_cfg.sctp);
  client2.connect();
  // Handle client 2 association creation.
  trigger_broker();
  ASSERT_TRUE(assoc_factory.association_created);
  ASSERT_FALSE(assoc_factory.association_destroyed);

  // Close Client 1.
  client.close();

  // Close Client 2.
  client2.close();

  // SCTP shutdown client 1
  trigger_broker();
  ASSERT_TRUE(assoc_factory.association_destroyed) << "Client 1 shutdown was not processed";
  assoc_factory.association_destroyed = false;
  // SCTP SHUTDOWN_COMP client 1
  trigger_broker();
  // SCTP shutdown client 2
  trigger_broker();
  ASSERT_TRUE(assoc_factory.association_destroyed) << "Client 2 shutdown was not processed";
  // SCTP SHUTDOWN_COMP client 2
  trigger_broker();
}