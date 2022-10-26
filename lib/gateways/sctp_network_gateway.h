/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/gateways/network_gateway.h"
#include "srsgnb/srslog/srslog.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace srsgnb {

class sctp_network_gateway : public network_gateway
{
public:
  explicit sctp_network_gateway(network_gateway_config            config_,
                                network_gateway_control_notifier& ctrl_notfier_,
                                network_gateway_data_notifier&    data_notifier_);

private:
  bool is_initialized();

  // network_gateway_data_handler interface
  void handle_pdu(const byte_buffer& pdu) override;

  bool set_sockopts();

  // network_gateway_controller interface
  /// \brief Create and bind socket to given address.
  bool bind() override;

  /// \brief Start listening on socket.
  bool listen() override;

  /// \brief Create and connect socket to given address.
  bool connect() override;

  /// \brief Trigger receive call on socket.
  void receive() override;

  /// \brief Return socket file descriptor.
  int get_socket_fd() override;

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

  network_gateway_config            config; /// configuration
  network_gateway_control_notifier& ctrl_notifier;
  network_gateway_data_notifier&    data_notifier;
  srslog::basic_logger&             logger;

  int sock_fd = -1;

  struct sockaddr_storage client_addr    = {}; // the local address
  socklen_t               client_addrlen = 0;

  struct sockaddr_storage server_addr    = {}; // the server address when operating as client
  socklen_t               server_addrlen = 0;

  unsigned ppi       = 0; /// the Payload Protocol Identifier (e.g. 18 for S1AP)
  unsigned stream_no = 0; /// the stream number to use for sending

  std::array<uint8_t, 1024> rx_buffer = {}; /// temporary Rx buffer for sctp_recv() operation
};

} // namespace srsgnb
