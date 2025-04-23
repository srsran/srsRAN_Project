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

#include "srsran/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ethernet/ethernet_transmitter.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {

class task_executor;

namespace ether {

class frame_notifier;
struct transmitter_config;

/// Creates a DPDK port context object.
std::shared_ptr<dpdk_port_context> create_dpdk_port_context(const transmitter_config& config);

/// Creates a DPDK Ethernet transmitter and receiver pair.
std::pair<std::unique_ptr<transmitter>, std::unique_ptr<receiver>>
create_dpdk_txrx(const transmitter_config& config, task_executor& rx_executor, srslog::basic_logger& logger);

} // namespace ether
} // namespace srsran
