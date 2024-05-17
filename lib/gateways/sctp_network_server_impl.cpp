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

class sctp_network_server_impl::sctp_send_notifier : public network_gateway_data_notifier
{
public:
  sctp_send_notifier(sctp_network_server_impl&                                parent_,
                     const sctp_network_server_impl::sctp_associaton_context& assoc_) :
    parent(parent_), assoc(&assoc_)
  {
  }

  ~sctp_send_notifier() override
  {
    if (assoc != nullptr) {
      parent.close_sctp_sender(*assoc);
    }
  }

  void on_new_pdu(byte_buffer pdu) override
  {
    if (assoc != nullptr and not parent.send_sctp_data(*assoc, std::move(pdu))) {
      parent.close_sctp_sender(*assoc);
      assoc = nullptr;
    }
  }

private:
  sctp_network_server_impl&                                parent;
  const sctp_network_server_impl::sctp_associaton_context* assoc = nullptr;
};

sctp_network_server_impl::sctp_associaton_context::sctp_associaton_context(int                    assoc_id_,
                                                                           const struct sockaddr& saddr_,
                                                                           socklen_t              saddr_len_) :
  assoc_id(assoc_id_), addr(transport_layer_address::create_from_sockaddr(saddr_, saddr_len_))
{
}

sctp_network_server_impl::sctp_network_server_impl(const srsran::sctp_network_node_config& sctp_cfg_) :
  sctp_network_gateway_common_impl(sctp_cfg_), temp_buffer(network_gateway_sctp_max_len)
{
}

sctp_network_server_impl::~sctp_network_server_impl() {}

bool sctp_network_server_impl::create_and_bind()
{
  return this->common_create_and_bind();
}

