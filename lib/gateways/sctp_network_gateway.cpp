/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sctp_network_gateway.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/sctp.h>
#include <unistd.h>

using namespace srsgnb;

sctp_network_gateway::sctp_network_gateway(network_gateway_config            config_,
                                           network_gateway_control_notifier& ctrl_notfier_,
                                           network_gateway_data_notifier&    data_notifier_) :
  config(config_),
  ctrl_notifier(ctrl_notfier_),
  data_notifier(data_notifier_),
  logger(srslog::fetch_basic_logger("SCTP-NW-GW"))
{
  address_family = get_address_family(config.bind_address.c_str());
  if (address_family < 0) {
    // failed to get address family for bind address, try connect address
    address_family = get_address_family(config.connect_address.c_str());
    if (address_family < 0) {
      logger.error("Couldn't determine address family for {} or {}", config.bind_address, config.connect_address);
      return;
    }
  }

  // create SCTP socket
  sock_fd = socket(address_family, SOCK_SEQPACKET, IPPROTO_SCTP);
  if (sock_fd == -1) {
    logger.error("Could not create SCTP socket");
    return;
  }

  if (not subscripe_to_events()) {
    logger.error("Could not subscribe to SCTP events");
    close_socket();
    return;
  }

  if (config.rx_timeout_sec > 0) {
    if (not set_receive_timeout(config.rx_timeout_sec)) {
      logger.error("Couldn't set receive timeout for socket");
      close_socket();
      return;
    }
  }

  if (config.non_blocking_mode) {
    if (not set_non_blocking()) {
      logger.error("Socket not non-blocking");
      return;
    }
  }
}

/// \brief Subscribes to various SCTP events to handle accociation and shutdown gracefully.
bool sctp_network_gateway::subscripe_to_events()
{
  struct sctp_event_subscribe events = {};
  events.sctp_data_io_event          = 1;
  events.sctp_shutdown_event         = 1;
  events.sctp_association_event      = 1;

  if (setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events))) {
    logger.error("Subscribing to SCTP events failed");
    return false;
  }
  return true;
}

bool sctp_network_gateway::set_receive_timeout(unsigned rx_timeout_sec)
{
  struct timeval tv;
  tv.tv_sec  = rx_timeout_sec;
  tv.tv_usec = 0;

  if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv)) {
    logger.error("Couldn't set receive timeout for socket");
    return false;
  }

  return true;
}

/// \brief Verify that socket has been created and can be used.
/// @return True if socket is valid, false otherwise.
bool sctp_network_gateway::is_initialized()
{
  return sock_fd != -1;
}

/// \brief Bind socket to given address and starts listenening for incoming connections.
bool sctp_network_gateway::bind()
{
  // Bind port
  struct sockaddr_in  bind_addr_v4 = {};
  struct sockaddr_in6 bind_addr_v6 = {};
  struct sockaddr*    bind_addr    = nullptr;
  size_t              bind_addr_sz = 0;
  if (address_family == AF_INET) {
    if (not set_sockaddr(&bind_addr_v4, config.bind_address.c_str(), config.bind_port)) {
      logger.error("Invalid net_ip: {}", config.bind_address.c_str());
      return false;
    }
    bind_addr    = (struct sockaddr*)&bind_addr_v4;
    bind_addr_sz = sizeof(bind_addr_v4);
  } else if (address_family == AF_INET6) {
    if (not set_sockaddr(&bind_addr_v6, config.bind_address.c_str(), config.bind_port)) {
      logger.error("Invalid net_ip: {}", config.bind_address.c_str());
      return false;
    }
    bind_addr    = (struct sockaddr*)&bind_addr_v6;
    bind_addr_sz = sizeof(bind_addr_v6);
  } else {
    return false;
  }

  int one = 1;
  if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
    close_socket();
    logger.error("Couldn't set receive timeout for socket");
    return false;
  }

  int ret = ::bind(sock_fd, bind_addr, bind_addr_sz);
  if (ret != 0) {
    logger.error("Error binding SCTP socket");
    close_socket();
    return false;
  }

  return true;
}

bool sctp_network_gateway::listen()
{
  // Listen for connections
  int ret = ::listen(sock_fd, SOMAXCONN);
  if (ret != 0) {
    logger.error("Error in SCTP socket listen");
    close_socket();
    return false;
  }

  return true;
}

bool sctp_network_gateway::connect()
{
  if (not is_initialized()) {
    logger.error("Socket not initialized");
    return false;
  }

  // bind to address/port
  if (not config.bind_address.empty()) {
    if (not bind()) {
      logger.error("Couldn't bind to address {}:{}", config.bind_address, config.bind_port);
      close_socket();
      return false;
    }
  }

  // Connect address
  if (address_family == AF_INET) {
    if (not set_sockaddr(&connect_addr_v4, config.connect_address.c_str(), config.connect_port)) {
      logger.error("Invalid IPv4 address: {}", config.connect_address);
      return false;
    }
    servaddr    = (struct sockaddr*)&connect_addr_v4;
    servaddr_sz = sizeof(connect_addr_v4);
  } else if (address_family == AF_INET6) {
    if (not set_sockaddr(&connect_addr_v6, config.connect_address.c_str(), config.connect_port)) {
      logger.error("Invalid IPv6 address: {}", config.connect_address);
      return false;
    }
    servaddr    = (struct sockaddr*)&connect_addr_v6;
    servaddr_sz = sizeof(connect_addr_v6);
  } else {
    return false;
  }

  // Connect
  int ret = ::connect(sock_fd, servaddr, servaddr_sz);
  if (ret == -1 && errno != EINPROGRESS) {
    logger.error("Failed to establish socket connection to {}: {}", config.connect_address, strerror(errno));
    close_socket();
    return false;
  }

  return true;
}

