/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/support/io/sockets.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

using namespace srsran;

udp_network_gateway_impl::udp_network_gateway_impl(udp_network_gateway_config                   config_,
                                                   network_gateway_data_notifier_with_src_addr& data_notifier_,
                                                   task_executor&                               io_tx_executor_,
                                                   task_executor&                               io_rx_executor_) :
  config(std::move(config_)),
  data_notifier(data_notifier_),
  logger(srslog::fetch_basic_logger("UDP-GW")),
  io_tx_executor(io_tx_executor_),
  io_rx_executor(io_rx_executor_)
{
  logger.info("UDP GW configured. rx_max_mmsg={} pool_thres={} ext_bind_addr={}",
              config.rx_max_mmsg,
              config.pool_occupancy_threshold,
              config.ext_bind_addr);
}

bool udp_network_gateway_impl::subscribe_to(io_broker& broker)
{
  io_subcriber = broker.register_fd(
      unique_fd(get_socket_fd()),
      io_rx_executor,
      [this]() { receive(); },
      [this](io_broker::error_code code) { handle_io_error(code); });
  if (not io_subcriber.registered()) {
    logger.error("Failed to register UDP network gateway at IO broker. socket_fd={}", get_socket_fd());
    return false;
  }
  logger.debug("Registered UDP network gateway at IO broker. socket_fd={}", get_socket_fd());
  return true;
}

void udp_network_gateway_impl::handle_pdu(byte_buffer pdu, const sockaddr_storage& dest_addr)
{
  auto fn = [this, p = std::move(pdu), dest_addr]() mutable { handle_pdu_impl(std::move(p), dest_addr); };
  if (not io_tx_executor.execute(std::move(fn))) {
    logger.info("Dropped PDU, queue is full.");
  }
}

void udp_network_gateway_impl::handle_pdu_impl(const byte_buffer& pdu, const sockaddr_storage& dest_addr)
{
  if (not sock_fd.is_open()) {
    logger.error("Socket not initialized");
    return;
  }

  if (pdu.length() > network_gateway_udp_max_len) {
    logger.error("PDU of {} bytes exceeds maximum length of {} bytes", pdu.length(), network_gateway_udp_max_len);
    return;
  }

  span<const uint8_t> pdu_span = to_span(pdu, tx_mem);

  int bytes_sent = sendto(
      sock_fd.value(), pdu_span.data(), pdu_span.size_bytes(), 0, (sockaddr*)&dest_addr, sizeof(sockaddr_storage));
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

  std::string local_addr_str;
  std::string dest_addr_str;
  uint16_t    dest_port = sockaddr_to_port((sockaddr*)&dest_addr, logger);
  sockaddr_to_ip_str((sockaddr*)&dest_addr, dest_addr_str, logger);
  sockaddr_to_ip_str((sockaddr*)&local_addr, local_addr_str, logger);
  logger.debug("Sent PDU of {} bytes. local_ip={} dest={}:{}", pdu.length(), local_addr_str, dest_addr_str, dest_port);
}

