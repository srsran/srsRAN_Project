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

#include "sctp_network_gateway_impl.h"
#include "srsran/gateways/addr_info.h"
#include "srsran/support/io/sockets.h"
#include <netdb.h>
#include <netinet/sctp.h>
#include <sys/socket.h>
#include <utility>

using namespace srsran;

sctp_network_gateway_impl::sctp_network_gateway_impl(const sctp_network_connector_config&   config_,
                                                     sctp_network_gateway_control_notifier& ctrl_notfier_,
                                                     network_gateway_data_notifier&         data_notifier_) :
  sctp_network_gateway_common_impl(config_),
  config(config_),
  ctrl_notifier(ctrl_notfier_),
  data_notifier(data_notifier_)
{
}

bool sctp_network_gateway_impl::create_and_bind()
{
  return this->create_and_bind_common();
}

bool sctp_network_gateway_impl::listen()
{
  return socket.listen();
}

std::optional<uint16_t> sctp_network_gateway_impl::get_listen_port()
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

  fmt::print("Connecting to {} on {}:{}\n", config.dest_name, config.connect_address.c_str(), config.connect_port);

  sockaddr_searcher                                  searcher{config.connect_address, config.connect_port, logger};
  std::chrono::time_point<std::chrono::steady_clock> start  = std::chrono::steady_clock::now();
  struct addrinfo*                                   result = nullptr;
  for (result = searcher.next(); result != nullptr; result = searcher.next()) {
    // Create SCTP socket only if not created in create_and_bind function.
    if (not socket.is_open()) {
      expected<sctp_socket> outcome = create_socket(result->ai_family, result->ai_socktype);
      if (not outcome.has_value()) {
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
  // Notify the connection drop.
  ctrl_notifier.on_connection_loss();

  // Stop listening to new Rx events.
  io_sub.reset();
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
