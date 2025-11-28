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
