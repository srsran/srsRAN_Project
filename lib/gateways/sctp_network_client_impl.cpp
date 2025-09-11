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

#include "sctp_network_client_impl.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/io/sockets.h"
#include <netinet/sctp.h>

using namespace srsran;

/// Stream number to use for sending.
static constexpr unsigned stream_no = 0;

class sctp_network_client_impl::sctp_send_notifier final : public sctp_association_sdu_notifier
{
public:
  sctp_send_notifier(sctp_network_client_impl& parent_, const transport_layer_address& server_addr_) :
    client_name(parent_.node_cfg.if_name),
    ppid(parent_.node_cfg.ppid),
    fd(parent_.socket.fd().value()),
    logger(parent_.logger),
    server_addr(server_addr_),
    closed_flag(parent_.shutdown_received)
  {
  }
  ~sctp_send_notifier() override { close(); }

  bool on_new_sdu(byte_buffer sdu) override
  {
    if (closed_flag->load(std::memory_order_relaxed)) {
      // Already closed.
      return false;
    }

    if (sdu.length() > network_gateway_sctp_max_len) {
      logger.error("PDU of {} bytes exceeds maximum length of {} bytes", sdu.length(), network_gateway_sctp_max_len);
      return false;
    }
    logger.debug("{}: Sending PDU of {} bytes", client_name, sdu.length());

    // Note: each sender needs its own buffer to avoid race conditions with the recv.
    span<const uint8_t> pdu_span = to_span(sdu, send_buffer);

    auto dest_addr  = server_addr.native();
    int  bytes_sent = ::sctp_sendmsg(fd,
                                    pdu_span.data(),
                                    pdu_span.size(),
                                    const_cast<struct sockaddr*>(dest_addr.addr),
                                    dest_addr.addrlen,
                                    htonl(ppid),
                                    0,
                                    stream_no,
                                    0,
                                    0);
    if (bytes_sent == -1) {
      logger.error("{}: Closing SCTP association. Cause: Couldn't send {} B of data. errno={}",
                   client_name,
                   pdu_span.size_bytes(),
                   ::strerror(errno));
      close();
      return false;
    }
    return true;
  }

private:
  // Closes the SCTP association, sending an EOF, when required.
  void close()
  {
    if (closed_flag->load(std::memory_order_relaxed)) {
      // Already closed.
      return;
    }

    // Send EOF to SCTP server.
    auto dest_addr  = server_addr.native();
    int  bytes_sent = ::sctp_sendmsg(fd,
                                    nullptr,
                                    0,
                                    const_cast<struct sockaddr*>(dest_addr.addr),
                                    dest_addr.addrlen,
                                    htonl(ppid),
                                    SCTP_EOF,
                                    stream_no,
                                    0,
                                    0);

    if (bytes_sent == -1) {
      // Failed to send EOF.
      // Note: It may happen when the sender notifier is removed just before the SCTP shutdown event is handled in
      // the server recv thread.
      logger.info("{}: Couldn't send EOF during shut down (errno=\"{}\")", client_name, ::strerror(errno));
    } else {
      logger.debug("{}: Sent EOF to SCTP client and closed SCTP association", client_name);
    }

    // Signal sender closed the channel.
    closed_flag->store(true, std::memory_order_relaxed);
  }

  const std::string             client_name;
  const uint32_t                ppid;
  int                           fd;
  srslog::basic_logger&         logger;
  const transport_layer_address server_addr;

  std::array<uint8_t, network_gateway_sctp_max_len> send_buffer;

  std::shared_ptr<std::atomic<bool>> closed_flag;
};

sctp_network_client_impl::sctp_network_client_impl(const sctp_network_gateway_config& sctp_cfg,
                                                   io_broker&                         broker_,
                                                   task_executor&                     io_rx_executor_) :
  sctp_network_gateway_common_impl(sctp_cfg), broker(broker_), io_rx_executor(io_rx_executor_)
{
}

sctp_network_client_impl::~sctp_network_client_impl()
{
  logger.debug("{}: Closing...", node_cfg.if_name);

  // If there is a connection on-going.
  bool                    eof_needed = false;
  transport_layer_address server_addr_cpy;
  {
    std::lock_guard<std::mutex> lock(connection_mutex);
    // If there is no connection, return right away.
    if (server_addr.empty()) {
      return;
    }
    eof_needed      = not shutdown_received->exchange(true);
    server_addr_cpy = server_addr;
  }

  // Signal that the upper layer sender should stop sending new SCTP data (including the EOF).
  if (eof_needed) {
    ::sctp_sendmsg(socket.fd().value(),
                   nullptr,
                   0,
                   const_cast<struct sockaddr*>(server_addr_cpy.native().addr),
                   server_addr_cpy.native().addrlen,
                   htonl(node_cfg.ppid),
                   SCTP_EOF,
                   stream_no,
                   0,
                   0);
  }

  // No subscription is on-going. It is now safe to close the socket.
  std::unique_lock<std::mutex> lock(connection_mutex);
  connection_cvar.wait(lock, [this]() { return server_addr.empty(); });
}

