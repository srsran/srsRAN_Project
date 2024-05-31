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

#pragma once

#include "sctp_network_gateway_common_impl.h"
#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/support/io/io_broker.h"
#include "srsran/support/io/sctp_socket.h"

namespace srsran {

class sctp_network_gateway_impl final : public sctp_network_gateway_common_impl, public sctp_network_gateway
{
public:
  explicit sctp_network_gateway_impl(const sctp_network_connector_config&   config_,
                                     sctp_network_gateway_control_notifier& ctrl_notifier_,
                                     network_gateway_data_notifier&         data_notifier_);

  /// \brief Create and connect socket to given address.
  bool create_and_connect() override;

  /// \brief Return socket file descriptor.
  int get_socket_fd() override { return socket.fd().value(); }

  /// \brief Trigger receive call on socket.
  void receive() override;

  // network_gateway_data_handler interface
  void handle_pdu(const byte_buffer& pdu) override;

  // network_gateway_controller interface
  bool create_and_bind() override;

  bool listen() override;

  /// \brief Return the port on which the socket is listening.
  std::optional<uint16_t> get_listen_port() override;

  /// \brief Subscribe to IO broker for automatic IO Rx notifications.
  bool subscribe_to(io_broker& broker) override;

private:
  // internal helpers
  using socket_buffer_type = uint8_t;
  void handle_data(span<socket_buffer_type> payload);
  void handle_notification(span<socket_buffer_type> payload);
  void handle_connection_loss();

  sctp_network_connector_config          config;
  sctp_network_gateway_control_notifier& ctrl_notifier;
  network_gateway_data_notifier&         data_notifier;

  bool client_mode = false;

  sockaddr_storage msg_src_addr    = {}; // message source address
  socklen_t        msg_src_addrlen = 0;

  sockaddr_storage msg_dst_addr    = {}; // message destination address
  socklen_t        msg_dst_addrlen = 0;

  unsigned stream_no = 0; /// the stream number to use for sending
};

} // namespace srsran
