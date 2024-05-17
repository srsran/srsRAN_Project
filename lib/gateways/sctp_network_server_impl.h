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

#include "sctp_network_gateway_common_impl.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

class sctp_network_server_impl : public sctp_network_server, public sctp_network_gateway_common_impl
{
public:
  explicit sctp_network_server_impl(const sctp_network_node_config& sctp_cfg);
  ~sctp_network_server_impl() override;

  /// \brief Create and bind socket to given address.
  bool create_and_bind() override;

  void receive() override;

  int get_socket_fd() override { return this->socket.fd().value(); }

  bool listen() override;

  optional<uint16_t> get_listen_port() override;

  bool subscribe_to(io_broker& broker) override;

  void attach_association_handler(sctp_network_association_factory& factory) override;

private:
};

} // namespace srsran