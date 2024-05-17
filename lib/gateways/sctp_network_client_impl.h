/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sctp_network_gateway_common_impl.h"
#include "srsran/gateways/sctp_network_server.h"

namespace srsran {

class sctp_network_client_impl : public sctp_network_client, public sctp_network_gateway_common_impl
{
  explicit sctp_network_client_impl(const sctp_network_node_config& sctp_cfg, io_broker& broker);

public:
  ~sctp_network_client_impl() override;

  static std::unique_ptr<sctp_network_client> create(const sctp_network_node_config& sctp_cfg, io_broker& broker);

  int get_socket_fd() const override { return socket.fd().value(); }

  std::unique_ptr<sctp_association_pdu_notifier>
  connect_to(const std::string& connection_name, const std::string& dest_addr, int dest_port) override;

private:
  io_broker& broker;
};

} // namespace srsran