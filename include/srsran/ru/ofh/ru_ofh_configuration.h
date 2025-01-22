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

#include "srsran/ofh/ofh_sector_config.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class ru_error_notifier;
class ru_timing_notifier;
class ru_uplink_plane_rx_symbol_notifier;
class task_executor;

/// Radio Unit configuration for the Open Fronthaul implementation.
struct ru_ofh_configuration {
  /// Individual Open Fronthaul sector configurations.
  std::vector<ofh::sector_configuration> sector_configs;
  /// GPS Alpha - Valid value range: [0, 1.2288e7].
  unsigned gps_Alpha;
  /// GPS Beta - Valid value range: [-32768, 32767].
  int gps_Beta;
};

/// Radio Unit dependencies for the Open Fronthaul implementation.
struct ru_ofh_dependencies {
  /// Logger.
  srslog::basic_logger* logger = nullptr;
  /// Radio Unit timing notifier.
  ru_timing_notifier* timing_notifier = nullptr;
  /// Radio Unit error notifier.
  ru_error_notifier* error_notifier = nullptr;
  /// Radio Unit received symbol notifier.
  ru_uplink_plane_rx_symbol_notifier* rx_symbol_notifier = nullptr;
  /// Realtime timing task executor.
  task_executor* rt_timing_executor = nullptr;

  /// Individual Open Fronthaul sector dependencies.
  std::vector<ofh::sector_dependencies> sector_dependencies;
};

/// Returns true if the given Open Fronthaul configuration is valid, otherwise false.
bool is_valid_ru_ofh_config(const ru_ofh_configuration& config);

} // namespace srsran
