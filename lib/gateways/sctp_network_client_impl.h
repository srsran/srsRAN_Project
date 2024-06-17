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

#pragma once

#include "sctp_network_gateway_common_impl.h"
#include "srsran/gateways/sctp_network_client.h"
#include "srsran/support/io/transport_layer_address.h"
#include <condition_variable>
#include <mutex>

struct sctp_sndrcvinfo;

namespace srsran {

/// \brief SCTP client implementation
///
/// This implementation assumes single-threaded access to its public interface.
class sctp_network_client_impl : public sctp_network_client, public sctp_network_gateway_common_impl
{
  explicit sctp_network_client_impl(const sctp_network_gateway_config& sctp_cfg, io_broker& broker);

public:
  ~sctp_network_client_impl() override;

  /// Create an SCTP client.
  static std::unique_ptr<sctp_network_client> create(const sctp_network_gateway_config& sctp_cfg, io_broker& broker);

  /// Connect to an SCTP server with the provided address.
  std::unique_ptr<sctp_association_sdu_notifier>
  connect_to(const std::string&                             dest_name,
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

  io_broker& broker;

  // Temporary buffer where read data is saved.
  std::vector<uint8_t> temp_recv_buffer;

  // Handler of IO events. It is only accessed by the backend (io_broker), once the connection is set up.
  std::unique_ptr<sctp_association_sdu_notifier> recv_handler;

  // The value of std::atomic<bool> is shared between client and sender notifier.
  // The value of the shared_ptr is shared between client frontend (public interface) and backend (io_broker), and
  // needs to be mutexed on creation/reset.
  std::shared_ptr<std::atomic<bool>> shutdown_received;

  // shared between client frontend (public interface) and backend (io_broker) and needs to be mutexed on read/write.
  transport_layer_address server_addr;

  std::mutex              connection_mutex;
  std::condition_variable connection_cvar;
};

} // namespace srsran
