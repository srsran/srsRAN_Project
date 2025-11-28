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

#include "srsran/phy/lower/lower_phy_configuration.h"
#include "srsran/radio/radio_configuration.h"
#include "srsran/ru/ru_error_notifier.h"
#include "srsran/ru/ru_timing_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"

namespace srsran {

/// SDR Radio Unit configuration.
struct ru_sdr_configuration {
  /// Metrics enabled flag.
  bool are_metrics_enabled;
  /// Device driver.
  std::string device_driver;
  /// Radio configuration.
  radio_configuration::radio radio_cfg;
  /// Lower PHY configurations.
  std::vector<lower_phy_configuration> lower_phy_config;
  /// \brief Optional starting time.
  ///
  /// Starts radio operation at the given time. The use of this parameter is for starting radio operation of several
  /// instances at same time.
  std::optional<std::chrono::system_clock::time_point> start_time;
};

/// SDR RU sector dependencies.
struct ru_sdr_sector_dependencies {
  /// Logger.
  srslog::basic_logger& logger;
  /// Receive task executor.
  task_executor& rx_task_executor;
  /// Transmit task executor.
  task_executor& tx_task_executor;
  /// Downlink task executor.
  task_executor& dl_task_executor;
  /// Uplink task executor.
  task_executor& ul_task_executor;
  /// PRACH asynchronous task executor.
  task_executor& prach_async_executor;
};

/// SDR Radio Unit dependencies.
struct ru_sdr_dependencies {
  /// Radio executor.
  task_executor& radio_exec;
  /// Radio logger.
  srslog::basic_logger& rf_logger;
  /// Radio Unit uplink plane received symbol notifier.
  ru_uplink_plane_rx_symbol_notifier& symbol_notifier;
  /// Radio Unit timing notifier.
  ru_timing_notifier& timing_notifier;
  /// Radio Unit error notifier.
  ru_error_notifier& error_notifier;
  /// SDR RU sector dependencies.
  std::vector<ru_sdr_sector_dependencies> sector_dependencies;
};

} // namespace srsran