std::unique_ptr<sctp_association_sdu_notifier>
sctp_network_client_impl::connect_to(const std::string&                             dest_name,
                                     const std::string&                             dest_addr,
                                     int                                            dest_port,
                                     std::unique_ptr<sctp_association_sdu_notifier> recv_handler_)
{
  {
    std::lock_guard<std::mutex> lock(connection_mutex);
    if (not server_addr.empty()) {
      // If this is not the first connection.
      logger.error("{}: Connection to {}:{} failed. Cause: Connection is already in progress",
                   node_cfg.if_name,
                   dest_addr,
                   dest_port);
      return nullptr;
    }
  }

  // If a bind address is provided, create a socket here and bind it.
  if (not node_cfg.bind_address.empty()) {
    if (not create_and_bind_common()) {
      return nullptr;
    }
  }

  sockaddr_searcher searcher{dest_addr, dest_port, logger};
  auto              start = std::chrono::steady_clock::now();
  // Create SCTP socket only if not created earlier during bind. Otherwise, reuse socket.
  bool             reuse_socket = socket.is_open();
  struct addrinfo* result       = nullptr;
  for (auto candidate = searcher.next(); candidate != nullptr and result == nullptr; candidate = searcher.next()) {
    if (not reuse_socket) {
      // Create SCTP socket only if not created earlier through bind or another connection.
      expected<sctp_socket> outcome = create_socket(candidate->ai_family, candidate->ai_socktype);
      if (not outcome.has_value()) {
        if (errno == ESOCKTNOSUPPORT) {
          // Stop the search.
          break;
        }
        continue;
      }
      socket = std::move(outcome.value());
    }

    bool connection_success = socket.connect(*candidate->ai_addr, candidate->ai_addrlen);
    if (not connection_success) {
      // connection failed, but before trying the next address, make sure the just created socket is deleted.
      if (not reuse_socket) {
        socket.close();
      }
      continue;
    }

    // Found a valid candidate.
    result = candidate;
  }

  if (result == nullptr) {
    auto        end    = std::chrono::steady_clock::now();
    auto        now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::string cause  = ::strerror(errno);
    if (errno == 0) {
      cause = "IO broker could not register socket";
    }
    fmt::print("{}: Failed to connect to {} on {}:{}. error=\"{}\" timeout={}ms\n",
               node_cfg.if_name,
               dest_name,
               dest_addr,
               dest_port,
               cause,
               now_ms.count());
    logger.error("{}: Failed to connect to {} on {}:{}. error=\"{}\" timeout={}ms",
                 node_cfg.if_name,
                 dest_name,
                 dest_addr,
                 dest_port,
                 cause,
                 now_ms.count());
    return nullptr;
  }

  // Create objects representing state of the client.
  recv_handler = std::move(recv_handler_);
  auto addr    = transport_layer_address::create_from_sockaddr(*result->ai_addr, result->ai_addrlen);
  {
    // Save server address and create a shutdown context flag.
    std::unique_lock<std::mutex> lock(connection_mutex);
    server_addr       = addr;
    shutdown_received = std::make_shared<std::atomic<bool>>(false);
  }

  // Register the socket in the IO broker.
  socket.release();
  io_sub = broker.register_fd(
      unique_fd(socket.fd().value()),
      io_rx_executor,
      [this]() { receive(); },
      [this](io_broker::error_code code) {
        std::string cause = fmt::format("IO error code={}", (int)code);
        handle_connection_terminated(cause);
      });
  if (not io_sub.registered()) {
    // IO subscription failed.
    {
      std::unique_lock<std::mutex> lock(connection_mutex);
      server_addr = {};
      shutdown_received.reset();
    }
    if (not reuse_socket) {
      socket.close();
    }
    recv_handler.reset();
    return nullptr;
  }

  logger.info("{}: SCTP connection to {} on {}:{} was established", node_cfg.if_name, dest_name, dest_addr, dest_port);

  return std::make_unique<sctp_send_notifier>(*this, addr);
}

