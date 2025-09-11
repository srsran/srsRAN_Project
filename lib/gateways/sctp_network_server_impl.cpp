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

#include "sctp_network_server_impl.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/io/sockets.h"
#include <netinet/sctp.h>

using namespace srsran;

/// Stream number to use for sending.
static constexpr unsigned stream_no = 0;

class sctp_network_server_impl::sctp_send_notifier : public sctp_association_sdu_notifier
{
public:
  sctp_send_notifier(sctp_network_server_impl&                                parent,
                     const sctp_network_server_impl::sctp_associaton_context& assoc,
                     srslog::basic_logger&                                    logger_) :
    ppid(parent.node_cfg.ppid),
    fd(parent.socket.fd().value()),
    if_name(parent.node_cfg.if_name),
    assoc_id(assoc.assoc_id),
    client_addr(assoc.addr),
    assoc_shutdown_flag(assoc.association_shutdown_received),
    logger(logger_)
  {
  }

  ~sctp_send_notifier() override { close(); }

  bool on_new_sdu(byte_buffer sdu) override
  {
    if (assoc_shutdown_flag->load(std::memory_order_relaxed)) {
      // It has already been released.
      return false;
    }
    if (sdu.length() > network_gateway_sctp_max_len) {
      logger.error("{}: PDU of {} bytes exceeds maximum length of {} bytes",
                   if_name,
                   sdu.length(),
                   network_gateway_sctp_max_len);
      return false;
    }
    logger.debug("{} assoc={}: Sending PDU of {} bytes", if_name, assoc_id, sdu.length());

    span<const uint8_t> pdu_span = to_span(sdu, send_buffer);

    transport_layer_address::native_type dest_addr  = client_addr.native();
    int                                  bytes_sent = ::sctp_sendmsg(fd,
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
      logger.error("{} assoc={}: Closing SCTP association. Cause: Couldn't send {} B of data. errno={}",
                   if_name,
                   assoc_id,
                   pdu_span.size_bytes(),
                   ::strerror(errno));
      close();
      return false;
    }
    return true;
  }

private:
  void close()
  {
    if (assoc_shutdown_flag->load(std::memory_order_relaxed)) {
      // Already closed.
      return;
    }

    // Send EOF to SCTP client.
    transport_layer_address::native_type dest_addr  = client_addr.native();
    int                                  bytes_sent = ::sctp_sendmsg(fd,
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
      logger.info(
          "{} assoc={}: Couldn't send EOF during shut down (errno=\"{}\")", if_name, assoc_id, ::strerror(errno));
    } else {
      logger.debug("{} assoc={}: Sent EOF to SCTP client and closed SCTP association", if_name, assoc_id);
    }

    // Signal sender closed the channel.
    assoc_shutdown_flag->store(true, std::memory_order_relaxed);
  }

  // Note: We copy all the required params by value to avoid race conditions with the server thread.
  const uint32_t                ppid;
  const int                     fd;
  std::string                   if_name;
  const int                     assoc_id;
  const transport_layer_address client_addr;
  // This flag is shared by the server main class and this notifier and is used to signal the association shut down.
  // Note: shared_ptr copy used to avoid the case when the notifier outlives the association.
  std::shared_ptr<std::atomic<bool>> assoc_shutdown_flag;
  srslog::basic_logger&              logger;

  // Buffer used to store data to send to client.
  std::array<uint8_t, network_gateway_sctp_max_len> send_buffer;
};

sctp_network_server_impl::sctp_associaton_context::sctp_associaton_context(int assoc_id_) : assoc_id(assoc_id_) {}

sctp_network_server_impl::sctp_network_server_impl(const srsran::sctp_network_gateway_config& sctp_cfg_,
                                                   io_broker&                                 broker_,
                                                   task_executor&                             io_rx_executor_,
                                                   sctp_network_association_factory&          assoc_factory_) :
  sctp_network_gateway_common_impl(sctp_cfg_),
  broker(broker_),
  io_rx_executor(io_rx_executor_),
  assoc_factory(assoc_factory_)
{
}

sctp_network_server_impl::~sctp_network_server_impl()
{
  // Stop handling new SCTP events.
  io_sub.reset();

  // Once the IO unsubscription completes, it is safe to remove associations.
  handle_socket_shutdown(nullptr);
}