void udp_network_gateway_impl::handle_io_error(io_broker::error_code code)
{
  logger.error("Error reading from UDP socket: {}", sock_fd.value());
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
    sock_fd = unique_fd{::socket(result->ai_family, result->ai_socktype, result->ai_protocol), false};
    if (not sock_fd.is_open()) {
      ret = errno;
      continue;
    }

    char ip_addr[NI_MAXHOST];
    char port_nr[NI_MAXSERV];
    getnameinfo(
        result->ai_addr, result->ai_addrlen, ip_addr, NI_MAXHOST, port_nr, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    logger.debug("Binding to {} port {}", ip_addr, port_nr);

    if (::bind(sock_fd.value(), result->ai_addr, result->ai_addrlen) == -1) {
      // binding failed, try next address
      ret = errno;
      logger.debug("Failed to bind to {}:{} - {}", ip_addr, port_nr, strerror(ret));
      close_socket();
      continue;
    }

    // Bind socket to interface (if requested)
    if (not bind_to_interface(sock_fd, config.bind_interface, logger)) {
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

  if (not sock_fd.is_open()) {
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

  // Set socket options. This is done after binding,
  // so that we can set IPv4/IPv6 options accordingly.
  if (not set_sockopts()) {
    close_socket();
    return false;
  }

  return true;
}

namespace {

/// Receive context used by the recvmmsg syscall.
struct receive_context {
  explicit receive_context(unsigned rx_max_mmsg);

  std::vector<std::vector<uint8_t>> rx_mem;
  std::vector<::sockaddr_storage>   rx_srcaddr;
  std::vector<::mmsghdr>            rx_msghdr;
  std::vector<::iovec>              rx_iovecs;
};

} // namespace

receive_context::receive_context(unsigned rx_max_mmsg)
{
  // Allocate RX buffers.
  rx_mem.resize(rx_max_mmsg);
  for (unsigned i = 0; i != rx_max_mmsg; ++i) {
    rx_mem[i].resize(network_gateway_udp_max_len);
  }

  // Allocate context for recv_mmsg.
  rx_srcaddr.resize(rx_max_mmsg);
  rx_msghdr.resize(rx_max_mmsg);
  rx_iovecs.resize(rx_max_mmsg);

  for (unsigned i = 0; i != rx_max_mmsg; ++i) {
    rx_msghdr[i].msg_hdr             = {};
    rx_msghdr[i].msg_hdr.msg_name    = &rx_srcaddr[i];
    rx_msghdr[i].msg_hdr.msg_namelen = sizeof(::sockaddr_storage);

    rx_iovecs[i].iov_base           = rx_mem[i].data();
    rx_iovecs[i].iov_len            = network_gateway_udp_max_len;
    rx_msghdr[i].msg_hdr.msg_iov    = &rx_iovecs[i];
    rx_msghdr[i].msg_hdr.msg_iovlen = 1;
  }
}

void udp_network_gateway_impl::receive()
{
  if (!sock_fd.is_open()) {
    logger.error("Cannot receive on UDP gateway: Socket is not initialized.");
  }

  thread_local receive_context rx_context(config.rx_max_mmsg);

  int rx_msgs = recvmmsg(sock_fd.value(), rx_context.rx_msghdr.data(), config.rx_max_mmsg, MSG_WAITFORONE, nullptr);
  srslog::fetch_basic_logger("IO-EPOLL").info("UDP rx {} packets, max is {}", rx_msgs, config.rx_max_mmsg);
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
    float pool_occupancy =
        (1 - (float)get_byte_buffer_segment_pool_current_size_approx() / get_byte_buffer_segment_pool_capacity());
    if (pool_occupancy >= config.pool_occupancy_threshold) {
      if (warn_low_buffer_pool) {
        logger.warning("Buffer pool at {:.1f}% occupancy. Dropping {} packets", pool_occupancy * 100, rx_msgs - i);
        warn_low_buffer_pool = false;
        return;
      }
      logger.info("Buffer pool at {:.1f}% occupancy. Dropping {} packets", pool_occupancy * 100, rx_msgs - i);
      return;
    }
    span<uint8_t> payload(rx_context.rx_mem[i].data(), rx_context.rx_msghdr[i].msg_len);

    byte_buffer pdu = {};
    if (pdu.append(payload)) {
      logger.debug("Received {} bytes on UDP socket. Pool occupancy {:.2f}%",
                   rx_context.rx_msghdr[i].msg_len,
                   pool_occupancy * 100);
      data_notifier.on_new_pdu(std::move(pdu), *(sockaddr_storage*)rx_context.rx_msghdr[i].msg_hdr.msg_name);
    } else {
      logger.error("Could not allocate byte buffer. Received {} bytes on UDP socket", rx_context.rx_msghdr[i].msg_len);
    }
  }
}

int udp_network_gateway_impl::get_socket_fd()
{
  return sock_fd.value();
}

std::optional<uint16_t> udp_network_gateway_impl::get_bind_port() const
{
  if (not sock_fd.is_open()) {
    logger.error("Socket of UDP network gateway not initialized.");
    return {};
  }

  sockaddr_storage gw_addr_storage;
  sockaddr*        gw_addr     = (sockaddr*)&gw_addr_storage;
  socklen_t        gw_addr_len = sizeof(gw_addr_storage);

  int ret = getsockname(sock_fd.value(), gw_addr, &gw_addr_len);
  if (ret != 0) {
    logger.error("Failed `getsockname` in UDP network gateway with sock_fd={}: {}", sock_fd.value(), strerror(errno));
    return {};
  }

  uint16_t gw_bind_port;
  if (gw_addr->sa_family == AF_INET) {
    gw_bind_port = ntohs(((sockaddr_in*)gw_addr)->sin_port);
  } else if (gw_addr->sa_family == AF_INET6) {
    gw_bind_port = ntohs(((sockaddr_in6*)gw_addr)->sin6_port);
  } else {
    logger.error("Unhandled address family in UDP network gateway with sock_fd={}, family={}",
                 sock_fd.value(),
                 gw_addr->sa_family);
    return {};
  }

  logger.debug("Read bind port of UDP network gateway: {}", gw_bind_port);
  return gw_bind_port;
}

bool udp_network_gateway_impl::get_bind_address(std::string& ip_address) const
{
  ip_address = "no address";

  if (not sock_fd.is_open()) {
    logger.error("Socket of UDP network gateway not initialized.");
    return false;
  }

  if (config.ext_bind_addr != "" and config.ext_bind_addr != "auto") {
    ip_address = config.ext_bind_addr;
    return true;
  }

  sockaddr_storage gw_addr_storage = {};
  sockaddr*        gw_addr         = (sockaddr*)&gw_addr_storage;
  socklen_t        gw_addr_len     = sizeof(gw_addr_storage);

  int ret = getsockname(sock_fd.value(), gw_addr, &gw_addr_len);
  if (ret != 0) {
    logger.error("Failed `getsockname` in UDP network gateway with sock_fd={}: {}", sock_fd.value(), strerror(errno));
    return false;
  }

  char addr_str[INET6_ADDRSTRLEN] = {};
  if (gw_addr->sa_family == AF_INET) {
    if (inet_ntop(AF_INET, &((sockaddr_in*)gw_addr)->sin_addr, addr_str, INET6_ADDRSTRLEN) == nullptr) {
      logger.error("Could not convert sockaddr_in to string. sock_fd={}, errno={}", sock_fd.value(), strerror(errno));
      return false;
    }
  } else if (gw_addr->sa_family == AF_INET6) {
    if (inet_ntop(AF_INET6, &((sockaddr_in6*)gw_addr)->sin6_addr, addr_str, INET6_ADDRSTRLEN) == nullptr) {
      logger.error("Could not convert sockaddr_in6 to string. sock_fd={}, errno={}", sock_fd.value(), strerror(errno));
      return false;
    }
  } else {
    logger.error("Unhandled address family in UDP network gateway with sock_fd={}", sock_fd.value());
    return false;
  }
  ip_address = addr_str;

  logger.debug("Read bind address of UDP network gateway: {}", ip_address);
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

  if (config.dscp.has_value()) {
    if (not set_dscp()) {
      logger.error("Couldn't set DSCP for socket");
      return false;
    }
  }
  logger.debug("Successfully set socket options");
  return true;
}

bool udp_network_gateway_impl::set_non_blocking()
{
  int flags = fcntl(sock_fd.value(), F_GETFL, 0);
  if (flags == -1) {
    logger.error("Error getting socket flags: {}", strerror(errno));
    return false;
  }

  int s = fcntl(sock_fd.value(), F_SETFL, flags | O_NONBLOCK);
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

  if (setsockopt(sock_fd.value(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv)) {
    logger.error("Couldn't set receive timeout for socket: {}", strerror(errno));
    return false;
  }

  return true;
}

bool udp_network_gateway_impl::set_reuse_addr()
{
  int one = 1;
  if (setsockopt(sock_fd.value(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
    logger.error("Couldn't set reuseaddr for socket: {}", strerror(errno));
    return false;
  }
  return true;
}

bool udp_network_gateway_impl::set_dscp()
{
  if (not config.dscp.has_value()) {
    return true; // No DSCP code to set.
  }
  uint8_t option = config.dscp.value() << 2; // DSCP is the only the 6 most significant bits.
  if (local_addr.ss_family == AF_INET) {
    if (::setsockopt(sock_fd.value(), IPPROTO_IP, IP_TOS, &option, sizeof(option))) {
      logger.error("Couldn't set DSCP for socket: {}", strerror(errno));
      return false;
    }
    logger.debug("Set DSCP for socket. dscp={}", config.dscp.value());
    return true;
  }
  if (local_addr.ss_family == AF_INET6) {
    if (setsockopt(sock_fd.value(), IPPROTO_IPV6, IPV6_TCLASS, &option, sizeof(option))) {
      logger.error("Couldn't set DSCP for socket: {}", strerror(errno));
      return false;
    }
    logger.debug("Set DSCP for socket. dscp={}", config.dscp.value());
    return true;
  }
  logger.error("Unknown socket familly when setting DSCP");
  return false;
}

bool udp_network_gateway_impl::close_socket()
{
  if (not sock_fd.close()) {
    logger.error("Error closing socket: {}", strerror(errno));
    return false;
  }
  return true;
}