void sctp_network_client_impl::receive()
{
  struct sctp_sndrcvinfo                            sri       = {};
  int                                               msg_flags = 0;
  std::array<uint8_t, network_gateway_sctp_max_len> temp_recv_buffer;

  // fromlen is an in/out variable in sctp_recvmsg.
  sockaddr_storage msg_src_addr;
  socklen_t        msg_src_addrlen = sizeof(msg_src_addr);

  int rx_bytes = ::sctp_recvmsg(socket.fd().value(),
                                temp_recv_buffer.data(),
                                temp_recv_buffer.size(),
                                (struct sockaddr*)&msg_src_addr,
                                &msg_src_addrlen,
                                &sri,
                                &msg_flags);

  // Handle error.
  if (rx_bytes == -1) {
    if (errno != EAGAIN) {
      std::string cause = fmt::format("Error reading from SCTP socket: {}", ::strerror(errno));
      handle_connection_terminated(cause.c_str());
    } else {
      if (!node_cfg.non_blocking_mode) {
        logger.debug("{}: Socket timeout reached", node_cfg.if_name);
      }
    }
    return;
  }

  span<const uint8_t> payload(temp_recv_buffer.data(), rx_bytes);
  if (msg_flags & MSG_NOTIFICATION) {
    handle_notification(payload, sri, (const sockaddr&)msg_src_addr, msg_src_addrlen);
  } else {
    handle_data(payload);
  }
}

void sctp_network_client_impl::handle_connection_shutdown(const char* cause)
{
  // Signal that the upper layer sender should stop sending new SCTP data (including the EOF, which would fail anyway).
  bool prev = shutdown_received->exchange(true);

  if (not prev and cause != nullptr) {
    // The SCTP sender (the upper layers) didn't yet close the connection.
    logger.info("{}: SCTP connection was shut down. Cause: {}", node_cfg.if_name, cause);
  }
}

void sctp_network_client_impl::handle_connection_terminated(const std::string& cause)
{
  logger.info("{}: {}. Notifying connection drop to upper layers", node_cfg.if_name, cause);

  // Notify SCTP sender that there is no need to send EOF.
  shutdown_received->store(true);

  // Notify the connection drop to the upper layers.
  recv_handler.reset();

  // Unsubscribe from listening to new IO events.
  io_sub.reset();

  // Erase server_addr and notify dtor that connection is closed.
  std::unique_lock<std::mutex> lock(connection_mutex);
  server_addr       = {};
  shutdown_received = nullptr;
  connection_cvar.notify_one();
}

void sctp_network_client_impl::handle_data(span<const uint8_t> payload)
{
  logger.debug("{}: Received {} bytes", node_cfg.if_name, payload.size());

  // Note: For SCTP, we avoid byte buffer allocation failures by resorting to fallback allocation.
  recv_handler->on_new_sdu(byte_buffer{byte_buffer::fallback_allocation_tag{}, payload});
}

void sctp_network_client_impl::handle_notification(span<const uint8_t>           payload,
                                                   const struct sctp_sndrcvinfo& sri,
                                                   const sockaddr&               src_addr,
                                                   socklen_t                     src_addr_len)
{
  if (not validate_and_log_sctp_notification(payload)) {
    // Handle error.
    handle_connection_terminated("Received invalid message");
    return;
  }

  const auto* notif = reinterpret_cast<const union sctp_notification*>(payload.data());
  switch (notif->sn_header.sn_type) {
    case SCTP_ASSOC_CHANGE: {
      const struct sctp_assoc_change* n = &notif->sn_assoc_change;
      switch (n->sac_state) {
        case SCTP_COMM_UP:
          break;
        case SCTP_COMM_LOST:
          handle_connection_terminated("Communication to the server was lost");
          break;
        case SCTP_SHUTDOWN_COMP:
          handle_connection_terminated("Received SCTP shutdown completed");
          break;
        case SCTP_CANT_STR_ASSOC:
          handle_connection_terminated("Can't start association");
          break;
        default:
          break;
      }
      break;
    }
    case SCTP_SHUTDOWN_EVENT: {
      handle_connection_shutdown("Server closed SCTP association");
      break;
    }
    default:
      break;
  }
}

std::unique_ptr<sctp_network_client> sctp_network_client_impl::create(const sctp_network_gateway_config& sctp_cfg,
                                                                      io_broker&                         broker_,
                                                                      task_executor&                     io_rx_executor)
{
  // Validate arguments.
  if (sctp_cfg.if_name.empty()) {
    srslog::fetch_basic_logger("SCTP-GW").error("Cannot create SCTP client. Cause: No name was provided");
    return nullptr;
  }

  // Create a SCTP client instance.
  std::unique_ptr<sctp_network_client_impl> client{new sctp_network_client_impl(sctp_cfg, broker_, io_rx_executor)};
  return client;
}
