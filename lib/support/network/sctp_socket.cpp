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

#include "srsran/support/io/sctp_socket.h"
#include "srsran/adt/optional.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/io/sockets.h"
#include "srsran/support/srsran_assert.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <sys/socket.h>

using namespace srsran;

namespace {

/// Subscribes to various SCTP events to handle association and shutdown gracefully.
bool sctp_subscribe_to_events(const unique_fd& fd)
{
  srsran_sanity_check(fd.is_open(), "Invalid FD");
  struct sctp_event_subscribe events = {};
  events.sctp_data_io_event          = 1;
  events.sctp_shutdown_event         = 1;
  events.sctp_association_event      = 1;

  return ::setsockopt(fd.value(), IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events)) == 0;
}

/// \brief Modify SCTP default parameters for quicker detection of broken links.
/// Changes to the maximum re-transmission timeout (rto_max).
bool sctp_set_rto_opts(const unique_fd&      fd,
                       std::optional<int>    rto_initial,
                       std::optional<int>    rto_min,
                       std::optional<int>    rto_max,
                       const std::string&    if_name,
                       srslog::basic_logger& logger)
{
  srsran_sanity_check(fd.is_open(), "Invalid FD");

  if (not rto_initial.has_value() && not rto_min.has_value() && not rto_max.has_value()) {
    // no need to set RTO
    return true;
  }

  // Set RTO_MAX to quickly detect broken links.
  sctp_rtoinfo rto_opts  = {};
  socklen_t    rto_sz    = sizeof(sctp_rtoinfo);
  rto_opts.srto_assoc_id = 0;
  if (getsockopt(fd.value(), SOL_SCTP, SCTP_RTOINFO, &rto_opts, &rto_sz) < 0) {
    logger.error("{}: Error getting RTO_INFO sockopts. errno={}", if_name, strerror(errno));
    return false; // Responsibility of closing the socket is on the caller
  }

  if (rto_initial.has_value()) {
    rto_opts.srto_initial = rto_initial.value();
  }
  if (rto_min.has_value()) {
    rto_opts.srto_min = rto_min.value();
  }
  if (rto_max.has_value()) {
    rto_opts.srto_max = rto_max.value();
  }

  logger.debug(
      "{}: Setting RTO_INFO options on SCTP socket. Association {}, Initial RTO {}, Minimum RTO {}, Maximum RTO {}",
      if_name,
      rto_opts.srto_assoc_id,
      rto_opts.srto_initial,
      rto_opts.srto_min,
      rto_opts.srto_max);

  if (::setsockopt(fd.value(), SOL_SCTP, SCTP_RTOINFO, &rto_opts, rto_sz) < 0) {
    logger.error("{}: Error setting RTO_INFO sockopts. errno={}", if_name, strerror(errno));
    return false;
  }
  return true;
}

/// \brief Modify SCTP default parameters for quicker detection of broken links.
/// Changes to the SCTP_INITMSG parameters (to control the timeout of the connect() syscall)
bool sctp_set_init_msg_opts(const unique_fd&      fd,
                            std::optional<int>    init_max_attempts,
                            std::optional<int>    max_init_timeo,
                            const std::string&    if_name,
                            srslog::basic_logger& logger)
{
  srsran_sanity_check(fd.is_open(), "Invalid FD");

  if (not init_max_attempts.has_value() && not max_init_timeo.has_value()) {
    // No value set for init max attempts or max init_timeo,
    // no need to call set_sockopts()
    return true;
  }

  // Set SCTP INITMSG options to reduce blocking timeout of connect()
  sctp_initmsg init_opts = {};
  socklen_t    init_sz   = sizeof(sctp_initmsg);
  if (getsockopt(fd.value(), SOL_SCTP, SCTP_INITMSG, &init_opts, &init_sz) < 0) {
    logger.error("{}: Error getting sockopts. errno={}", if_name, strerror(errno));
    return false; // Responsibility of closing the socket is on the caller
  }

  if (init_max_attempts.has_value()) {
    init_opts.sinit_max_attempts = init_max_attempts.value();
  }
  if (max_init_timeo.has_value()) {
    init_opts.sinit_max_init_timeo = max_init_timeo.value();
  }

  logger.debug("{}: Setting SCTP_INITMSG options on SCTP socket. Max attempts {}, Max init attempts timeout {}",
               if_name,
               init_opts.sinit_max_attempts,
               init_opts.sinit_max_init_timeo);
  if (::setsockopt(fd.value(), SOL_SCTP, SCTP_INITMSG, &init_opts, init_sz) < 0) {
    logger.error("{}: Error setting SCTP_INITMSG sockopts. errno={}\n", if_name, strerror(errno));
    return false; // Responsibility of closing the socket is on the caller
  }
  return true;
}

