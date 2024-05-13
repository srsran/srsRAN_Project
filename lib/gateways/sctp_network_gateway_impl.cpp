/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sctp_network_gateway_impl.h"
#include "srsran/gateways/addr_info.h"
#include "srsran/support/error_handling.h"
#include <fcntl.h>
#include <srsran/support/sockets.h>
#include <utility>

using namespace srsran;

namespace {

/// Subscribes to various SCTP events to handle association and shutdown gracefully.
SRSRAN_NODISCARD bool subscribe_to_events(const unique_fd& fd, srslog::basic_logger& logger)
{
  srsran_sanity_check(fd.is_open(), "Invalid FD");
  struct sctp_event_subscribe events = {};
  events.sctp_data_io_event          = 1;
  events.sctp_shutdown_event         = 1;
  events.sctp_association_event      = 1;

  if (::setsockopt(fd.value(), IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events)) != 0) {
    logger.error("SCTP failed to be created. Cause: Subscribing to SCTP events failed: {}", strerror(errno));
    return false;
  }
  return true;
}

/// Set a receive timeout for a SCTP socket.
SRSRAN_NODISCARD bool
set_receive_timeout(const unique_fd& fd, std::chrono::seconds rx_timeout, srslog::basic_logger& logger)
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

/// \brief Modify SCTP default parameters for quicker detection of broken links.
/// Changes to the maximum re-transmission timeout (rto_max).
bool sctp_set_rto_opts(const unique_fd&      fd,
                       optional<int>         rto_initial,
                       optional<int>         rto_min,
                       optional<int>         rto_max,
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
    logger.error("Error getting RTO_INFO sockopts. errono={}", strerror(errno));
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
      "Setting RTO_INFO options on SCTP socket. Association {}, Initial RTO {}, Minimum RTO {}, Maximum RTO {}",
      rto_opts.srto_assoc_id,
      rto_opts.srto_initial,
      rto_opts.srto_min,
      rto_opts.srto_max);

  if (::setsockopt(fd.value(), SOL_SCTP, SCTP_RTOINFO, &rto_opts, rto_sz) < 0) {
    logger.error("Error setting RTO_INFO sockopts. errno={}", strerror(errno));
    return false;
  }
  return true;
}

