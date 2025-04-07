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
  /// Maximum number of PRACH concurrent requests.
  unsigned max_nof_prach_concurrent_requests = 11;
  /// Device driver.
  std::string device_driver;
  /// Radio configuration.
  radio_configuration::radio radio_cfg;
  /// Lower PHY configurations.
  std::vector<lower_phy_configuration> lower_phy_config;
  /// Radio executor.
  task_executor* radio_exec = nullptr;
  /// Radio logger.
  srslog::basic_logger* rf_logger = nullptr;
  /// Radio Unit uplink plane received symbol notifier.
  ru_uplink_plane_rx_symbol_notifier* symbol_notifier = nullptr;
  /// Radio Unit timing notifier.
  ru_timing_notifier* timing_notifier = nullptr;
  /// Radio Unit error notifier.
  ru_error_notifier* error_notifier = nullptr;
};

} // namespace srsran
