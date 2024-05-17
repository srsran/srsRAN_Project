/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cameron314/concurrentqueue.h"
#include "sctp_network_gateway_common_impl.h"
#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/support/io/transport_layer_address.h"
#include <unordered_map>

namespace srsran {

class sctp_network_server_impl : public sctp_network_server, public sctp_network_gateway_common_impl
{
public:
  explicit sctp_network_server_impl(const sctp_network_node_config& sctp_cfg);
  ~sctp_network_server_impl() override;

  bool create_and_bind() override;

  void receive() override;

  int get_socket_fd() override { return this->socket.fd().value(); }

  bool listen() override;

  optional<uint16_t> get_listen_port() override;

  bool subscribe_to(io_broker& broker) override;

  void attach_association_handler(sctp_network_association_factory& factory) override;

private:
  class sctp_send_notifier;

  struct sctp_associaton_context {
    const int               assoc_id;
    transport_layer_address addr;
    bool                    sender_closed = false;

    std::unique_ptr<network_gateway_data_notifier> sctp_data_recv_notifier;

    sctp_associaton_context(int assoc_id, const sockaddr& saddr, socklen_t saddr_len);
  };

  using association_iterator = std::unordered_map<int, sctp_associaton_context>::iterator;

  void handle_socket_error();

  std::unordered_map<int, sctp_associaton_context>::iterator handle_new_sctp_association(int assoc_id);
  void handle_data(association_iterator assoc_it, span<const uint8_t> payload);
  void handle_notification(association_iterator assoc_it, span<const uint8_t> payload);
  void handle_association_shutdown(association_iterator assoc_it);

  // Handle commands from the sender telling that it released its association.
  void handle_pending_sender_close_commands();

  bool send_sctp_data(const sctp_associaton_context& assoc, byte_buffer pdu);
  void close_sctp_sender(const sctp_associaton_context& assoc);

  std::unordered_map<int, sctp_associaton_context> associations;

  // Queue used to communicate commands to the receiver execution context.
  moodycamel::ConcurrentQueue<int> pending_cmds;

  sctp_network_association_factory* assoc_factory = nullptr;

  std::vector<uint8_t> temp_buffer;
  sockaddr_storage     msg_src_addr    = {};
  socklen_t            msg_src_addrlen = 0;

  // the stream number to use for sending
  const unsigned stream_no = 0;
};

} // namespace srsran