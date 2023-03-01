/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "udp_network_gateway_impl.h"
#include "srsran/adt/span.h"
#include "srsran/srslog/srslog.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <utility>

using namespace srsran;

udp_network_gateway_impl::udp_network_gateway_impl(udp_network_gateway_config     config_,
                                                   network_gateway_data_notifier& data_notifier_) :
  config(std::move(config_)), data_notifier(data_notifier_), logger(srslog::fetch_basic_logger("UDP-GW"))
{
}

bool udp_network_gateway_impl::is_initialized()
{
  return sock_fd != -1;
}

void udp_network_gateway_impl::handle_pdu(const byte_buffer& pdu, const ::sockaddr* dest_addr, ::socklen_t dest_len)
{
  logger.debug("Sending PDU of {} bytes", pdu.length());

  if (not is_initialized()) {
    logger.error("Socket not initialized");
    return;
  }

  if (pdu.length() > network_gateway_udp_max_len) {
    logger.error("PDU of {} bytes exceeds maximum length of {} bytes", pdu.length(), network_gateway_udp_max_len);
    return;
  }

  // Fixme: consider class member on heap when sequential access is guaranteed
  std::array<uint8_t, network_gateway_udp_max_len> tmp_mem; // no init

  span<const uint8_t> pdu_span = to_span(pdu, tmp_mem);

  int bytes_sent = sendto(sock_fd, pdu_span.data(), pdu_span.size_bytes(), 0, dest_addr, dest_len);
  if (bytes_sent == -1) {
    logger.error("Couldn't send {} B of data on UDP socket: {}", pdu_span.size_bytes(), strerror(errno));
  }

  logger.debug("PDU was sent successfully");
}

