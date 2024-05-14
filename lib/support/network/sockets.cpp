/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/io/sockets.h"
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace srsran;

std::pair<std::string, int> srsran::get_nameinfo(struct sockaddr& ai_addr, const socklen_t& ai_addrlen)
{
  char ip_addr[NI_MAXHOST], port_nr[NI_MAXSERV];
  if (getnameinfo(&ai_addr, ai_addrlen, ip_addr, NI_MAXHOST, port_nr, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) !=
      0) {
    return {std::string(""), -1};
  }
  return std::make_pair(std::string(ip_addr), std::stoi(port_nr));
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

  ifreq ifr;
  std::strncpy(ifr.ifr_ifrn.ifrn_name, interface.c_str(), IFNAMSIZ);
  ifr.ifr_ifrn.ifrn_name[IFNAMSIZ - 1] = 0; // ensure null termination in case input exceeds maximum length

  if (setsockopt(fd.value(), SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr)) < 0) {
    logger.error("Could not bind socket to interface. interface={} error={}", ifr.ifr_ifrn.ifrn_name, strerror(errno));
    return false;
  }
  return true;
}

bool srsran::sockaddr_to_ip_str(const sockaddr* addr, std::string& ip_address, srslog::basic_logger& logger)
{
  char addr_str[INET6_ADDRSTRLEN] = {};
  if (addr->sa_family == AF_INET) {
    if (inet_ntop(AF_INET, &((sockaddr_in*)addr)->sin_addr, addr_str, INET6_ADDRSTRLEN) == nullptr) {
      logger.error("Could not convert sockaddr_in to string. errno={}", strerror(errno));
      return false;
    }
  } else if (addr->sa_family == AF_INET6) {
    if (inet_ntop(AF_INET6, &((sockaddr_in6*)addr)->sin6_addr, addr_str, INET6_ADDRSTRLEN) == nullptr) {
      logger.error("Could not convert sockaddr_in6 to string. errno={}", strerror(errno));
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
