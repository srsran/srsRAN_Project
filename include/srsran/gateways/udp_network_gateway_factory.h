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

#include "udp_network_gateway.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>
#include <utility>

namespace srsran {

struct udp_network_gateway_creation_message {
  udp_network_gateway_creation_message(udp_network_gateway_config                   config_,
                                       network_gateway_data_notifier_with_src_addr& data_notifier_,
                                       task_executor&                               io_tx_executor_) :
    config(std::move(config_)), data_notifier(data_notifier_), io_tx_executor(io_tx_executor_)
  {
  }
  udp_network_gateway_config                   config;
  network_gateway_data_notifier_with_src_addr& data_notifier;
  task_executor&                               io_tx_executor;
};

/// Creates an instance of an network gateway
std::unique_ptr<udp_network_gateway> create_udp_network_gateway(udp_network_gateway_creation_message msg);

} // namespace srsran