///< Close socket handle and set FD to -1
bool sctp_network_gateway::close_socket()
{
  if (not is_initialized()) {
    logger.error("Socket not initialized");
    return false;
  }

  if (close(sock_fd) == -1) {
    logger.error("Error closing socket");
    return false;
  }

  sock_fd = -1;

  return true;
}

///< Set IP:port for ipv4
int sctp_network_gateway::get_address_family(const char* ip_str)
{
  in6_addr tmp = {};
  if (inet_pton(AF_INET, ip_str, (void*)&tmp) == 1) {
    return AF_INET; // valid IPv4 address
  }
  if (inet_pton(AF_INET6, ip_str, (void*)&tmp) == 1) {
    return AF_INET6; // valid IPv6 address
  }
  logger.error("Couldn't determine IP address family.");
  return -1;
}

///< Set IP:port for ipv4
bool sctp_network_gateway::set_sockaddr(sockaddr_in* addr, const char* ip_str, int port)
{
  addr->sin_family = AF_INET;
  if (::inet_pton(addr->sin_family, ip_str, &addr->sin_addr) != 1) {
    logger.error("Couldn't convert IPv4 address {}:{}, error: {}", ip_str, port, strerror(errno));
    return false;
  }
  addr->sin_port = htons(port);
  return true;
}

///< Set IP:port for ipv6
bool sctp_network_gateway::set_sockaddr(sockaddr_in6* addr, const char* ip_str, int port)
{
  addr->sin6_family = AF_INET6;
  if (::inet_pton(addr->sin6_family, ip_str, &addr->sin6_addr) != 1) {
    logger.error("Couldn't convert IPv6 address {}:{}, error: {}", ip_str, port, strerror(errno));
    return false;
  }
  addr->sin6_port = htons(port);
  return true;
}

void sctp_network_gateway::receive()
{
  struct sctp_sndrcvinfo sri       = {};
  socklen_t              fromlen   = sizeof(client_addr);
  int                    msg_flags = 0;
  int                    rx_bytes  = ::sctp_recvmsg(
      sock_fd, rx_buffer.data(), rx_buffer.size(), (struct sockaddr*)&client_addr, &fromlen, &sri, &msg_flags);
  if (rx_bytes == -1 && errno != EAGAIN) {
    logger.error("Error reading from SCTP socket: %s", strerror(errno));
  } else if (rx_bytes == -1 && errno == EAGAIN) {
    logger.debug("Socket timeout reached");
  } else {
    logger.debug("Received {} bytes on SCTP socket", rx_bytes);
    span<socket_buffer_type> payload(rx_buffer.data(), rx_bytes);
    if (msg_flags & MSG_NOTIFICATION) {
      // Received notification
      handle_notification(payload);
    } else {
      // Received data
      handle_data(payload);
    }
  }
}

int sctp_network_gateway::get_socket_fd()
{
  return sock_fd;
}

void sctp_network_gateway::handle_notification(span<socket_buffer_type> payload)
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

void sctp_network_gateway::handle_data(const span<socket_buffer_type> payload)
{
  logger.info("Received data of {} bytes", payload.size_bytes());
  data_notifier.on_new_pdu(byte_buffer(payload.begin(), payload.end()));
}

///< Process outgoing PDU and send over SCTP socket to peer.
void sctp_network_gateway::handle_pdu(const byte_buffer& pdu)
{
  logger.debug("Sending PDU of {} bytes", pdu.length());

  if (not is_initialized()) {
    logger.error("Socket not initialized");
    return;
  }

  // send segment by segment over socket
  for (const auto& segment : pdu.segments()) {
    int bytes_sent =
        sctp_sendmsg(sock_fd, segment.data(), segment.size_bytes(), servaddr, servaddr_sz, ppi, 0, stream_no, 0, 0);
    if (bytes_sent == -1) {
      logger.error("Couldn't send {} B of data on SCTP socket", pdu.length());
      return;
    }
  }
}

///< Set socket to non-blocking-mode.
bool sctp_network_gateway::set_non_blocking()
{
  int flags = fcntl(sock_fd, F_GETFL, 0);
  if (flags == -1) {
    logger.error("Error getting socket flags: {}", strerror(errno));
    return false;
  }

  flags |= O_NONBLOCK;
  int s = fcntl(sock_fd, F_SETFL, flags);
  if (s == -1) {
    logger.error("Error setting socket to non-blocking mode: {}", strerror(errno));
    return false;
  }

  return true;
}
