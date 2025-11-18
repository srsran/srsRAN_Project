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

#include "lower_phy_sector.h"
#include "srsran/phy/lower/lower_phy_factory.h"
#include <memory>

namespace srsran {

struct lower_phy_configuration;

/// Lower phy sector dependencies.
struct lower_phy_sector_dependencies {
  srslog::basic_logger&         logger;
  task_executor&                rx_task_executor;
  task_executor&                tx_task_executor;
  task_executor&                dl_task_executor;
  task_executor&                ul_task_executor;
  task_executor&                prach_async_executor;
  lower_phy_error_notifier&     error_notifier;
  lower_phy_timing_notifier*    timing_notifier;
  baseband_gateway&             bb_gateway;
  lower_phy_rx_symbol_notifier& rx_symbol_notifier;
};

/// Helper class that creates the lower PHY sector using the given configuration.
std::unique_ptr<lower_phy_sector> create_lower_phy_sector(const lower_phy_configuration&       lower_phy_config,
                                                          const lower_phy_sector_dependencies& sector_deps);

} // namespace srsran
