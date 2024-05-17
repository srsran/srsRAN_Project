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
#include "srsran/srslog/logger.h"
#include "srsran/support/io/io_broker.h"
#include "srsran/support/io/sctp_socket.h"

struct addrinfo;

namespace srsran {

/// Helper generator class that traverses a list of SCTP sockaddr param candidates (ipv4, ipv6, hostnames).
class sockaddr_searcher
{
public:
  sockaddr_searcher(const std::string& address, int port, srslog::basic_logger& logger);
  sockaddr_searcher(const sockaddr_searcher&) = delete;
  sockaddr_searcher(sockaddr_searcher&&)      = delete;
  ~sockaddr_searcher();

  /// Get next candidate or nullptr of search has ended.
  struct addrinfo* next();

private:
  struct addrinfo* results     = nullptr;
  struct addrinfo* next_result = nullptr;
};

/// This class holds common functionality to the SCTP network server and client implementations.
class sctp_network_gateway_common_impl
{
public:
  sctp_network_gateway_common_impl(const sctp_network_node_config& cfg);
  ~sctp_network_gateway_common_impl();

protected:
  constexpr static uint32_t network_gateway_sctp_max_len = 9100;

  // Close socket and unsubscribe it from the io_broker.
  bool close_socket();

  // Creates an SCTP socket with the provided protocol.
  SRSRAN_NODISCARD expected<sctp_socket> create_socket(int ai_family, int ai_socktype) const;

  bool common_create_and_bind();

  const sctp_network_node_config node_cfg;
  srslog::basic_logger&          logger;

  sctp_socket socket;

  io_broker::subscriber io_sub;
};

} // namespace srsran