/// Set or unset SCTP_NODELAY. With NODELAY enabled, SCTP messages are sent as soon as possible with no unnecessary
/// delay, at the cost of transmitting more packets over the network. Otherwise their transmission might be delayed and
/// concatenated with subsequent messages in order to transmit them in one big PDU.
///
/// Note: If the local interface supports jumbo frames (MTU size > 1500) but not the receiver, then the receiver might
/// discard big PDUs and the stream might get stuck.
bool sctp_set_nodelay(const unique_fd& fd, std::optional<bool> nodelay)
{
  if (not nodelay.has_value()) {
    // no need to change anything
    return true;
  }

  int optval = nodelay.value() ? 1 : 0;
  return ::setsockopt(fd.value(), IPPROTO_SCTP, SCTP_NODELAY, &optval, sizeof(optval)) == 0;
}

} // namespace

// sctp_socket class.

sctp_socket::sctp_socket() : logger(srslog::fetch_basic_logger("SCTP-GW")) {}

expected<sctp_socket> sctp_socket::create(const sctp_socket_params& params)
{
  sctp_socket socket;
  if (params.if_name.empty()) {
    socket.logger.error("Failed to create SCTP socket. Cause: No interface name was provided");
    return make_unexpected(default_error_t{});
  }
  socket.if_name = params.if_name;
  socket.sock_fd = unique_fd{::socket(params.ai_family, params.ai_socktype, IPPROTO_SCTP)};
  if (not socket.sock_fd.is_open()) {
    int ret = errno;
    if (ret == ESOCKTNOSUPPORT) {
      // probably the sctp kernel module is missing on the system, inform the user and exit here
      socket.logger.error(
          "{}: Failed to create SCTP socket: {}. Hint: Please ensure 'sctp' kernel module is available on the system.",
          socket.if_name,
          strerror(ret));
      report_error("{}: Failed to create SCTP socket: {}. Hint: Please ensure 'sctp' kernel module is available on the "
                   "system.\n",
                   socket.if_name,
                   strerror(ret));
    }
    return make_unexpected(default_error_t{});
  }
  socket.logger.debug("{}: SCTP socket created with fd={}", socket.if_name, socket.sock_fd.value());

  if (not socket.set_sockopts(params)) {
    socket.close();
    return make_unexpected(default_error_t{});
  }

  // Save non-blocking mode to apply after bind/connect. We do not yet support async bind/connect.
  socket.non_blocking_mode = params.non_blocking_mode;

  return socket;
}

sctp_socket& sctp_socket::operator=(sctp_socket&& other) noexcept
{
  sock_fd = std::move(other.sock_fd);
  if_name = std::move(other.if_name);
  return *this;
}

sctp_socket::~sctp_socket()
{
  close();
}

bool sctp_socket::close()
{
  if (not sock_fd.is_open()) {
    return true;
  }
  if (not sock_fd.close()) {
    logger.error("{}: Error closing SCTP socket: {}", if_name, strerror(errno));
    return false;
  }
  logger.info("{}: SCTP socket closed", if_name);
  if_name.clear();
  return true;
}

SRSRAN_NODISCARD bool
sctp_socket::bind(struct sockaddr& ai_addr, const socklen_t& ai_addrlen, const std::string& bind_interface)
{
  if (not is_open()) {
    logger.error("Failed to bind to {}. Cause: Socket is closed", get_nameinfo(ai_addr, ai_addrlen));
    return false;
  }

  if (not bind_to_interface(sock_fd, bind_interface, logger)) {
    return false;
  }

  logger.debug("{}: Binding to {}...", if_name, get_nameinfo(ai_addr, ai_addrlen));

  if (::bind(fd().value(), &ai_addr, ai_addrlen) == -1) {
    logger.error("{}: Failed to bind to {}. Cause: {}", if_name, get_nameinfo(ai_addr, ai_addrlen), strerror(errno));
    return false;
  }

  logger.info("{}: Bind to {} was successful", if_name, get_nameinfo(ai_addr, ai_addrlen));

  // set socket to non-blocking after bind is successful
  if (non_blocking_mode) {
    if (not set_non_blocking()) {
      return false;
    }
  }

  return true;
}

