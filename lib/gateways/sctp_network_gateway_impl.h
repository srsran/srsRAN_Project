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
#include "srsran/support/io/io_broker.h"
#include "srsran/support/io/unique_fd.h"
#include <sys/socket.h>

namespace srsran {

constexpr uint32_t network_gateway_sctp_max_len = 9100;

struct sctp_socket_params {
  int                  ai_family;
  int                  ai_socktype;
  int                  ai_protocol;
  bool                 reuse_addr = false;
  std::chrono::seconds rx_timeout{0};
  optional<int32_t>    rto_initial;
  optional<int32_t>    rto_min;
  optional<int32_t>    rto_max;
  optional<int32_t>    init_max_attempts;
  optional<int32_t>    max_init_timeo;
  optional<bool>       nodelay;
};

class sctp_socket
{
public:
  ~sctp_socket();
  sctp_socket();
  sctp_socket(sctp_socket&& other) noexcept = default;

  sctp_socket& operator=(sctp_socket&& other) noexcept;

  static expected<sctp_socket, int> create(const sctp_socket_params& params);

  bool close();

  bool             is_open() const { return sock_fd.is_open(); }
  const unique_fd& fd() const { return sock_fd; }

  bool set_non_blocking();

private:
  bool set_sockopts(const sctp_socket_params& params);

  srslog::basic_logger& logger;
  unique_fd             sock_fd;
};

class sctp_network_gateway_impl final : public sctp_network_gateway
{
public:
  explicit sctp_network_gateway_impl(sctp_network_gateway_config            config_,
                                     sctp_network_gateway_control_notifier& ctrl_notfier_,
                                     network_gateway_data_notifier&         data_notifier_);
  ~sctp_network_gateway_impl() override;

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

  /// \brief Subscribe to IO broker for automatic IO Rx notifications.
  bool subscribe_to(io_broker& broker) override;

private:
  expected<sctp_socket, int> create_socket(int ai_family, int ai_socktype, int ai_protocol) const;

  /// \brief Recreate and reconnect socket to given address.
  bool recreate_and_reconnect() override;

  // internal helpers
  using socket_buffer_type = uint8_t;
  void handle_data(span<socket_buffer_type> payload);
  void handle_notification(span<socket_buffer_type> payload);
  void handle_io_error(io_broker::error_code code);

  // socket helpers
  bool close_socket();

  sctp_network_gateway_config            config; /// configuration
  sctp_network_gateway_control_notifier& ctrl_notifier;
  network_gateway_data_notifier&         data_notifier;
  srslog::basic_logger&                  logger;

  sctp_socket           socket;
  io_broker::subscriber io_sub;

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