bool sctp_network_server_impl::create_and_bind()
{
  return this->create_and_bind_common();
}

void sctp_network_server_impl::receive()
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
      logger.error("Error reading from SCTP socket: {}", ::strerror(errno));
      handle_socket_shutdown(nullptr);
    } else {
      if (!node_cfg.non_blocking_mode) {
        logger.debug("Socket timeout reached");
      }
    }
    return;
  }

  span<const uint8_t> payload(temp_recv_buffer.data(), rx_bytes);
  if (msg_flags & MSG_NOTIFICATION) {
    handle_notification(payload, sri, (const sockaddr&)msg_src_addr, msg_src_addrlen);
  } else {
    handle_data(sri.sinfo_assoc_id, payload);
  }
}

void sctp_network_server_impl::handle_socket_shutdown(const char* cause)
{
  // Signal to senders to all existing senders that the server was deleted, so they don't bother with sending EOF.
  // Note: This function is called from within the io_broker. So, it safe to delete the associations while the io_broker
  // subscription is still active.
  while (not associations.empty()) {
    handle_association_shutdown(associations.begin()->first, cause);
    handle_sctp_shutdown_comp(associations.begin()->first);
  }

  // Stop handling new SCTP events.
  io_sub.reset();
}

void sctp_network_server_impl::handle_data(int assoc_id, span<const uint8_t> payload)
{
  auto assoc_it = associations.find(assoc_id);
  if (assoc_it == associations.end()) {
    logger.error("{} assoc={}: Received data on unknown SCTP association", node_cfg.if_name, assoc_id);
    return;
  }

  logger.debug("{} assoc={}: Received {} bytes", node_cfg.if_name, assoc_id, payload.size());

  // Note: For SCTP, we avoid byte buffer allocation failures by resorting to fallback allocation.
  assoc_it->second.sctp_data_recv_notifier->on_new_sdu(byte_buffer{byte_buffer::fallback_allocation_tag{}, payload});
}

void sctp_network_server_impl::handle_notification(span<const uint8_t>           payload,
                                                   const struct sctp_sndrcvinfo& sri,
                                                   const sockaddr&               src_addr,
                                                   socklen_t                     src_addr_len)
{
  if (not validate_and_log_sctp_notification(payload)) {
    // Handle error.
    handle_association_shutdown(sri.sinfo_assoc_id, "The received message is invalid");
    return;
  }

  const auto* notif = reinterpret_cast<const union sctp_notification*>(payload.data());
  switch (notif->sn_header.sn_type) {
    case SCTP_ASSOC_CHANGE: {
      const struct sctp_assoc_change* n = &notif->sn_assoc_change;
      switch (n->sac_state) {
        case SCTP_COMM_UP:
          handle_sctp_comm_up(*n, src_addr, src_addr_len);
          break;
        case SCTP_COMM_LOST:
          handle_association_shutdown(n->sac_assoc_id, "Communication was lost");
          break;
        case SCTP_CANT_STR_ASSOC:
          handle_association_shutdown(n->sac_assoc_id, "Can't state association");
          break;
        case SCTP_SHUTDOWN_COMP:
          handle_sctp_shutdown_comp(n->sac_assoc_id);
          break;
        default:
          break;
      }
      break;
    }
    case SCTP_SHUTDOWN_EVENT: {
      const struct sctp_shutdown_event* n = &notif->sn_shutdown_event;
      handle_association_shutdown(n->sse_assoc_id, "Client requested the shutdown");
      break;
    }
    default:
      break;
  }
}

