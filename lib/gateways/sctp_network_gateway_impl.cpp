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

#include "sctp_network_gateway_impl.h"
#include "srsran/gateways/addr_info.h"
#include <fcntl.h>
#include <srsran/support/sockets.h>
#include <utility>

using namespace srsran;

sctp_network_gateway_impl::sctp_network_gateway_impl(sctp_network_gateway_config            config_,
                                                     sctp_network_gateway_control_notifier& ctrl_notfier_,
                                                     network_gateway_data_notifier&         data_notifier_) :
  config(std::move(config_)),
  ctrl_notifier(ctrl_notfier_),
  data_notifier(data_notifier_),
  logger(srslog::fetch_basic_logger("SCTP-GW"))
{
}

bool sctp_network_gateway_impl::set_sockopts()
{
  if (not subscripe_to_events()) {
    logger.error("Couldn't subscribe to SCTP events");
    return false;
  }

  if (config.rx_timeout_sec > 0) {
    if (not set_receive_timeout(config.rx_timeout_sec)) {
      logger.error("Couldn't set receive timeout for socket");
      return false;
    }
  }

  // Set SRTO_MAX
  if (not sctp_set_rto_opts(sock_fd, config.rto_initial, config.rto_min, config.rto_max, logger)) {
    return false;
  }

  // Set SCTP init options
  if (not sctp_set_init_msg_opts(sock_fd, config.init_max_attempts, config.max_init_timeo, logger)) {
    return false;
  }

  if (config.reuse_addr) {
    if (not set_reuse_addr()) {
      logger.error("Couldn't set reuseaddr for socket");
      return false;
    }
  }

  return true;
}

/// \brief Subscribes to various SCTP events to handle accociation and shutdown gracefully.
bool sctp_network_gateway_impl::subscripe_to_events()
{
  struct sctp_event_subscribe events = {};
  events.sctp_data_io_event          = 1;
  events.sctp_shutdown_event         = 1;
  events.sctp_association_event      = 1;

  if (::setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events)) != 0) {
    logger.error("Subscribing to SCTP events failed: {}", strerror(errno));
    return false;
  }
  return true;
}

bool sctp_network_gateway_impl::set_receive_timeout(unsigned rx_timeout_sec)
{
  struct timeval tv;
  tv.tv_sec  = rx_timeout_sec;
  tv.tv_usec = 0;

  if (::setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) != 0) {
    logger.error("Couldn't set receive timeout for socket: {}", strerror(errno));
    return false;
  }

  return true;
}

bool sctp_network_gateway_impl::set_reuse_addr()
{
  int one = 1;
  if (::setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) != 0) {
    logger.error("Couldn't set reuseaddr for socket: {}", strerror(errno));
    return false;
  }
  return true;
}

/// \brief Verify that socket has been created and can be used.
/// @return True if socket is valid, false otherwise.
bool sctp_network_gateway_impl::is_initialized()
{
  return sock_fd != -1;
}