void sctp_network_server_impl::receive()
{
  struct sctp_sndrcvinfo sri       = {};
  int                    msg_flags = 0;

  std::array<uint8_t, network_gateway_sctp_max_len> tmp_mem;

  int rx_bytes = ::sctp_recvmsg(socket.fd().value(),
                                tmp_mem.data(),
                                network_gateway_sctp_max_len,
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

  // Check if association already exists.
  auto assoc_it = associations.find(sri.sinfo_assoc_id);
  if (assoc_it == associations.end()) {
    assoc_it = handle_new_sctp_association(sri.sinfo_assoc_id);
    if (assoc_it == associations.end()) {
      logger.error(
          "fd={} assoc_id={}: Unable to create new association handler", socket.fd().value(), sri.sinfo_assoc_id);
      return;
    }
  }

  span<const uint8_t> payload(tmp_mem.data(), rx_bytes);
  if (msg_flags & MSG_NOTIFICATION) {
    handle_notification(assoc_it, payload);
  } else {
    handle_data(assoc_it, payload);
  }

  handle_pending_sender_close_commands();
}

std::unordered_map<int, sctp_network_server_impl::sctp_associaton_context>::iterator
sctp_network_server_impl::handle_new_sctp_association(int assoc_id)
{
  const socket_name_info addr = get_nameinfo((struct sockaddr&)msg_src_addr, msg_src_addrlen);

  logger.debug(
      "fd={} assoc_id={}: New SCTP association from {}:{}", socket.fd().value(), assoc_id, addr.address, addr.port);

  sctp_associaton_context context{assoc_id, (struct sockaddr&)msg_src_addr, msg_src_addrlen};
  context.sctp_data_recv_notifier = assoc_factory->create();
  if (context.sctp_data_recv_notifier == nullptr) {
    return associations.end();
  }

  auto ret = associations.insert(std::make_pair(assoc_id, std::move(context)));
  return ret.first;
}

void sctp_network_server_impl::handle_pending_sender_close_commands()
{
  // Note: This is called from the SCTP recv thread.

  // Check if any sender released its connection.
  int assoc_to_rem;
  while (pending_cmds.try_dequeue(assoc_to_rem)) {
    auto assoc_it = associations.find(assoc_to_rem);
    if (assoc_it == associations.end()) {
      logger.error("assoc={}: Removing nonexistent SCTP association", assoc_to_rem);
      continue;
    }

    // Mark that the sender has no handle for this association.
    assoc_it->second.sender_closed = true;

    handle_association_shutdown(assoc_it);
  }
}

void sctp_network_server_impl::handle_socket_error()
{
  // TODO: Notify associations and handle socket shutdown.
}

void sctp_network_server_impl::handle_data(association_iterator assoc_it, span<const uint8_t> payload)
{
  logger.debug(
      "fd={} assoc_id={}: Received {} bytes on SCTP socket", socket.fd().value(), assoc_it->first, payload.size());

  // Note: For SCTP, we avoid byte buffer allocation failures by resorting to fallback allocation.
  assoc_it->second.sctp_data_recv_notifier->on_new_pdu(byte_buffer{byte_buffer::fallback_allocation_tag{}, payload});
}

void sctp_network_server_impl::handle_notification(association_iterator assoc_it, span<const uint8_t> payload)
{
  logger.debug("fd={} assoc_id={}: Received notification on SCTP socket", socket.fd().value(), assoc_it->first);

  const auto* notif             = reinterpret_cast<const union sctp_notification*>(payload.data());
  uint32_t    notif_header_size = sizeof(notif->sn_header);
  if (notif_header_size > payload.size_bytes()) {
    logger.error("fd={}: Notification msg size ({} B) is smaller than notification header size ({} B)",
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
          state = "COMM UP";
          // TODO: Handle.
          break;
        case SCTP_COMM_LOST:
          state = "COMM_LOST";
          handle_association_shutdown(assoc_it);
          break;
        case SCTP_RESTART:
          state = "RESTART";
          break;
        case SCTP_SHUTDOWN_COMP:
          state = "SHUTDOWN_COMP";
          break;
        case SCTP_CANT_STR_ASSOC:
          state = "CAN'T START ASSOC";
          handle_association_shutdown(assoc_it);
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
      const struct sctp_shutdown_event* n = &notif->sn_shutdown_event;
      logger.debug("SCTP_SHUTDOWN_EVENT notif: assoc id: {}", n->sse_assoc_id);
      handle_association_shutdown(assoc_it);
      break;
    }

    default:
      logger.warning("Unhandled notification type {}", notif->sn_header.sn_type);
      break;
  }
}

void sctp_network_server_impl::handle_association_shutdown(association_iterator assoc_it)
{
  // Signal to the SCTP association Rx handler that the Rx channel is closed.
  assoc_it->second.sctp_data_recv_notifier.reset();

  if (assoc_it->second.sender_closed) {
    // Sender has already released its notifier. It is safe to delete the association instance.
    associations.erase(assoc_it);
  } else {
    logger.debug("fd={} assoc={}: SCTP association connection was lost", socket.fd().value(), assoc_it->first);
  }
}

void sctp_network_server_impl::close_sctp_sender(const sctp_associaton_context& assoc)
{
  // Note: This function may be called from a different thread than the one that calls sctp_recv.
  srsran_assert(socket.fd().is_open(), "Socket is closed");

  // Send EOF.
  auto dest_addr  = assoc.addr.native();
  int  bytes_sent = sctp_sendmsg(socket.fd().value(),
                                nullptr,
                                0,
                                const_cast<struct sockaddr*>(dest_addr.addr),
                                dest_addr.addrlen,
                                htonl(node_cfg.ppid),
                                SCTP_EOF,
                                stream_no,
                                0,
                                0);
  if (bytes_sent == -1) {
    logger.error(
        "fd={} assoc={}: Couldn't send EOF on SCTP socket: {}", socket.fd().value(), assoc.assoc_id, strerror(errno));
  } else {
    logger.debug("fd={} assoc={}: SCTP association Tx channel was closed", socket.fd().value(), assoc.assoc_id);
  }

  // Notify that the association can now be deleted.
  pending_cmds.enqueue(assoc.assoc_id);
}

bool sctp_network_server_impl::send_sctp_data(const sctp_associaton_context& assoc, byte_buffer pdu)
{
  // Note: This function may be called from a different thread than the one that calls sctp_recv.
  srsran_assert(socket.fd().is_open(), "Socket is closed");

  logger.debug("assoc={}: Sending PDU of {} bytes", assoc.assoc_id, pdu.length());

  if (pdu.length() > temp_buffer.size()) {
    logger.error("PDU of {} bytes exceeds maximum length of {} bytes", pdu.length(), temp_buffer.size());
    return false;
  }

  span<const uint8_t> pdu_span = to_span(pdu, temp_buffer);

  auto dest_addr  = assoc.addr.native();
  int  bytes_sent = sctp_sendmsg(socket.fd().value(),
                                pdu_span.data(),
                                pdu_span.size(),
                                const_cast<struct sockaddr*>(dest_addr.addr),
                                dest_addr.addrlen,
                                htonl(node_cfg.ppid),
                                0,
                                stream_no,
                                0,
                                0);
  if (bytes_sent == -1) {
    logger.error("assoc={}: Closing SCTP association. Cause: Couldn't send {} B of data on SCTP socket: {}",
                 assoc.assoc_id,
                 pdu_span.size_bytes(),
                 strerror(errno));
    return false;
  }
  return true;
}

bool sctp_network_server_impl::listen()
{
  return socket.listen();
}

optional<uint16_t> sctp_network_server_impl::get_listen_port()
{
  return socket.get_listen_port();
}

bool sctp_network_server_impl::subscribe_to(io_broker& broker)
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

void sctp_network_server_impl::attach_association_handler(sctp_network_association_factory& factory)
{
  assoc_factory = &factory;
}
