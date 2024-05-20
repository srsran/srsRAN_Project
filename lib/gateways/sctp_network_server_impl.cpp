/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sctp_network_server_impl.h"
#include "srsran/support/io/sockets.h"
#include <netinet/sctp.h>

using namespace srsran;

// the stream number to use for sending
const unsigned stream_no = 0;

class sctp_network_server_impl::sctp_send_notifier : public sctp_association_pdu_notifier
{
public:
  sctp_send_notifier(sctp_network_server_impl&                                parent,
                     const sctp_network_server_impl::sctp_associaton_context& assoc,
                     srslog::basic_logger&                                    logger_) :
    ppid(parent.node_cfg.ppid),
    fd(parent.socket.fd().value()),
    assoc_id(assoc.assoc_id),
    client_addr(assoc.addr),
    assoc_shutdown_flag(assoc.association_shutdown_received),
    logger(logger_)
  {
  }

  ~sctp_send_notifier() override { close(); }

  bool on_new_pdu(byte_buffer pdu) override
  {
    if (assoc_shutdown_flag->load(std::memory_order_relaxed)) {
      // It has already been released.
      return false;
    }
    if (pdu.length() > network_gateway_sctp_max_len) {
      logger.error("PDU of {} bytes exceeds maximum length of {} bytes", pdu.length(), network_gateway_sctp_max_len);
      return false;
    }
    logger.debug("assoc={}: Sending PDU of {} bytes", assoc_id, pdu.length());

    // Note: each sender needs its own buffer to avoid race conditions with the recv.
    std::array<uint8_t, network_gateway_sctp_max_len> temp_send_buffer;
    span<const uint8_t>                               pdu_span = to_span(pdu, temp_send_buffer);

    auto dest_addr  = client_addr.native();
    int  bytes_sent = sctp_sendmsg(fd,
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
      logger.error("assoc={}: Closing SCTP association. Cause: Couldn't send {} B of data. errno={}",
                   assoc_id,
                   pdu_span.size_bytes(),
                   strerror(errno));
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
    auto dest_addr  = client_addr.native();
    int  bytes_sent = sctp_sendmsg(fd,
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
      logger.info("fd={} assoc={}: Couldn't send EOF during shut down (errno=\"{}\")", fd, assoc_id, strerror(errno));
    } else {
      logger.debug("fd={} assoc={}: Sent EOF to SCTP client and closed SCTP association", fd, assoc_id);
    }

    // Signal send closed the channel.
    assoc_shutdown_flag->store(true, std::memory_order_relaxed);
  }

  // Note: We copy all the required params by value to avoid race conditions with the server thread.
  const uint32_t                ppid;
  const int                     fd;
  const int                     assoc_id;
  const transport_layer_address client_addr;
  // This flag is shared by the server main class and this notifier and is used to signal the association shut down.
  // Note: shared_ptr copy used to avoid the case when the notifier outlives the association.
  std::shared_ptr<std::atomic<bool>> assoc_shutdown_flag;
  srslog::basic_logger&              logger;
};

sctp_network_server_impl::sctp_associaton_context::sctp_associaton_context(int assoc_id_) : assoc_id(assoc_id_) {}

sctp_network_server_impl::sctp_network_server_impl(const srsran::sctp_network_node_config& sctp_cfg_,
                                                   io_broker&                              broker_,
                                                   sctp_network_association_factory&       assoc_factory_) :
  sctp_network_gateway_common_impl(sctp_cfg_),
  broker(broker_),
  assoc_factory(assoc_factory_),
  temp_recv_buffer(network_gateway_sctp_max_len)
{
}

sctp_network_server_impl::~sctp_network_server_impl()
{
  // Stop handling new SCTP events.
  io_sub.reset();

  // Signal to senders to all existing senders that the server was deleted, so they don't bother with sending EOF.
  while (not associations.empty()) {
    handle_association_shutdown(associations.begin()->first, false);
  }

  logger.info("fd={}: SCTP server closed", socket.fd().value());
}

bool sctp_network_server_impl::create_and_bind()
{
  return this->common_create_and_bind();
}

void sctp_network_server_impl::receive()
{
  struct sctp_sndrcvinfo sri       = {};
  int                    msg_flags = 0;
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
      logger.error("Error reading from SCTP socket: {}", strerror(errno));
      handle_socket_error();
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

void sctp_network_server_impl::handle_socket_error()
{
  // TODO: Notify associations and handle socket shutdown.
}

void sctp_network_server_impl::handle_data(int assoc_id, span<const uint8_t> payload)
{
  auto assoc_it = associations.find(assoc_id);
  if (assoc_it == associations.end()) {
    logger.error("fd={} assoc={}: Received data on unknown SCTP association", socket.fd().value(), assoc_id);
    return;
  }

  logger.debug("fd={} assoc={}: Received {} bytes", socket.fd().value(), assoc_id, payload.size());

  // Note: For SCTP, we avoid byte buffer allocation failures by resorting to fallback allocation.
  assoc_it->second.sctp_data_recv_notifier->on_new_pdu(byte_buffer{byte_buffer::fallback_allocation_tag{}, payload});
}

void sctp_network_server_impl::handle_notification(span<const uint8_t>           payload,
                                                   const struct sctp_sndrcvinfo& sri,
                                                   const sockaddr&               src_addr,
                                                   socklen_t                     src_addr_len)
{
  const auto* notif             = reinterpret_cast<const union sctp_notification*>(payload.data());
  uint32_t    notif_header_size = sizeof(notif->sn_header);
  if (notif_header_size > payload.size_bytes()) {
    logger.error("fd={}: SCTP Notification msg size ({} B) is smaller than notification header size ({} B)",
                 socket.fd().value(),
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

      const char*                     state = NULL;
      const struct sctp_assoc_change* n     = &notif->sn_assoc_change;

      switch (n->sac_state) {
        case SCTP_COMM_UP:
          handle_sctp_comm_up(*n, src_addr, src_addr_len);
          state = "COMM UP";
          break;
        case SCTP_COMM_LOST:
          state = "COMM_LOST";
          handle_association_shutdown(n->sac_assoc_id);
          break;
        case SCTP_RESTART:
          state = "RESTART";
          break;
        case SCTP_SHUTDOWN_COMP:
          state = "SHUTDOWN_COMP";
          break;
        case SCTP_CANT_STR_ASSOC:
          state = "CAN'T START ASSOC";
          handle_association_shutdown(n->sac_assoc_id);
          break;
      }

      if (n->sac_state != SCTP_COMM_UP) {
        logger.debug(
            "SCTP_ASSOC_CHANGE notif: state: {}, error code: {}, out streams: {}, in streams: {}, assoc id: {}",
            state,
            n->sac_error,
            n->sac_outbound_streams,
            n->sac_inbound_streams,
            n->sac_assoc_id);
      }
      break;
    }

    case SCTP_SHUTDOWN_EVENT: {
      if (sizeof(struct sctp_shutdown_event) > payload.size_bytes()) {
        logger.error("Error notification msg size is smaller than struct sctp_shutdown_event size");
        return;
      }
      const struct sctp_shutdown_event* n = &notif->sn_shutdown_event;
      handle_association_shutdown(n->sse_assoc_id);
      break;
    }

    default:
      logger.warning("Unhandled notification type {}", notif->sn_header.sn_type);
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
    logger.warning(
        "fd={} assoc={}: SCTP COMM UP received but association already existed", socket.fd().value(), assoc_id);
    return;
  }

  // Add an entry for the association in the lookup
  auto result = associations.emplace(assoc_id, assoc_id);
  if (not result.second) {
    logger.error("fd={} assoc={}: Unable to create new SCTP association", socket.fd().value(), assoc_id);
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
    logger.error("fd={} assoc={} client={}: Unable to create a new SCTP association handler",
                 socket.fd().value(),
                 assoc_id,
                 assoc_ctxt.addr);
    return;
  }

  logger.info(
      "fd={} assoc={}: New client SCTP association (client_addr={})", socket.fd().value(), assoc_id, assoc_ctxt.addr);
}

void sctp_network_server_impl::handle_association_shutdown(int assoc_id, bool client_shutdown)
{
  auto assoc_it = associations.find(assoc_id);
  if (assoc_it == associations.end()) {
    logger.error("fd={} assoc={}: Failed to shutdown SCTP association. Cause: SCTP association Id not found",
                 socket.fd().value(),
                 assoc_id);
    return;
  }

  // The client wishes to close the association.
  // Signal that the upper layer sender should stop sending new SCTP data (including the EOF, which would fail anyway).
  bool prev = assoc_it->second.association_shutdown_received->exchange(true);
  if (not prev and client_shutdown) {
    // The association sender didn't yet close the connection and the association was closed by the client
    logger.info("fd={} assoc={}: Client shut down SCTP association (client_addr={})",
                socket.fd().value(),
                assoc_it->first,
                assoc_it->second.addr);
  }

  // Remove association.
  // Note: Deleting the recv notifier should trigger the deletion of the sender interface.
  associations.erase(assoc_it);
}

bool sctp_network_server_impl::listen()
{
  if (node_cfg.bind_address.empty()) {
    logger.error("Cannot listen to new SCTP associations if an address to bind to is not provided");
    return false;
  }

  if (not socket.listen()) {
    return false;
  }

  if (not subscribe_to_broker()) {
    return false;
  }

  if (logger.debug.enabled()) {
    logger.debug("fd={}: Listening for new SCTP associations on {}:{}...",
                 socket.fd().value(),
                 node_cfg.bind_address,
                 get_listen_port());
  }

  return true;
}

optional<uint16_t> sctp_network_server_impl::get_listen_port()
{
  return socket.get_listen_port();
}

bool sctp_network_server_impl::subscribe_to_broker()
{
  io_sub = broker.register_fd(
      socket.fd().value(),
      [this]() { receive(); },
      [this](io_broker::error_code code) {
        logger.info("Connection loss due to IO error code={}.", (int)code);
        handle_socket_error();
      });
  return io_sub.registered();
}

std::unique_ptr<sctp_network_server> sctp_network_server_impl::create(const sctp_network_node_config&   sctp_cfg,
                                                                      io_broker&                        broker_,
                                                                      sctp_network_association_factory& assoc_factory_)
{
  // Validate arguments
  if (sctp_cfg.bind_address.empty()) {
    srslog::fetch_basic_logger("SCTP-GW").error("Cannot create SCTP server without bind address");
    return nullptr;
  }

  // Create a SCTP server instance.
  std::unique_ptr<sctp_network_server_impl> server{new sctp_network_server_impl(sctp_cfg, broker_, assoc_factory_)};

  // Create a socket and bind it to the provided address.
  if (not server->create_and_bind()) {
    return nullptr;
  }

  return server;
}