/// \brief Modify SCTP default parameters for quicker detection of broken links.
/// Changes to the SCTP_INITMSG parameters (to control the timeout of the connect() syscall)
bool sctp_set_init_msg_opts(const unique_fd&      fd,
                            optional<int>         init_max_attempts,
                            optional<int>         max_init_timeo,
                            srslog::basic_logger& logger)
{
  if (not init_max_attempts.has_value() && not max_init_timeo.has_value()) {
    // No value set for init max attempts or max init_timeo,
    // no need to call set_sockopts()
    return true;
  }

  // Set SCTP INITMSG options to reduce blocking timeout of connect()
  sctp_initmsg init_opts = {};
  socklen_t    init_sz   = sizeof(sctp_initmsg);
  if (getsockopt(fd.value(), SOL_SCTP, SCTP_INITMSG, &init_opts, &init_sz) < 0) {
    logger.error("Error getting sockopts. errno={}", strerror(errno));
    return false; // Responsibility of closing the socket is on the caller
  }

  if (init_max_attempts.has_value()) {
    init_opts.sinit_max_attempts = init_max_attempts.value();
  }
  if (max_init_timeo.has_value()) {
    init_opts.sinit_max_init_timeo = max_init_timeo.value();
  }

  logger.debug("Setting SCTP_INITMSG options on SCTP socket. Max attempts {}, Max init attempts timeout {}",
               init_opts.sinit_max_attempts,
               init_opts.sinit_max_init_timeo);
  if (::setsockopt(fd.value(), SOL_SCTP, SCTP_INITMSG, &init_opts, init_sz) < 0) {
    logger.error("Error setting SCTP_INITMSG sockopts. errno={}\n", strerror(errno));
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
bool sctp_set_nodelay(const unique_fd& fd, optional<bool> nodelay, srslog::basic_logger& logger)
{
  if (not nodelay.has_value()) {
    // no need to change anything
    return true;
  }

  int optval = nodelay.value() == true ? 1 : 0;
  if (::setsockopt(fd.value(), IPPROTO_SCTP, SCTP_NODELAY, &optval, sizeof(optval)) != 0) {
    logger.error("Could not set SCTP_NODELAY. optval={} error={}", optval, strerror(errno));
    return false;
  }
  return true;
}

bool set_reuse_addr(const unique_fd& fd, srslog::basic_logger& logger)
{
  int one = 1;
  if (::setsockopt(fd.value(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) != 0) {
    logger.error("Couldn't set reuseaddr for socket: {}", strerror(errno));
    return false;
  }
  return true;
}

bool set_non_blocking(const unique_fd& fd, srslog::basic_logger& logger)
{
  int flags = fcntl(fd.value(), F_GETFL, 0);
  if (flags == -1) {
    logger.error("Error getting socket flags: {}", strerror(errno));
    return false;
  }

  int s = fcntl(fd.value(), F_SETFL, flags | O_NONBLOCK);
  if (s == -1) {
    logger.error("Error setting socket to non-blocking mode: {}", strerror(errno));
    return false;
  }

  return true;
}

} // namespace

// sctp_socket class.

sctp_socket::sctp_socket() : logger(srslog::fetch_basic_logger("SCTP-GW")) {}

expected<sctp_socket, int> sctp_socket::create(const sctp_socket_params& params)
{
  sctp_socket socket;
  socket.sock_fd = unique_fd{::socket(params.ai_family, params.ai_socktype, params.ai_protocol)};
  if (not socket.sock_fd.is_open()) {
    int ret = errno;
    if (ret == ESOCKTNOSUPPORT) {
      // probably the sctp kernel module is missing on the system, inform the user and exit here
      socket.logger.error(
          "Failed to create SCTP socket: {}. Hint: Please ensure 'sctp' kernel module is available on the system.",
          strerror(ret));
      report_error(
          "Failed to create SCTP socket: {}. Hint: Please ensure 'sctp' kernel module is available on the system.\n",
          strerror(ret));
    }
    return ret;
  }

  if (not socket.set_sockopts(params)) {
    socket.close();
    return errno;
  }
  return socket;
}

sctp_socket& sctp_socket::operator=(sctp_socket&& other) noexcept
{
  sock_fd = std::move(other.sock_fd);
  return *this;
}

sctp_socket::~sctp_socket()
{
  close();
}

bool sctp_socket::close()
{
  if (not sock_fd.close()) {
    logger.error("Error closing SCTP socket: {}", strerror(errno));
    return false;
  }
  return true;
}

bool sctp_socket::set_non_blocking()
{
  srsran_assert(is_open(), "socket is not open");
  return ::set_non_blocking(sock_fd, logger);
}

bool sctp_socket::set_sockopts(const sctp_socket_params& params)
{
  if (not subscribe_to_events(sock_fd, logger)) {
    return false;
  }

  if (params.rx_timeout.count() > 0) {
    if (not set_receive_timeout(sock_fd, params.rx_timeout, logger)) {
      return false;
    }
  }

  // Set SRTO_MAX
  if (not sctp_set_rto_opts(sock_fd, params.rto_initial, params.rto_min, params.rto_max, logger)) {
    return false;
  }

  // Set SCTP init options
  if (not sctp_set_init_msg_opts(sock_fd, params.init_max_attempts, params.max_init_timeo, logger)) {
    return false;
  }

  // Set SCTP NODELAY option
  if (not sctp_set_nodelay(sock_fd, params.nodelay, logger)) {
    return false;
  }

  if (params.reuse_addr) {
    if (not set_reuse_addr(sock_fd, logger)) {
      return false;
    }
  }

  return true;
}

// Gateway Client.

sctp_network_gateway_impl::sctp_network_gateway_impl(sctp_network_gateway_config            config_,
                                                     sctp_network_gateway_control_notifier& ctrl_notfier_,
                                                     network_gateway_data_notifier&         data_notifier_) :
  config(std::move(config_)),
  ctrl_notifier(ctrl_notfier_),
  data_notifier(data_notifier_),
  logger(srslog::fetch_basic_logger("SCTP-GW"))
{
}

sctp_network_gateway_impl::~sctp_network_gateway_impl()
{
  close_socket();
}

expected<sctp_socket, int>
sctp_network_gateway_impl::create_socket(int ai_family, int ai_socktype, int ai_protocol) const
{
  sctp_socket_params params;
  params.ai_family         = ai_family;
  params.ai_socktype       = ai_socktype;
  params.ai_protocol       = ai_protocol;
  params.rx_timeout        = std::chrono::seconds(config.rx_timeout_sec);
  params.rto_initial       = config.rto_initial;
  params.rto_min           = config.rto_min;
  params.rto_max           = config.rto_max;
  params.init_max_attempts = config.init_max_attempts;
  params.max_init_timeo    = config.max_init_timeo;
  params.nodelay           = config.nodelay;
  return sctp_socket::create(params);
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
    auto obj_or_error = create_socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (obj_or_error.is_error()) {
      ret = obj_or_error.error();
      if (ret == ESOCKTNOSUPPORT) {
        break;
      }
      continue;
    }
    socket = std::move(obj_or_error.value());

    // Bind socket to interface (if requested)
    if (not bind_to_interface(socket.fd(), config.bind_interface, logger)) {
      close_socket();
      continue;
    }

    char ip_addr[NI_MAXHOST], port_nr[NI_MAXSERV];
    getnameinfo(
        result->ai_addr, result->ai_addrlen, ip_addr, NI_MAXHOST, port_nr, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    logger.debug("Binding to {} port {}", ip_addr, port_nr);

    if (::bind(socket.fd().value(), result->ai_addr, result->ai_addrlen) == -1) {
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
      if (not socket.set_non_blocking()) {
        // failed, try next address
        close_socket();
        continue;
      }
    }

    logger.debug("Binding successful");
    break;
  }

  freeaddrinfo(results);

  if (not socket.is_open()) {
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
  int ret = ::listen(socket.fd().value(), SOMAXCONN);
  if (ret != 0) {
    logger.error("Error in SCTP socket listen: {}", strerror(errno));
    close_socket();
    return false;
  }

  return true;
}

optional<uint16_t> sctp_network_gateway_impl::get_listen_port()
{
  if (not socket.is_open()) {
    logger.error("Socket of SCTP network gateway not created.");
    return {};
  }

  sockaddr_storage gw_addr_storage;
  sockaddr*        gw_addr     = (sockaddr*)&gw_addr_storage;
  socklen_t        gw_addr_len = sizeof(gw_addr_storage);

  int ret = getsockname(socket.fd().value(), gw_addr, &gw_addr_len);
  if (ret != 0) {
    logger.error(
        "Failed `getsockname` in SCTP network gateway with sock_fd={}: {}", socket.fd().value(), strerror(errno));
    return {};
  }

  uint16_t gw_listen_port;
  if (gw_addr->sa_family == AF_INET) {
    gw_listen_port = ntohs(((sockaddr_in*)gw_addr)->sin_port);
  } else if (gw_addr->sa_family == AF_INET6) {
    gw_listen_port = ntohs(((sockaddr_in6*)gw_addr)->sin6_port);
  } else {
    logger.error("Unhandled address family in SCTP network gateway with sock_fd={}, family={}",
                 socket.fd().value(),
                 gw_addr->sa_family);
    return {};
  }

  logger.debug("Read bind port of SCTP network gateway: {}", gw_listen_port);
  return gw_listen_port;
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
    if (not socket.is_open()) {
      auto outcome = create_socket(result->ai_family, result->ai_socktype, result->ai_protocol);
      if (outcome.is_error()) {
        ret = outcome.error();
        if (ret == ESOCKTNOSUPPORT) {
          break;
        }
        continue;
      }
      socket = std::move(outcome.value());
    }

    char ip_addr[NI_MAXHOST], port_nr[NI_MAXSERV];
    getnameinfo(
        result->ai_addr, result->ai_addrlen, ip_addr, NI_MAXHOST, port_nr, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    logger.debug("Connecting to {} port {}", ip_addr, port_nr);

    if (::connect(socket.fd().value(), result->ai_addr, result->ai_addrlen) == -1) {
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
      if (not socket.set_non_blocking()) {
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

  if (not socket.fd().is_open()) {
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
  auto ret = create_socket(server_ai_family, server_ai_socktype, server_ai_protocol);
  if (ret.is_error()) {
    logger.error("Failed to recreate socket: {}", strerror(ret.error()));
    return false;
  }
  socket = std::move(ret.value());

  // set socket to non-blocking before reconnecting
  if (config.non_blocking_mode) {
    if (not socket.set_non_blocking()) {
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
  if (::bind(socket.fd().value(), (sockaddr*)&server_addr, server_addrlen) == -1) {
    logger.error("Failed to bind to {}:{} - {}", ip_addr, port_nr, strerror(errno));
    close_socket();
    return false;
  }

  // reconnect to address/port
  if (::connect(socket.fd().value(), (sockaddr*)&server_addr, server_addrlen) == -1 && errno != EINPROGRESS) {
    logger.error("Failed to connect to {}:{} - {}", ip_addr, port_nr, strerror(errno));
    close_socket();
    return false;
  }

  return true;
}

/// Close socket handle and set FD to -1
bool sctp_network_gateway_impl::close_socket()
{
  // Stop listening to new IO Rx events.
  io_sub.reset();

  // Close SCTP socket.
  return socket.close();
}

void sctp_network_gateway_impl::receive()
{
  struct sctp_sndrcvinfo sri       = {};
  int                    msg_flags = 0;

  // Fixme: consider class member on heap when sequential access is guaranteed
  std::array<uint8_t, network_gateway_sctp_max_len> tmp_mem; // no init

  int rx_bytes = ::sctp_recvmsg(socket.fd().value(),
                                tmp_mem.data(),
                                network_gateway_sctp_max_len,
                                (struct sockaddr*)&msg_src_addr,
                                &msg_src_addrlen,
                                &sri,
                                &msg_flags);

  if (rx_bytes == -1 && errno != EAGAIN) {
    logger.error("Error reading from SCTP socket: {}", strerror(errno));
  } else if (rx_bytes == -1 && errno == EAGAIN) {
    if (!config.non_blocking_mode) {
      logger.debug("Socket timeout reached");
    }
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
  return socket.fd().value();
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

  // Note: For SCTP, we avoid byte buffer allocation failures by resorting to fallback allocation.
  data_notifier.on_new_pdu(byte_buffer{byte_buffer::fallback_allocation_tag{}, payload});
}

void sctp_network_gateway_impl::handle_io_error(io_broker::error_code code)
{
  logger.info("Connection loss due to IO error code={}.", (int)code);
  io_sub.reset();
  ctrl_notifier.on_connection_loss();
}

///< Process outgoing PDU and send over SCTP socket to peer.
void sctp_network_gateway_impl::handle_pdu(const byte_buffer& pdu)
{
  logger.debug("Sending PDU of {} bytes", pdu.length());

  if (not socket.fd().is_open()) {
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

  int bytes_sent = sctp_sendmsg(socket.fd().value(),
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

bool sctp_network_gateway_impl::subscribe_to(io_broker& broker)
{
  io_sub = broker.register_fd(
      socket.fd().value(), [this]() { receive(); }, [this](io_broker::error_code code) { handle_io_error(code); });
  return io_sub.registered();
}
