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

#include "srsran/gateways/udp_network_gateway.h"
#include "srsran/support/executors/task_executor.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace srsran {

constexpr uint32_t network_gateway_udp_max_len = 9100;

class udp_network_gateway_impl final : public udp_network_gateway
{
public:
  explicit udp_network_gateway_impl(udp_network_gateway_config                   config_,
                                    network_gateway_data_notifier_with_src_addr& data_notifier_,
                                    task_executor&                               io_executor_);
  ~udp_network_gateway_impl() override { close_socket(); }

private:
  bool is_initialized();
  bool set_sockopts();

  // udp_network_gateway_data_handler interface, called from CU-UP executor.
  void handle_pdu(byte_buffer pdu, const sockaddr_storage& dest_addr) override;

  // Actual PDU handling, shall run in IO executor.
  void handle_pdu_impl(const byte_buffer& pdu, const sockaddr_storage& dest_addr);

  // udp_network_gateway_controller interface
  bool create_and_bind() override;
  void receive() override;
  int  get_socket_fd() override;
  bool get_bind_port(uint16_t& port) override;
  bool get_bind_address(std::string& ip_address) override;

  // socket helpers
  bool set_non_blocking();
  bool set_receive_timeout(unsigned rx_timeout_sec);
  bool set_reuse_addr();
  bool close_socket();

  udp_network_gateway_config                   config; // configuration
  network_gateway_data_notifier_with_src_addr& data_notifier;
  srslog::basic_logger&                        logger;
  task_executor&                               io_tx_executor;

  int sock_fd = -1;

  sockaddr_storage local_addr        = {}; // the local address
  socklen_t        local_addrlen     = 0;
  int              local_ai_family   = 0;
  int              local_ai_socktype = 0;
  int              local_ai_protocol = 0;

  /// Temporary RX buffers for reception.
  std::vector<std::vector<uint8_t>> rx_mem;
  std::vector<::sockaddr_storage>   rx_srcaddr;
  std::vector<::mmsghdr>            rx_msghdr;
  std::vector<::iovec>              rx_iovecs;

  // Temporary Tx buffer for transmission.
  std::array<uint8_t, network_gateway_udp_max_len> tx_mem;
};

} // namespace srsran