SRSRAN_NODISCARD bool sctp_socket::connect(struct sockaddr& ai_addr, const socklen_t& ai_addrlen)
{
  logger.debug("{}: Connecting to {}...", if_name, get_nameinfo(ai_addr, ai_addrlen));
  if (not is_open()) {
    logger.error("Failed to connect to {}. Cause: socket is closed", get_nameinfo(ai_addr, ai_addrlen));
    return false;
  }

  if (::connect(sock_fd.value(), &ai_addr, ai_addrlen) == -1) {
    logger.debug("{}: Failed to connect to {} - {}", if_name, get_nameinfo(ai_addr, ai_addrlen), strerror(errno));
    return false;
  }

  // set socket to non-blocking after connect is established
  if (non_blocking_mode) {
    if (not set_non_blocking()) {
      return false;
    }
  }

  return true;
}

SRSRAN_NODISCARD bool sctp_socket::listen()
{
  if (not is_open()) {
    logger.error("{}: Failed to listen for new SCTP connections. Cause: socket is closed", if_name);
    return false;
  }
  // Listen for connections
  int ret = ::listen(sock_fd.value(), SOMAXCONN);
  if (ret != 0) {
    logger.error("{}: Error in SCTP socket listen: {}", if_name, strerror(errno));
    return false;
  }
  if (logger.info.enabled()) {
    // Note: avoid computing the listen_port if log channel is disabled.
    uint16_t port = get_listen_port().value();
    logger.info("{}: Listening for new SCTP connections on port {}...", if_name, port);
  }
  return true;
}

bool sctp_socket::set_non_blocking()
{
  return ::set_non_blocking(sock_fd, logger);
}

bool sctp_socket::set_sockopts(const sctp_socket_params& params)
{
  logger.debug("Setting socket options. params=[{}]", params);
  if (not sctp_subscribe_to_events(sock_fd)) {
    logger.error(
        "{}: SCTP failed to be created. Cause: Subscribing to SCTP events failed: {}", if_name, strerror(errno));
    return false;
  }

  if (params.rx_timeout.count() > 0) {
    if (not set_receive_timeout(sock_fd, params.rx_timeout, logger)) {
      return false;
    }
  }

  // Set SRTO_MAX
  if (not sctp_set_rto_opts(sock_fd, params.rto_initial, params.rto_min, params.rto_max, if_name, logger)) {
    return false;
  }

  // Set SCTP init options
  if (not sctp_set_init_msg_opts(sock_fd, params.init_max_attempts, params.max_init_timeo, if_name, logger)) {
    return false;
  }

  // Set SCTP NODELAY option
  if (not sctp_set_nodelay(sock_fd, params.nodelay)) {
    logger.error(
        "{}: Could not set SCTP_NODELAY. optval={} error={}", if_name, params.nodelay.value() ? 1 : 0, strerror(errno));
    return false;
  }

  if (params.reuse_addr) {
    if (not set_reuse_addr(sock_fd, logger)) {
      return false;
    }
  }

  return true;
}

std::optional<uint16_t> sctp_socket::get_listen_port() const
{
  if (not sock_fd.is_open()) {
    logger.error("Socket of SCTP network gateway not created.");
    return {};
  }

  sockaddr_storage gw_addr_storage;
  sockaddr*        gw_addr     = (sockaddr*)&gw_addr_storage;
  socklen_t        gw_addr_len = sizeof(gw_addr_storage);

  int ret = getsockname(sock_fd.value(), gw_addr, &gw_addr_len);
  if (ret != 0) {
    logger.error("{}: Failed `getsockname` in SCTP network gateway with sock_fd={}: {}",
                 if_name,
                 sock_fd.value(),
                 strerror(errno));
    return {};
  }

  uint16_t gw_listen_port;
  if (gw_addr->sa_family == AF_INET) {
    gw_listen_port = ntohs(((sockaddr_in*)gw_addr)->sin_port);
  } else if (gw_addr->sa_family == AF_INET6) {
    gw_listen_port = ntohs(((sockaddr_in6*)gw_addr)->sin6_port);
  } else {
    logger.error("{}: Unhandled address family in SCTP network gateway with sock_fd={} family={}",
                 if_name,
                 sock_fd.value(),
                 gw_addr->sa_family);
    return {};
  }

  return gw_listen_port;
}
