/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

/// Creates a DPDK Ethernet gateway.
std::unique_ptr<gateway> create_dpdk_gateway(srslog::basic_logger& logger);

/// Creates a DPDK Ethernet receiver.
std::unique_ptr<receiver>
create_dpdk_receiver(task_executor& executor, frame_notifier& notifier, srslog::basic_logger& logger);

} // namespace ether
} // namespace srsran
