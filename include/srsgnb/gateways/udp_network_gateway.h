/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

namespace srsran {

struct udp_network_gateway_config : common_network_gateway_config {};

/// Interface to inject PDUs into gateway entity.
class udp_network_gateway_data_handler
{
public:
  virtual ~udp_network_gateway_data_handler() = default;

  /// \brief Handle the incoming PDU.
  /// \param[in]  put Byte-buffer with new PDU.
  virtual void handle_pdu(const byte_buffer& pdu, const ::sockaddr* dest_addr, ::socklen_t dest_len) = 0;
};

/// Interface to trigger bind/listen/connect operations on gateway socket.
class udp_network_gateway_controller
{
public:
  virtual ~udp_network_gateway_controller() = default;

  /// \brief Create and bind the configured address and port.
  virtual bool create_and_bind() = 0;

  /// \brief Trigger receive call on socket.
  virtual void receive() = 0;

  /// \brief Return socket file descriptor.
  virtual int get_socket_fd() = 0;

  /// \brief Return the port to which the socket is bound.
  ///
  /// In case the gateway was configured to bind to port 0, i.e. the operating system shall pick a random free port,
  /// this function can be used to get the actual port number.
  virtual bool get_bind_port(uint16_t& port) = 0;

  /// \brief Return the address to which the socket is bound.
  ///
  /// In case the gateway was configured to use a hostname,
  /// this function can be used to get the actual IP address in string form.
  virtual bool get_bind_address(std::string& ip_address) = 0;
};

class udp_network_gateway : public udp_network_gateway_data_handler, public udp_network_gateway_controller
{};

} // namespace srsran
