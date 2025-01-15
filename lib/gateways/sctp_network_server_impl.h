/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "sctp_network_gateway_common_impl.h"
#include "srsran/gateways/sctp_network_server.h"
#include <unordered_map>

struct sctp_sndrcvinfo;
struct sctp_assoc_change;

namespace srsran {

/// Implements an SCTP server, capable of handling multiple SCTP associations.
///
/// The server handles Rx data and SCTP association updates in the same thread of the io_broker.
class sctp_network_server_impl : public sctp_network_server, public sctp_network_gateway_common_impl
{
  explicit sctp_network_server_impl(const sctp_network_gateway_config& sctp_cfg,
                                    io_broker&                         broker,
                                    task_executor&                     io_rx_executor,
                                    sctp_network_association_factory&  assoc_factory);

public:
  ~sctp_network_server_impl() override;

  static std::unique_ptr<sctp_network_server> create(const sctp_network_gateway_config& sctp_cfg,
                                                     io_broker&                         broker,
                                                     task_executor&                     io_rx_executor,
                                                     sctp_network_association_factory&  assoc_factory);

  int get_socket_fd() const override { return socket.fd().value(); }

  void receive();

  bool listen() override;

  std::optional<uint16_t> get_listen_port() override;

private:
  class sctp_send_notifier;

  struct sctp_associaton_context {
    const int                          assoc_id;
    transport_layer_address            addr;
    std::shared_ptr<std::atomic<bool>> association_shutdown_received;

    std::unique_ptr<sctp_association_sdu_notifier> sctp_data_recv_notifier;

    sctp_associaton_context(int assoc_id);
  };

  // We use unique_ptr to maintain address stability.
  using association_map      = std::unordered_map<int, sctp_associaton_context>;
  using association_iterator = association_map::iterator;

  // Create a bind SCTP socket.
  bool create_and_bind();

  // Subscribe to IO broker to listen for incoming SCTP messages/events.
  bool subscribe_to_broker();

  void handle_socket_shutdown(const char* cause);

  void handle_data(int assoc_id, span<const uint8_t> payload);
  void handle_notification(span<const uint8_t>           payload,
                           const struct sctp_sndrcvinfo& sri,
                           const sockaddr&               src_addr,
                           socklen_t                     src_addr_len);
  void handle_association_shutdown(int assoc_id, const char* cause);
  void handle_sctp_shutdown_comp(int assoc_id);

  /// Handle SCTP COMM UP event.
  void
  handle_sctp_comm_up(const struct sctp_assoc_change& assoc_change, const sockaddr& src_addr, socklen_t src_addr_len);

  io_broker&                        broker;
  task_executor&                    io_rx_executor;
  sctp_network_association_factory& assoc_factory;

  association_map associations;
};

} // namespace srsran
