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

#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {

class task_executor;

namespace ether {

class frame_notifier;
struct gw_config;

/// Creates a DPDK Ethernet transmitter and receiver pair.
std::pair<std::unique_ptr<gateway>, std::unique_ptr<receiver>> create_dpdk_txrx(const gw_config&      config,
                                                                                task_executor&        rx_executor,
                                                                                frame_notifier&       notifier,
                                                                                srslog::basic_logger& logger);

} // namespace ether
} // namespace srsran
