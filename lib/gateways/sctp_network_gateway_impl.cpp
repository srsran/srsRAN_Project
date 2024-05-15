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
#include "srsran/support/io/sockets.h"
#include <netdb.h>
#include <netinet/sctp.h>
#include <sys/socket.h>
#include <utility>

using namespace srsran;

constexpr uint32_t network_gateway_sctp_max_len = 9100;

namespace {

/// Helper generator class that traverses a list of SCTP sockaddr param candidates (ipv4, ipv6, hostnames).
class sockaddr_searcher
{
public:
  sockaddr_searcher(const std::string& address, int port, srslog::basic_logger& logger)
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
      logger.error("Getaddrinfo error: {} - {}", address, gai_strerror(ret));
      results = nullptr;
      return;
    }
    next_result = results;
  }
  sockaddr_searcher(const sockaddr_searcher&) = delete;
  sockaddr_searcher(sockaddr_searcher&&)      = delete;
  ~sockaddr_searcher() { freeaddrinfo(results); }

  /// Get next candidate or nullptr of search has ended.
  struct addrinfo* next()
  {
    struct addrinfo* ret = next_result;
    if (next_result != nullptr) {
      next_result = next_result->ai_next;
    }
    return ret;
  }

private:
  struct addrinfo* results     = nullptr;
  struct addrinfo* next_result = nullptr;
};

} // namespace

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

expected<sctp_socket> sctp_network_gateway_impl::create_socket(int ai_family, int ai_socktype, int ai_protocol) const
{
  sctp_socket_params params;
  params.ai_family         = ai_family;
  params.ai_socktype       = ai_socktype;
  params.ai_protocol       = ai_protocol;
  params.reuse_addr        = config.reuse_addr;
  params.non_blocking_mode = config.non_blocking_mode;
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
  sockaddr_searcher searcher{config.bind_address, config.bind_port, logger};
  struct addrinfo*  result = nullptr;
  for (result = searcher.next(); result != nullptr; result = searcher.next()) {
    // create SCTP socket
    auto outcome = create_socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (outcome.is_error()) {
      if (errno == ESOCKTNOSUPPORT) {
        // Stop search.
        break;
      }
      continue;
    }
    sctp_socket& candidate = outcome.value();

    if (not candidate.bind(*result->ai_addr, result->ai_addrlen, config.bind_interface)) {
      // Bind failed. Try next candidate
      continue;
    }

    // Save socket.
    socket = std::move(candidate);
    break;
  }

  if (not socket.is_open()) {
    fmt::print("Failed to bind SCTP socket to {}:{}. {}\n", config.bind_address, config.bind_port, strerror(errno));
    logger.error("Failed to bind SCTP socket to {}:{}. {}", config.bind_address, config.bind_port, strerror(errno));
    return false;
  }

  logger.debug("Binding successful");

  return true;
}

bool sctp_network_gateway_impl::listen()
{
  return socket.listen();
}

optional<uint16_t> sctp_network_gateway_impl::get_listen_port()
{
  return socket.get_listen_port();
}

bool sctp_network_gateway_impl::create_and_connect()
{
  // bind to address/port
  if (not config.bind_address.empty()) {
    if (not create_and_bind()) {
      logger.error("Couldn't bind to address {}:{}", config.bind_address, config.bind_port);
      return false;
    }
  }

  fmt::print("Connecting to {} on {}:{}\n",
             config.connection_name.c_str(),
             config.connect_address.c_str(),
             config.connect_port);

  sockaddr_searcher                                  searcher{config.connect_address, config.connect_port, logger};
  std::chrono::time_point<std::chrono::steady_clock> start  = std::chrono::steady_clock::now();
  struct addrinfo*                                   result = nullptr;
  for (result = searcher.next(); result != nullptr; result = searcher.next()) {
    // Create SCTP socket only if not created in create_and_bind function.
    if (not socket.is_open()) {
      expected<sctp_socket> outcome = create_socket(result->ai_family, result->ai_socktype, result->ai_protocol);
      if (outcome.is_error()) {
        if (errno == ESOCKTNOSUPPORT) {
          break;
        }
        continue;
      }
      socket = std::move(outcome.value());
    }

    if (not socket.connect(*result->ai_addr, result->ai_addrlen)) {
      // connection failed, try next address
      close_socket();
      continue;
    }

    break;
  }

  if (not socket.fd().is_open()) {
    std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    fmt::print("Failed to connect SCTP socket to {}:{}. error=\"{}\" timeout={}ms\n",
               config.connect_address,
               config.connect_port,
               strerror(errno),
               now_ms.count());
    logger.error("Failed to connect SCTP socket to {}:{}. error=\"{}\" timeout={}ms",
                 config.connect_address,
                 config.connect_port,
                 strerror(errno),
                 now_ms.count());
    return false;
  }

  // Connect was successful.
  client_mode = true;

  // Use server address as destination address.
  std::memcpy(&msg_dst_addr, result->ai_addr, result->ai_addrlen);
  msg_dst_addrlen = result->ai_addrlen;

  logger.debug("Connection successful");

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
          handle_connection_loss();
          break;
        case SCTP_RESTART:
          state = "RESTART";
          break;
        case SCTP_SHUTDOWN_COMP:
          state = "SHUTDOWN_COMP";
          break;
        case SCTP_CANT_STR_ASSOC:
          state = "CAN'T START ASSOC";
          handle_connection_loss();
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
      handle_connection_loss();
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

void sctp_network_gateway_impl::handle_connection_loss()
{
  // Stop listening to new Rx events.
  io_sub.reset();

  // Notify the connection drop.
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

  if (not client_mode) {
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
      socket.fd().value(),
      [this]() { receive(); },
      [this](io_broker::error_code code) {
        logger.info("Connection loss due to IO error code={}.", (int)code);
        handle_connection_loss();
      });
  return io_sub.registered();
}
