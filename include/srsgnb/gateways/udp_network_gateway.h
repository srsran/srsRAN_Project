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

constexpr uint32_t network_gateway_udp_max_len = 9100;

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
  virtual int get_bind_port() = 0;
};

class udp_network_gateway : public udp_network_gateway_data_handler, public udp_network_gateway_controller
{
public:
  explicit udp_network_gateway(udp_network_gateway_config config_, network_gateway_data_notifier& data_notifier_);
  ~udp_network_gateway() override { close_socket(); }

  // network_gateway_data_handler interface
  void handle_pdu(const byte_buffer& pdu, const ::sockaddr* dest_addr, ::socklen_t dest_len) override;

  // network_gateway_controller interface
  bool create_and_bind() final;
  void receive() final;
  int  get_socket_fd() final;
  int  get_bind_port() final;

private:
  bool is_initialized();
  bool set_sockopts();

  // socket helpers
  bool set_non_blocking();
  bool set_receive_timeout(unsigned rx_timeout_sec);
  bool set_reuse_addr();
  bool close_socket();

  udp_network_gateway_config     config; // configuration
  network_gateway_data_notifier& data_notifier;
  srslog::basic_logger&          logger;

  int sock_fd = -1;

  sockaddr_storage local_addr        = {}; // the local address
  socklen_t        local_addrlen     = 0;
  int              local_ai_family   = 0;
  int              local_ai_socktype = 0;
  int              local_ai_protocol = 0;
};

} // namespace srsgnb
