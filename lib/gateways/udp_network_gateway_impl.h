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

#pragma once

#include "srsran/adt/batched_dispatch_queue.h"
#include "srsran/gateways/udp_network_gateway.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/io/unique_fd.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace srsran {

constexpr uint32_t network_gateway_udp_max_len = 9100;

struct udp_tx_pdu_t {
  byte_buffer      pdu;
  sockaddr_storage dst_addr;
};

/// Transmit context used by the sendmmsg syscall.
struct transmit_context {
  explicit transmit_context(unsigned tx_max_mmsg, unsigned tx_max_segments)
  {
    mmsg.resize(tx_max_mmsg);
    msgs.resize(tx_max_mmsg);
    for (unsigned i = 0; i < tx_max_mmsg; i++) {
      msgs[i].resize(tx_max_segments);
    }
  }

  std::vector<::mmsghdr>            mmsg;
  std::vector<std::vector<::iovec>> msgs;
};

class udp_network_gateway_impl final : public udp_network_gateway
{
public:
  explicit udp_network_gateway_impl(udp_network_gateway_config                   config_,
                                    network_gateway_data_notifier_with_src_addr& data_notifier_,
                                    task_executor&                               io_tx_executor_,
                                    task_executor&                               io_rx_executor_);
  ~udp_network_gateway_impl() override { io_subcriber.reset(); }

  bool subscribe_to(io_broker& broker) override;

private:
  bool set_sockopts();

  // udp_network_gateway_data_handler interface, called from CU-UP executor.
  void handle_pdu(byte_buffer pdu, const sockaddr_storage& dest_addr) override;

  // Actual PDU handling, shall run in IO executor.
  void handle_pdu_impl(span<udp_tx_pdu_t> pdus);

  // Handle error detected by io_broker that led to the io deregistration.
  void handle_io_error(io_broker::error_code code);

  // udp_network_gateway_controller interface
  bool                    create_and_bind() override;
  void                    receive() override;
  int                     get_socket_fd() override;
  std::optional<uint16_t> get_bind_port() const override;
  bool                    get_bind_address(std::string& ip_address) const override;

  // socket helpers
  bool set_non_blocking();
  bool set_receive_timeout(unsigned rx_timeout_sec);
  bool set_reuse_addr();
  bool set_dscp();
  bool close_socket();

  udp_network_gateway_config                   config; // configuration
  network_gateway_data_notifier_with_src_addr& data_notifier;
  srslog::basic_logger&                        logger;
  task_executor&                               io_tx_executor;
  task_executor&                               io_rx_executor;

  unique_fd             sock_fd;
  io_broker::subscriber io_subcriber;

  transmit_context                     tx_ctx;
  batched_dispatch_queue<udp_tx_pdu_t> batched_queue;

  sockaddr_storage local_addr        = {}; // the local address
  socklen_t        local_addrlen     = 0;
  int              local_ai_family   = 0;
  int              local_ai_socktype = 0;
  int              local_ai_protocol = 0;

  // Helper boolean to avoid spamming the logs in case of buffer pool depletion
  bool warn_low_buffer_pool = true;
};

} // namespace srsran
