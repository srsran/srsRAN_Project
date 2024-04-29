/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/gateways/unique_fd.h"
#include <sys/socket.h>

namespace srsran {

constexpr uint32_t network_gateway_sctp_max_len = 9100;

class sctp_network_gateway_impl final : public sctp_network_gateway
{
public:
  explicit sctp_network_gateway_impl(sctp_network_gateway_config            config_,
                                     sctp_network_gateway_control_notifier& ctrl_notfier_,
                                     network_gateway_data_notifier&         data_notifier_);
  virtual ~sctp_network_gateway_impl() { close_socket(); }

  /// \brief Create and connect socket to given address.
  bool create_and_connect() override;

  /// \brief Return socket file descriptor.
  int get_socket_fd() override;

  /// \brief Trigger receive call on socket.
  void receive() override;

  // network_gateway_data_handler interface
  void handle_pdu(const byte_buffer& pdu) override;

  // network_gateway_controller interface
  /// \brief Create and bind socket to given address.
  bool create_and_bind() override;

  /// \brief Start listening on socket.
  bool listen() override;

  /// \brief Return the port on which the socket is listening.
  optional<uint16_t> get_listen_port() override;

private:
  bool set_sockopts();

  /// \brief Recreate and reconnect socket to given address.
  bool recreate_and_reconnect() override;

  // internal helpers
  using socket_buffer_type = uint8_t;
  void handle_data(span<socket_buffer_type> payload);
  void handle_notification(span<socket_buffer_type> payload);

  // socket helpers
  bool set_non_blocking();
  bool set_receive_timeout(unsigned rx_timeout_sec);
  bool set_reuse_addr();
  bool subscripe_to_events();
  bool close_socket();

  sctp_network_gateway_config            config; /// configuration
  sctp_network_gateway_control_notifier& ctrl_notifier;
  network_gateway_data_notifier&         data_notifier;
  srslog::basic_logger&                  logger;

  unique_fd sock_fd;

  sockaddr_storage client_addr        = {}; // the local address
  socklen_t        client_addrlen     = 0;
  int              client_ai_family   = 0;
  int              client_ai_socktype = 0;
  int              client_ai_protocol = 0;

  sockaddr_storage server_addr        = {}; // the server address when operating as client
  socklen_t        server_addrlen     = 0;
  int              server_ai_family   = 0;
  int              server_ai_socktype = 0;
  int              server_ai_protocol = 0;

  sockaddr_storage msg_src_addr    = {}; // message source address
  socklen_t        msg_src_addrlen = 0;

  sockaddr_storage msg_dst_addr    = {}; // message destination address
  socklen_t        msg_dst_addrlen = 0;

  unsigned stream_no = 0; /// the stream number to use for sending
};

} // namespace srsran