bool udp_network_gateway_impl::create_and_bind()
{
  struct addrinfo hints;
  // support ipv4, ipv6 and hostnames
  hints.ai_family    = AF_UNSPEC;
  hints.ai_socktype  = SOCK_DGRAM;
  hints.ai_flags     = 0;
  hints.ai_protocol  = IPPROTO_UDP;
  hints.ai_canonname = nullptr;
  hints.ai_addr      = nullptr;
  hints.ai_next      = nullptr;

  std::string      bind_port = std::to_string(config.bind_port);
  struct addrinfo* results;

  int ret = getaddrinfo(config.bind_address.c_str(), bind_port.c_str(), &hints, &results);
  if (ret != 0) {
    logger.error("Getaddrinfo error: {} - {}", config.bind_address, gai_strerror(ret));
    return false;
  }

  struct addrinfo* result;
  for (result = results; result != nullptr; result = result->ai_next) {
    // create UDP socket
    sock_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock_fd == -1) {
      ret = errno;
      continue;
    }

    if (not set_sockopts()) {
      close_socket();
      continue;
    }

    char ip_addr[NI_MAXHOST], port_nr[NI_MAXSERV];
    getnameinfo(
        result->ai_addr, result->ai_addrlen, ip_addr, NI_MAXHOST, port_nr, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    logger.debug("Binding to {} port {}", ip_addr, port_nr);

    if (bind(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
      // binding failed, try next address
      ret = errno;
      logger.debug("Failed to bind to {}:{} - {}", ip_addr, port_nr, strerror(ret));
      close_socket();
      continue;
    }

    // store client address
    memcpy(&local_addr, result->ai_addr, result->ai_addrlen);
    local_addrlen     = result->ai_addrlen;
    local_ai_family   = result->ai_family;
    local_ai_socktype = result->ai_socktype;
    local_ai_protocol = result->ai_protocol;

    // set socket to non-blocking after bind is successful
    if (config.non_blocking_mode) {
      if (not set_non_blocking()) {
        // failed, try next address
        logger.error("Socket not non-blocking");
        close_socket();
        continue;
      }
    }

    logger.debug("Binding successful");
    break;
  }

  freeaddrinfo(results);

  if (sock_fd == -1) {
    logger.error("Error binding to {}:{} - {}", config.bind_address, config.bind_port, strerror(ret));
    return false;
  }

  return true;
}

void udp_network_gateway_impl::receive()
{
  if (!is_initialized()) {
    logger.error("Cannot receive on UDP gateway: Socket is not initialized.");
  }

  // Fixme: consider class member on heap when sequential access is guaranteed
  std::array<uint8_t, network_gateway_udp_max_len> tmp_mem; // no init

  int rx_bytes = recv(sock_fd, tmp_mem.data(), network_gateway_udp_max_len, 0);

  if (rx_bytes == -1 && errno != EAGAIN) {
    logger.error("Error reading from UDP socket: {}", strerror(errno));
  } else if (rx_bytes == -1 && errno == EAGAIN) {
    logger.debug("Socket timeout reached");
  } else {
    logger.debug("Received {} bytes on UDP socket", rx_bytes);
    span<uint8_t> payload(tmp_mem.data(), rx_bytes);
    byte_buffer   pdu = {payload};
    data_notifier.on_new_pdu(std::move(pdu));
  }
}

int udp_network_gateway_impl::get_socket_fd()
{
  return sock_fd;
}

bool udp_network_gateway_impl::get_bind_port(uint16_t& gw_bind_port)
{
  if (not is_initialized()) {
    logger.error("Socket of UDP network gateway not initialized.");
    return false;
  }

  sockaddr_storage gw_addr_storage;
  sockaddr*        gw_addr     = (sockaddr*)&gw_addr_storage;
  socklen_t        gw_addr_len = sizeof(gw_addr_storage);

  int ret = getsockname(sock_fd, gw_addr, &gw_addr_len);
  if (ret != 0) {
    logger.error("Failed `getsockname` in UDP network gateway with sock_fd={}: {}", sock_fd, strerror(errno));
    return false;
  }

  if (gw_addr->sa_family == AF_INET) {
    gw_bind_port = ntohs(((sockaddr_in*)gw_addr)->sin_port);
  } else if (gw_addr->sa_family == AF_INET6) {
    gw_bind_port = ntohs(((sockaddr_in6*)gw_addr)->sin6_port);
  } else {
    logger.error(
        "Unhandled address family in UDP network gateway with sock_fd={}, family={}", sock_fd, gw_addr->sa_family);
    return false;
  }

  logger.debug("Read bind port of UDP network gateway: {}", gw_bind_port);
  return true;
}

bool udp_network_gateway_impl::get_bind_address(std::string& ip_address)
{
  ip_address = "no address";

  if (not is_initialized()) {
    logger.error("Socket of UDP network gateway not initialized.");
    return false;
  }

  sockaddr_storage gw_addr_storage = {};
  sockaddr*        gw_addr         = (sockaddr*)&gw_addr_storage;
  socklen_t        gw_addr_len     = sizeof(gw_addr_storage);

  int ret = getsockname(sock_fd, gw_addr, &gw_addr_len);
  if (ret != 0) {
    logger.error("Failed `getsockname` in UDP network gateway with sock_fd={}: {}", sock_fd, strerror(errno));
    return false;
  }

  char addr_str[INET6_ADDRSTRLEN] = {};
  if (gw_addr->sa_family == AF_INET) {
    if (inet_ntop(AF_INET, &((sockaddr_in*)gw_addr)->sin_addr, addr_str, INET6_ADDRSTRLEN) == nullptr) {
      logger.error("Could not convert sockaddr_in to string. sock_fd={}, errno={}", sock_fd, strerror(errno));
      return false;
    }
  } else if (gw_addr->sa_family == AF_INET6) {
    if (inet_ntop(AF_INET6, &((sockaddr_in6*)gw_addr)->sin6_addr, addr_str, INET6_ADDRSTRLEN) == nullptr) {
      logger.error("Could not convert sockaddr_in6 to string. sock_fd={}, errno={}", sock_fd, strerror(errno));
      return false;
    }
  } else {
    logger.error("Unhandled address family in UDP network gateway with sock_fd={}", sock_fd);
    return false;
  }

  ip_address = addr_str;
  logger.debug("Read bind port of UDP network gateway: {}", ip_address);
  return true;
}

bool udp_network_gateway_impl::set_sockopts()
{
  if (config.rx_timeout_sec > 0) {
    if (not set_receive_timeout(config.rx_timeout_sec)) {
      logger.error("Couldn't set receive timeout for socket");

      return false;
    }
  }

  if (config.reuse_addr) {
    if (not set_reuse_addr()) {
      logger.error("Couldn't set reuseaddr for socket");
      return false;
    }
  }

  return true;
}

bool udp_network_gateway_impl::set_non_blocking()
{
  int flags = fcntl(sock_fd, F_GETFL, 0);
  if (flags == -1) {
    logger.error("Error getting socket flags: {}", strerror(errno));
    return false;
  }

  int s = fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);
  if (s == -1) {
    logger.error("Error setting socket to non-blocking mode: {}", strerror(errno));
    return false;
  }

  return true;
}

bool udp_network_gateway_impl::set_receive_timeout(unsigned rx_timeout_sec)
{
  struct timeval tv;
  tv.tv_sec  = rx_timeout_sec;
  tv.tv_usec = 0;

  if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv)) {
    logger.error("Couldn't set receive timeout for socket: {}", strerror(errno));
    return false;
  }

  return true;
}

bool udp_network_gateway_impl::set_reuse_addr()
{
  int one = 1;
  if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
    logger.error("Couldn't set reuseaddr for socket: {}", strerror(errno));
    return false;
  }
  return true;
}

bool udp_network_gateway_impl::close_socket()
{
  if (not is_initialized()) {
    return true;
  }

  if (close(sock_fd) == -1) {
    logger.error("Error closing socket: {}", strerror(errno));
    return false;
  }

  sock_fd = -1;

  return true;
}
