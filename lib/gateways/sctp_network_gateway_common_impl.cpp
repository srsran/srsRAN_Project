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
#include <netdb.h>
#include <sys/socket.h>

using namespace srsran;

sockaddr_searcher::sockaddr_searcher(const std::string& address, int port, srslog::basic_logger& logger)
{
  struct addrinfo hints {};
  // support ipv4, ipv6 and hostnames
  hints.ai_family    = AF_UNSPEC;
  hints.ai_socktype  = SOCK_SEQPACKET;
  hints.ai_flags     = 0;
  hints.ai_protocol  = IPPROTO_SCTP;
  hints.ai_canonname = nullptr;
  hints.ai_addr      = nullptr;
  hints.ai_next      = nullptr;

  std::string port_str = std::to_string(port);
  int         ret      = getaddrinfo(address.c_str(), port_str.c_str(), &hints, &results);
  if (ret != 0) {
    logger.error("Error in \"getaddrinfo\" for \"{}\":{}. Cause: {}", address, port, gai_strerror(ret));
    results = nullptr;
    return;
  }
  next_result = results;
}

sockaddr_searcher::~sockaddr_searcher()
{
  freeaddrinfo(results);
}

/// Get next candidate or nullptr of search has ended.
struct addrinfo* sockaddr_searcher::next()
{
  struct addrinfo* ret = next_result;
  if (next_result != nullptr) {
    next_result = next_result->ai_next;
  }
  return ret;
}

// class common_sctp_network_gateway_impl

sctp_network_gateway_common_impl::sctp_network_gateway_common_impl(const sctp_network_node_config& cfg) :
  node_cfg(cfg), logger(srslog::fetch_basic_logger("SCTP-GW"))
{
}

sctp_network_gateway_common_impl::~sctp_network_gateway_common_impl()
{
  close_socket();
}

bool sctp_network_gateway_common_impl::close_socket()
{
  // Stop listening to new IO Rx events.
  io_sub.reset();

  // Close SCTP socket.
  return socket.close();
}

expected<sctp_socket> sctp_network_gateway_common_impl::create_socket(int ai_family, int ai_socktype) const
{
  sctp_socket_params params;
  params.ai_family         = ai_family;
  params.ai_socktype       = ai_socktype;
  params.reuse_addr        = node_cfg.reuse_addr;
  params.non_blocking_mode = node_cfg.non_blocking_mode;
  params.rx_timeout        = std::chrono::seconds(node_cfg.rx_timeout_sec);
  params.rto_initial       = node_cfg.rto_initial;
  params.rto_min           = node_cfg.rto_min;
  params.rto_max           = node_cfg.rto_max;
  params.init_max_attempts = node_cfg.init_max_attempts;
  params.max_init_timeo    = node_cfg.max_init_timeo;
  params.nodelay           = node_cfg.nodelay;
  return sctp_socket::create(params);
}

/// \brief Create and bind socket to given address.
bool sctp_network_gateway_common_impl::common_create_and_bind()
{
  sockaddr_searcher searcher{node_cfg.bind_address, node_cfg.bind_port, logger};
  struct addrinfo*  result = nullptr;
  for (result = searcher.next(); result != nullptr; result = searcher.next()) {
    // create SCTP socket
    auto outcome = this->create_socket(result->ai_family, result->ai_socktype);
    if (outcome.is_error()) {
      if (errno == ESOCKTNOSUPPORT) {
        // There is no support for this type of socket. Stop search.
        break;
      }
      continue;
    }
    sctp_socket& candidate = outcome.value();

    if (not candidate.bind(*result->ai_addr, result->ai_addrlen, node_cfg.bind_interface)) {
      // Bind failed. Try next candidate
      continue;
    }

    // Socket is successfully created and binded. Save it and exit search.
    socket = std::move(candidate);
    break;
  }

  if (not socket.is_open()) {
    fmt::print("Failed to bind SCTP socket to {}:{}. errno=\"{}\"\n",
               node_cfg.bind_address,
               node_cfg.bind_port,
               strerror(errno));
    logger.error("Failed to bind SCTP socket to {}:{}. errno=\"{}\"",
                 node_cfg.bind_address,
                 node_cfg.bind_port,
                 strerror(errno));
    return false;
  }

  logger.debug("Binding to {}:{} was successful", node_cfg.bind_address, node_cfg.bind_port);

  return true;
}
