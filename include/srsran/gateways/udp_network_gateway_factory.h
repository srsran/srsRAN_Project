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

#include "srsran/gateways/udp_network_gateway.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>
#include <utility>

namespace srsran {

struct udp_network_gateway_creation_message {
  udp_network_gateway_creation_message(udp_network_gateway_config                   config_,
                                       network_gateway_data_notifier_with_src_addr& data_notifier_,
                                       task_executor&                               io_tx_executor_,
                                       task_executor&                               io_rx_executor_) :
    config(std::move(config_)),
    data_notifier(data_notifier_),
    io_tx_executor(io_tx_executor_),
    io_rx_executor(io_rx_executor_)
  {
  }
  udp_network_gateway_config                   config;
  network_gateway_data_notifier_with_src_addr& data_notifier;
  task_executor&                               io_tx_executor;
  task_executor&                               io_rx_executor;
};

/// Creates an instance of an network gateway
std::unique_ptr<udp_network_gateway> create_udp_network_gateway(udp_network_gateway_creation_message msg);

} // namespace srsran
