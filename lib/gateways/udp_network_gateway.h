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
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace srsgnb {

class udp_network_gateway : public network_gateway
{
public:
  explicit udp_network_gateway(network_gateway_config config_, network_gateway_data_notifier& data_notifier_);

  // network_gateway_data_handler interface
  void handle_pdu(const byte_buffer& pdu) final;

  // network_gateway_controller interface
  bool create_and_bind() final;
  bool listen() final;
  bool create_and_connect() final;
  bool recreate_and_reconnect() final;
  void receive() final;
  int  get_socket_fd() final;

private:
  bool is_initialized();
  bool set_sockopts();

  // socket helpers
  bool set_non_blocking();
  bool set_receive_timeout(unsigned rx_timeout_sec);
  bool set_reuse_addr();
  bool close_socket();

  network_gateway_config         config; // configuration
  network_gateway_data_notifier& data_notifier;
  srslog::basic_logger&          logger;

  int sock_fd = -1;

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
};

} // namespace srsgnb