/// \brief Create and bind socket to given address.
bool sctp_network_gateway_impl::create_and_bind()
{
  struct addrinfo hints;
  // support ipv4, ipv6 and hostnames
  hints.ai_family    = AF_UNSPEC;
  hints.ai_socktype  = SOCK_SEQPACKET;
  hints.ai_flags     = 0;
  hints.ai_protocol  = IPPROTO_SCTP;
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
    // create SCTP socket
    sock_fd = ::socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock_fd == -1) {
      ret = errno;
      if (ret == ESOCKTNOSUPPORT) {
        // probably the sctp kernel module is missing on the system, inform the user and exit here
        logger.error(
            "Failed to create SCTP socket: {}. Hint: Please ensure 'sctp' kernel module is available on the system.",
            strerror(ret));
        report_error(
            "Failed to create SCTP socket: {}. Hint: Please ensure 'sctp' kernel module is available on the system.\n",
            strerror(ret));
        break;
      }
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

    if (::bind(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
      // binding failed, try next address
      ret = errno;
      logger.debug("Failed to bind to {}:{} - {}", ip_addr, port_nr, strerror(ret));
      close_socket();
      continue;
    }

    // store client address
    std::memcpy(&client_addr, result->ai_addr, result->ai_addrlen);
    client_addrlen     = result->ai_addrlen;
    client_ai_family   = result->ai_family;
    client_ai_socktype = result->ai_socktype;
    client_ai_protocol = result->ai_protocol;

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

bool sctp_network_gateway_impl::listen()
{
  // Listen for connections
  int ret = ::listen(sock_fd, SOMAXCONN);
  if (ret != 0) {
    logger.error("Error in SCTP socket listen: {}", strerror(errno));
    close_socket();
    return false;
  }

  return true;
}

bool sctp_network_gateway_impl::create_and_connect()
{
  // bind to address/port
  if (not config.bind_address.empty()) {
    if (not create_and_bind()) {
      logger.error("Couldn't bind to address {}:{}", config.bind_address, config.bind_port);
      close_socket();
      return false;
    }
  }

  struct addrinfo hints;
  // support ipv4, ipv6 and hostnames
  hints.ai_family    = AF_UNSPEC;
  hints.ai_socktype  = SOCK_SEQPACKET;
  hints.ai_flags     = 0;
  hints.ai_protocol  = IPPROTO_SCTP;
  hints.ai_canonname = nullptr;
  hints.ai_addr      = nullptr;
  hints.ai_next      = nullptr;

  std::string      connect_port = std::to_string(config.connect_port);
  struct addrinfo* results;

  int ret = getaddrinfo(config.connect_address.c_str(), connect_port.c_str(), &hints, &results);
  if (ret != 0) {
    logger.error("Getaddrinfo error: {} - {}", config.connect_address, gai_strerror(ret));
    return false;
  }

  fmt::print("Connecting to {} on {}:{}\n",
             config.connection_name.c_str(),
             config.connect_address.c_str(),
             connect_port.c_str());
  std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
  struct addrinfo*                                   result;
  for (result = results; result != nullptr; result = result->ai_next) {
    // Create SCTP socket only if not created in create_and_bind function.
    if (sock_fd == -1) {
      sock_fd = ::socket(result->ai_family, result->ai_socktype, result->ai_protocol);
      if (sock_fd == -1) {
        ret = errno;
        if (ret == ESOCKTNOSUPPORT) {
          // probably the sctp kernel module is missing on the system, inform the user and exit here
          logger.error(
              "Failed to create SCTP socket: {}. Hint: Please ensure 'sctp' kernel module is available on the system.",
              strerror(ret));
          report_error("Failed to create SCTP socket: {}. Hint: Please ensure 'sctp' kernel module is available on the "
                       "system.\n",
                       strerror(ret));
          break;
        }
        continue;
      }
    }

    if (not set_sockopts()) {
      close_socket();
      continue;
    }

    char ip_addr[NI_MAXHOST], port_nr[NI_MAXSERV];
    getnameinfo(
        result->ai_addr, result->ai_addrlen, ip_addr, NI_MAXHOST, port_nr, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    logger.debug("Connecting to {} port {}", ip_addr, port_nr);

    if (::connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
      // connection failed, try next address
      ret = errno;
      logger.debug("Failed to connect to {}:{} - {}", ip_addr, port_nr, strerror(ret));
      close_socket();
      continue;
    }

    // store server address
    std::memcpy(&server_addr, result->ai_addr, result->ai_addrlen);
    server_addrlen     = result->ai_addrlen;
    server_ai_family   = result->ai_family;
    server_ai_socktype = result->ai_socktype;
    server_ai_protocol = result->ai_protocol;

    // set socket to non-blocking after connect is established
    if (config.non_blocking_mode) {
      if (not set_non_blocking()) {
        // failed, try next address
        logger.error("Socket not non-blocking");
        close_socket();
        continue;
      }
    }

    // If connected then use server address as destination address.
    std::memcpy(&msg_dst_addr, &server_addr, server_addrlen);
    msg_dst_addrlen = server_addrlen;

    logger.debug("Connection successful");
    break;
  }

  freeaddrinfo(results);

  if (sock_fd == -1) {
    std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    fmt::print("Failed to connect {} socket to {}:{}. error=\"{}\" timeout={}ms\n",
               ipproto_to_string(hints.ai_protocol),
               config.connect_address,
               config.connect_port,
               strerror(ret),
               now_ms.count());
    logger.error("Failed to connect {} socket to {}:{}. error=\"{}\" timeout={}ms",
                 ipproto_to_string(hints.ai_protocol),
                 config.connect_address,
                 config.connect_port,
                 strerror(ret),
                 now_ms.count());
    return false;
  }

  return true;
}

bool sctp_network_gateway_impl::recreate_and_reconnect()
{
  // Recreate socket
  sock_fd = ::socket(server_ai_family, server_ai_socktype, server_ai_protocol);
  if (sock_fd == -1) {
    logger.error("Failed to recreate socket: {}", strerror(errno));
    return false;
  }

  if (not set_sockopts()) {
    close_socket();
  }

  // set socket to non-blocking before reconnecting
  if (config.non_blocking_mode) {
    if (not set_non_blocking()) {
      logger.error("Socket not non-blocking");
      close_socket();
      return false;
    }
  }

  char ip_addr[NI_MAXHOST], port_nr[NI_MAXSERV];
  getnameinfo((sockaddr*)&server_addr,
              server_addrlen,
              ip_addr,
              NI_MAXHOST,
              port_nr,
              NI_MAXSERV,
              NI_NUMERICHOST | NI_NUMERICSERV);

  // rebind to address/port
  if (::bind(sock_fd, (sockaddr*)&server_addr, server_addrlen) == -1) {
    logger.error("Failed to bind to {}:{} - {}", ip_addr, port_nr, strerror(errno));
    close_socket();
    return false;
  }

  // reconnect to address/port
  if (::connect(sock_fd, (sockaddr*)&server_addr, server_addrlen) == -1 && errno != EINPROGRESS) {
    logger.error("Failed to connect to {}:{} - {}", ip_addr, port_nr, strerror(errno));
    close_socket();
    return false;
  }

  return true;
}

/// Close socket handle and set FD to -1
bool sctp_network_gateway_impl::close_socket()
{
  logger.debug("Closing socket.");
  if (not is_initialized()) {
    return true;
  }

  if (::close(sock_fd) == -1) {
    logger.error("Error closing socket: {}", strerror(errno));
    return false;
  }

  sock_fd = -1;

  return true;
}

void sctp_network_gateway_impl::receive()
{
  struct sctp_sndrcvinfo sri       = {};
  int                    msg_flags = 0;

  // Fixme: consider class member on heap when sequential access is guaranteed
  std::array<uint8_t, network_gateway_sctp_max_len> tmp_mem; // no init

  int rx_bytes = ::sctp_recvmsg(sock_fd,
                                tmp_mem.data(),
                                network_gateway_sctp_max_len,
                                (struct sockaddr*)&msg_src_addr,
                                &msg_src_addrlen,
                                &sri,
                                &msg_flags);

  if (rx_bytes == -1 && errno != EAGAIN) {
    logger.error("Error reading from SCTP socket: {}", strerror(errno));
  } else if (rx_bytes == -1 && errno == EAGAIN) {
    logger.debug("Socket timeout reached");
  } else {
    logger.debug("Received {} bytes on SCTP socket", rx_bytes);
    span<socket_buffer_type> payload(tmp_mem.data(), rx_bytes);
    if (msg_flags & MSG_NOTIFICATION) {
      // Received notification
      handle_notification(payload);
    } else {
      // Received data
      handle_data(payload);
    }
  }
}

int sctp_network_gateway_impl::get_socket_fd()
{
  return sock_fd;
}

int sctp_network_gateway_impl::get_bind_port()
{
  int gw_bind_port = 0;

  if (not is_initialized()) {
    logger.error("Socket of SCTP network gateway not initialized.");
    return gw_bind_port;
  }

  sockaddr_in gw_addr;
  socklen_t   gw_addr_len = sizeof(gw_addr);

  int ret = getsockname(sock_fd, (struct sockaddr*)&gw_addr, &gw_addr_len);
  if (ret != 0) {
    logger.error("Failed `getsockname` in SCTP network gateway with sock_fd={}: {}", sock_fd, strerror(errno));
  }
  gw_bind_port = ntohs(gw_addr.sin_port);

  logger.debug("Read bind port of UDP network gateway: {}", gw_bind_port);
  return gw_bind_port;
}

void sctp_network_gateway_impl::handle_notification(span<socket_buffer_type> payload)
{
  union sctp_notification* notif             = (union sctp_notification*)payload.data();
  uint32_t                 notif_header_size = sizeof(((union sctp_notification*)NULL)->sn_header);
  if (notif_header_size > payload.size_bytes()) {
    logger.error("Notification msg size ({} B) is smaller than notification header size ({} B)",
                 payload.size_bytes(),
                 notif_header_size);
    return;
  }

  switch (notif->sn_header.sn_type) {
    case SCTP_ASSOC_CHANGE: {
      if (sizeof(struct sctp_assoc_change) > payload.size_bytes()) {
        logger.error("Notification msg size ({} B) is smaller than struct sctp_assoc_change size ({} B)",
                     payload.size_bytes(),
                     sizeof(struct sctp_assoc_change));
        return;
      }

      const char*               state = NULL;
      struct sctp_assoc_change* n     = &notif->sn_assoc_change;

      switch (n->sac_state) {
        case SCTP_COMM_UP:
          state = "COMM UP";
          ctrl_notifier.on_connection_established();
          break;
        case SCTP_COMM_LOST:
          state = "COMM_LOST";
          ctrl_notifier.on_connection_loss();
          break;
        case SCTP_RESTART:
          state = "RESTART";
          break;
        case SCTP_SHUTDOWN_COMP:
          state = "SHUTDOWN_COMP";
          break;
        case SCTP_CANT_STR_ASSOC:
          state = "CAN'T START ASSOC";
          ctrl_notifier.on_connection_loss();
          break;
      }

      logger.debug("SCTP_ASSOC_CHANGE notif: state: {}, error code: {}, out streams: {}, in streams: {}, assoc id: {}",
                   state,
                   n->sac_error,
                   n->sac_outbound_streams,
                   n->sac_inbound_streams,
                   n->sac_assoc_id);
      break;
    }

    case SCTP_SHUTDOWN_EVENT: {
      if (sizeof(struct sctp_shutdown_event) > payload.size_bytes()) {
        logger.error("Error notification msg size is smaller than struct sctp_shutdown_event size");
        return;
      }
      struct sctp_shutdown_event* n = &notif->sn_shutdown_event;
      logger.debug("SCTP_SHUTDOWN_EVENT notif: assoc id: {}", n->sse_assoc_id);
      ctrl_notifier.on_connection_loss();
      break;
    }

    default:
      logger.warning("Unhandled notification type {}", notif->sn_header.sn_type);
      break;
  }
}

void sctp_network_gateway_impl::handle_data(const span<socket_buffer_type> payload)
{
  logger.debug("Received data of {} bytes", payload.size_bytes());
  data_notifier.on_new_pdu(byte_buffer(payload.begin(), payload.end()));
}

///< Process outgoing PDU and send over SCTP socket to peer.
void sctp_network_gateway_impl::handle_pdu(const byte_buffer& pdu)
{
  logger.debug("Sending PDU of {} bytes", pdu.length());

  if (not is_initialized()) {
    logger.error("Socket not initialized");
    return;
  }

  if (pdu.length() > network_gateway_sctp_max_len) {
    logger.error("PDU of {} bytes exceeds maximum length of {} bytes", pdu.length(), network_gateway_sctp_max_len);
    return;
  }

  // Fixme: consider class member on heap when sequential access is guaranteed
  std::array<uint8_t, network_gateway_sctp_max_len> tmp_mem; // no init

  span<const uint8_t> pdu_span = to_span(pdu, tmp_mem);

  if (not server_addrlen) {
    // If only bind, send msg to the last src address.
    std::memcpy(&msg_dst_addr, &msg_src_addr, msg_src_addrlen);
    msg_dst_addrlen = msg_src_addrlen;
  }

  int bytes_sent = sctp_sendmsg(sock_fd,
                                pdu_span.data(),
                                pdu_span.size_bytes(),
                                (struct sockaddr*)&msg_dst_addr,
                                msg_dst_addrlen,
                                htonl(config.ppid),
                                0,
                                stream_no,
                                0,
                                0);
  if (bytes_sent == -1) {
    logger.error("Couldn't send {} B of data on SCTP socket: {}", pdu_span.size_bytes(), strerror(errno));
    return;
  }
}

///< Set socket to non-blocking-mode.
bool sctp_network_gateway_impl::set_non_blocking()
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
