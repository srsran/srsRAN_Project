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

#include "srsran/phy/lower/lower_phy_configuration.h"
#include "srsran/radio/radio_configuration.h"
#include "srsran/ru/ru_error_notifier.h"
#include "srsran/ru/ru_timing_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"

namespace srsran {

/// Radio Unit generic configuration.
struct ru_generic_configuration {
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

/// RU generic sector dependencies.
struct ru_generic_sector_dependencies {
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

/// Radio Unit generic dependencies.
struct ru_generic_dependencies {
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
  /// RU generic sector dependencies.
  std::vector<ru_generic_sector_dependencies> ru_generic_sector_deps;
};

} // namespace srsran
