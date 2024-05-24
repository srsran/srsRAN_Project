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

#include "sctp_network_gateway_common_impl.h"
#include "srsran/gateways/sctp_network_client.h"
#include "srsran/support/io/transport_layer_address.h"
#include <mutex>

struct sctp_sndrcvinfo;

namespace srsran {

class sctp_network_client_impl : public sctp_network_client, public sctp_network_gateway_common_impl
{
  explicit sctp_network_client_impl(const std::string&                 client_name,
                                    const sctp_network_gateway_config& sctp_cfg,
                                    io_broker&                         broker);

public:
  ~sctp_network_client_impl() override;

  /// Create an SCTP client.
  static std::unique_ptr<sctp_network_client>
  create(const std::string& client_name, const sctp_network_gateway_config& sctp_cfg, io_broker& broker);

  /// Connect to an SCTP server with the provided address.
  std::unique_ptr<sctp_association_sdu_notifier>
  connect_to(const std::string&                             connection_name,
             const std::string&                             dest_addr,
             int                                            dest_port,
             std::unique_ptr<sctp_association_sdu_notifier> recv_handler) override;

  int get_socket_fd() const override { return socket.fd().value(); }

private:
  class sctp_send_notifier;

  void receive();

  void handle_data(span<const uint8_t> payload);
  void handle_notification(span<const uint8_t>           payload,
                           const struct sctp_sndrcvinfo& sri,
                           const sockaddr&               src_addr,
                           socklen_t                     src_addr_len);
  void handle_connection_close(const char* cause);
  void handle_sctp_shutdown_comp();

  const std::string client_name;
  io_broker&        broker;

  // Temporary buffer where read data is saved.
  std::vector<uint8_t> temp_recv_buffer;

  std::unique_ptr<sctp_association_sdu_notifier> recv_handler;

  // Shared state between client a notifier.
  std::shared_ptr<std::atomic<bool>> shutdown_received;

  std::mutex shutdown_mutex;
};

} // namespace srsran