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

#include "srsran/support/io/sockets.h"
#include "srsran/support/srsran_assert.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace srsran;

bool srsran::getnameinfo(const struct sockaddr&        ai_addr,
                         const socklen_t&              ai_addrlen,
                         std::array<char, NI_MAXHOST>& ip_address,
                         int&                          port)
{
  char port_nr[NI_MAXSERV];
  if (getnameinfo(
          &ai_addr, ai_addrlen, ip_address.data(), NI_MAXHOST, port_nr, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) !=
      0) {
    return false;
  }
  port = std::stoi(port_nr);
  return true;
}

socket_name_info srsran::get_nameinfo(const struct sockaddr& ai_addr, const socklen_t& ai_addrlen)
{
  std::array<char, NI_MAXHOST> ip_addr;
  int                          port;
  if (not getnameinfo(ai_addr, ai_addrlen, ip_addr, port)) {
    return {std::string(""), -1};
  }
  return socket_name_info{std::string(ip_addr.data()), port};
}

bool srsran::set_reuse_addr(const unique_fd& fd, srslog::basic_logger& logger)
{
  srsran_assert(fd.is_open(), "fd is not open");
  int one = 1;
  if (::setsockopt(fd.value(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) != 0) {
    logger.error("fd={}: Couldn't set reuseaddr for socket: {}", fd.value(), strerror(errno));
    return false;
  }
  return true;
}

bool srsran::bind_to_interface(const unique_fd& fd, const std::string& interface, srslog::basic_logger& logger)
{
  if (interface.empty() || interface == "auto") {
    // no need to change anything
    return true;
  }

  ifreq ifr{};
  std::strncpy(ifr.ifr_ifrn.ifrn_name, interface.c_str(), IFNAMSIZ);
  ifr.ifr_ifrn.ifrn_name[IFNAMSIZ - 1] = 0; // ensure null termination in case input exceeds maximum length

  if (setsockopt(fd.value(), SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr)) < 0) {
    logger.error("fd={}: Could not bind socket to interface \"{}\". errno=\"{}\"",
                 fd.value(),
                 ifr.ifr_ifrn.ifrn_name,
                 strerror(errno));
    return false;
  }
  return true;
}

bool srsran::sockaddr_to_ip_str(const sockaddr* addr, std::string& ip_address, srslog::basic_logger& logger)
{
  char addr_str[INET6_ADDRSTRLEN] = {};
  if (addr->sa_family == AF_INET) {
    if (inet_ntop(AF_INET, &((sockaddr_in*)addr)->sin_addr, addr_str, INET6_ADDRSTRLEN) == nullptr) {
      logger.error("Could not convert sockaddr_in to string. errno=\"{}\"", strerror(errno));
      return false;
    }
  } else if (addr->sa_family == AF_INET6) {
    if (inet_ntop(AF_INET6, &((sockaddr_in6*)addr)->sin6_addr, addr_str, INET6_ADDRSTRLEN) == nullptr) {
      logger.error("Could not convert sockaddr_in6 to string. errno=\"{}\"", strerror(errno));
      return false;
    }
  } else {
    logger.error("Unhandled address family.");
    return false;
  }

  ip_address = addr_str;
  logger.debug("Read bind port of UDP network gateway: {}", ip_address);
  return true;
}

bool srsran::set_receive_timeout(const unique_fd& fd, std::chrono::seconds rx_timeout, srslog::basic_logger& logger)
{
  srsran_sanity_check(fd.is_open(), "Invalid FD");
  struct timeval tv;
  tv.tv_sec  = rx_timeout.count();
  tv.tv_usec = 0;

  if (::setsockopt(fd.value(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) != 0) {
    logger.error("SCTP failed to be created. Cause: Couldn't set receive timeout for socket: {}", strerror(errno));
    return false;
  }
  return true;
}

bool srsran::set_non_blocking(const unique_fd& fd, srslog::basic_logger& logger)
{
  if (not fd.is_open()) {
    logger.error("Failed to set socket as non-blocking. Cause: Socket is closed");
    return false;
  }
  int flags = fcntl(fd.value(), F_GETFL, 0);
  if (flags == -1) {
    logger.error("Failed to set socket as non-blocking. Cause: Error getting socket flags: {}", strerror(errno));
    return false;
  }

  int s = fcntl(fd.value(), F_SETFL, flags | O_NONBLOCK);
  if (s == -1) {
    logger.error("Failed to set socket as non-blocking. Cause: Error {}", strerror(errno));
    return false;
  }

  return true;
}

std::string srsran::sock_type_to_str(int type)
{
  switch (type) {
    case SOCK_STREAM:
      return "SOCK_STREAM";
    case SOCK_DGRAM:
      return "SOCK_DGRAM";
    case SOCK_RAW:
      return "SOCK_RAW";
    case SOCK_RDM:
      return "SOCK_RDM";
    case SOCK_SEQPACKET:
      return "SOCK_SEQPACKET";
    case SOCK_DCCP:
      return "SOCK_DCCP";
    case SOCK_PACKET:
      return "SOCK_PACKET";
  }
  return "unknown type";
}
