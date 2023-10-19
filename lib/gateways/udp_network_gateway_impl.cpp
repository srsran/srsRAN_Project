/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "udp_network_gateway_impl.h"
#include "srsran/adt/span.h"
#include "srsran/gateways/addr_info.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/sockets.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <utility>

using namespace srsran;

udp_network_gateway_impl::udp_network_gateway_impl(udp_network_gateway_config                   config_,
                                                   network_gateway_data_notifier_with_src_addr& data_notifier_) :
  config(std::move(config_)), data_notifier(data_notifier_), logger(srslog::fetch_basic_logger("UDP-GW"))
{
  logger.info("UDP GW configured. rx_max_mmsg={}", config.rx_max_mmsg);

  // Allocate RX buffers
  rx_mem.resize(config.rx_max_mmsg);
  for (uint32_t i = 0; i < config.rx_max_mmsg; ++i) {
    rx_mem[i].resize(network_gateway_udp_max_len);
  }

  // Allocate context for recv_mmsg
  rx_srcaddr.resize(config.rx_max_mmsg);
  rx_msghdr.resize(config.rx_max_mmsg);
  rx_iovecs.resize(config.rx_max_mmsg);
}

bool udp_network_gateway_impl::is_initialized()
{
  return sock_fd != -1;
}

void udp_network_gateway_impl::handle_pdu(const byte_buffer& pdu, const sockaddr_storage& dest_addr)
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

  int bytes_sent =
      sendto(sock_fd, pdu_span.data(), pdu_span.size_bytes(), 0, (sockaddr*)&dest_addr, sizeof(sockaddr_storage));
  if (bytes_sent == -1) {
    std::string local_addr_str;
    std::string dest_addr_str;
    sockaddr_to_ip_str((sockaddr*)&dest_addr, dest_addr_str, logger);
    sockaddr_to_ip_str((sockaddr*)&local_addr, local_addr_str, logger);
    logger.error("Couldn't send {} B of data on UDP socket: local_ip={} dest_ip={} error=\"{}\"",
                 pdu_span.size_bytes(),
                 local_addr_str,
                 dest_addr_str,
                 strerror(errno));
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

    char ip_addr[NI_MAXHOST];
    char port_nr[NI_MAXSERV];
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
    fmt::print("Failed to bind {} socket to {}:{}. {}\n",
               ipproto_to_string(hints.ai_protocol),
               config.bind_address,
               config.bind_port,
               strerror(ret));
    logger.error("Failed to bind {} socket to {}:{}. {}",
                 ipproto_to_string(hints.ai_protocol),
                 config.bind_address,
                 config.bind_port,
                 strerror(ret));
    return false;
  }

  return true;
}

void udp_network_gateway_impl::receive()
{
  if (!is_initialized()) {
    logger.error("Cannot receive on UDP gateway: Socket is not initialized.");
  }

  socklen_t src_addr_len = sizeof(struct sockaddr_storage);

  for (unsigned i = 0; i < config.rx_max_mmsg; ++i) {
    rx_msghdr[i].msg_hdr             = {};
    rx_msghdr[i].msg_hdr.msg_name    = &rx_srcaddr[i];
    rx_msghdr[i].msg_hdr.msg_namelen = src_addr_len;

    rx_iovecs[i].iov_base           = rx_mem[i].data();
    rx_iovecs[i].iov_len            = network_gateway_udp_max_len;
    rx_msghdr[i].msg_hdr.msg_iov    = &rx_iovecs[i];
    rx_msghdr[i].msg_hdr.msg_iovlen = 1;
  }

  int rx_msgs = recvmmsg(sock_fd, rx_msghdr.data(), config.rx_max_mmsg, MSG_WAITFORONE, nullptr);
  if (rx_msgs == -1 && errno != EAGAIN) {
    logger.error("Error reading from UDP socket: {}", strerror(errno));
    return;
  }
  if (rx_msgs == -1 && errno == EAGAIN) {
    if (!config.non_blocking_mode) {
      logger.debug("Socket timeout reached");
    }
    return;
  }

  for (int i = 0; i < rx_msgs; ++i) {
    span<uint8_t> payload(rx_mem[i].data(), rx_msghdr[i].msg_len);
    byte_buffer   pdu = {};
    if (pdu.append(payload)) {
      logger.debug("Received {} bytes on UDP socket", rx_msghdr[i].msg_len);
      data_notifier.on_new_pdu(std::move(pdu), *(sockaddr_storage*)rx_msghdr[i].msg_hdr.msg_name);
    } else {
      logger.error("Could not allocate byte buffer. Received {} bytes on UDP socket", rx_msghdr[i].msg_len);
    }
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
