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