void sctp_network_server_impl::handle_sctp_comm_up(const struct sctp_assoc_change& assoc_change,
                                                   const sockaddr&                 src_addr,
                                                   socklen_t                       src_addr_len)
{
  int  assoc_id = assoc_change.sac_assoc_id;
  auto it       = associations.find(assoc_id);
  if (it != associations.end()) {
    logger.warning("{} assoc={}: SCTP COMM UP received but association already existed", node_cfg.if_name, assoc_id);
    return;
  }

  // Add an entry for the association in the lookup
  auto result = associations.emplace(assoc_id, assoc_id);
  if (not result.second) {
    logger.error("{} assoc={}: Unable to create new SCTP association", node_cfg.if_name, assoc_id);
    return;
  }

  // Fill the association context.
  sctp_associaton_context& assoc_ctxt      = result.first->second;
  assoc_ctxt.addr                          = transport_layer_address::create_from_sockaddr(src_addr, src_addr_len);
  assoc_ctxt.association_shutdown_received = std::make_shared<std::atomic<bool>>(false);
  assoc_ctxt.sctp_data_recv_notifier =
      assoc_factory.create(std::make_unique<sctp_send_notifier>(*this, assoc_ctxt, logger));
  if (assoc_ctxt.sctp_data_recv_notifier == nullptr) {
    associations.erase(assoc_id);
    logger.error("{} assoc={} client={}: Unable to create a new SCTP association handler",
                 node_cfg.if_name,
                 assoc_id,
                 assoc_ctxt.addr);
    return;
  }

  logger.info("{} assoc={}: New client SCTP association (client_addr={})", node_cfg.if_name, assoc_id, assoc_ctxt.addr);
}

void sctp_network_server_impl::handle_association_shutdown(int assoc_id, const char* cause)
{
  auto assoc_it = associations.find(assoc_id);
  if (assoc_it == associations.end()) {
    logger.error("{} assoc={}: Failed to shutdown SCTP association. Cause: SCTP association Id not found",
                 node_cfg.if_name,
                 assoc_id);
    return;
  }

  // The business domain or the peer side wishes to close the association.
  // Signal that the business domain should stop sending new SCTP data (including the EOF, which would fail anyway).
  bool prev = assoc_it->second.association_shutdown_received->exchange(true);
  if (not prev and cause != nullptr) {
    // The association sender didn't yet close the connection and the association was closed by the peer side.
    logger.info("{} assoc={}: SCTP association was shut down (client_addr={}). Cause: {}",
                node_cfg.if_name,
                assoc_it->first,
                assoc_it->second.addr,
                cause);
  }
}

void sctp_network_server_impl::handle_sctp_shutdown_comp(int assoc_id)
{
  auto assoc_it = associations.find(assoc_id);
  if (assoc_it == associations.end()) {
    logger.error("{} assoc={}: Failed to shutdown SCTP association. Cause: SCTP association Id not found",
                 node_cfg.if_name,
                 assoc_id);
    return;
  }

  // Remove association.
  // Note: Deleting the recv notifier should trigger the deletion of the sender interface.
  associations.erase(assoc_it);
}

bool sctp_network_server_impl::listen()
{
  if (node_cfg.bind_address.empty()) {
    logger.error("{}: Cannot listen to new SCTP associations if an address to bind to is not provided",
                 node_cfg.if_name);
    return false;
  }

  if (not socket.listen()) {
    return false;
  }

  if (not subscribe_to_broker()) {
    return false;
  }

  return true;
}

std::optional<uint16_t> sctp_network_server_impl::get_listen_port()
{
  return socket.get_listen_port();
}

bool sctp_network_server_impl::subscribe_to_broker()
{
  socket.release();
  io_sub = broker.register_fd(
      unique_fd(socket.fd().value()),
      io_rx_executor,
      [this]() { receive(); },
      [this](io_broker::error_code code) {
        logger.info("Connection loss due to IO error code={}.", (int)code);
        handle_socket_shutdown(nullptr);
      });
  return io_sub.registered();
}

std::unique_ptr<sctp_network_server> sctp_network_server_impl::create(const sctp_network_gateway_config& sctp_cfg,
                                                                      io_broker&                         broker_,
                                                                      task_executor&                    io_rx_executor_,
                                                                      sctp_network_association_factory& assoc_factory_)
{
  // Validate arguments
  if (sctp_cfg.if_name.empty()) {
    srslog::fetch_basic_logger("SCTP-GW").error("Cannot create SCTP server. Cause: No name was provided");
    return nullptr;
  }
  if (sctp_cfg.bind_address.empty()) {
    srslog::fetch_basic_logger("SCTP-GW").error("{}: Cannot create SCTP server without bind address", sctp_cfg.if_name);
    return nullptr;
  }

  // Create a SCTP server instance.
  std::unique_ptr<sctp_network_server_impl> server{
      new sctp_network_server_impl(sctp_cfg, broker_, io_rx_executor_, assoc_factory_)};

  // Create a socket and bind it to the provided address.
  if (not server->create_and_bind()) {
    return nullptr;
  }

  return server;
}
