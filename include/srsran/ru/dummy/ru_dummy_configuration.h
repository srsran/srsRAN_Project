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

#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ru/ru_error_notifier.h"
#include "srsran/ru/ru_timing_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// Collects the necessary parameters for the dummy radio unit.
struct ru_dummy_configuration {
  /// Flag that enables (or not) metrics.
  bool are_metrics_enabled;
  /// Subcarrier spacing - Determines the slot timing.
  subcarrier_spacing scs;
  /// Number of sectors.
  unsigned nof_sectors;
  /// Receive resource grid number of PRB.
  unsigned rx_rg_nof_prb;
  /// Receive resource grid number of ports.
  unsigned rx_rg_nof_ports;
  /// Receive PRACH buffer number of ports.
  unsigned rx_prach_nof_ports;
  /// \brief Number of slots is notified in advance of the transmission time.
  ///
  /// Sets the maximum allowed processing delay in slots.
  unsigned max_processing_delay_slots;
  /// DL processing processing delay in slots. See \ref ru_dummy_appconfig::dl_processing_delay for more information.
  unsigned dl_processing_delay;
  /// Time scaling, \ref ru_dummy_unit_config::time_scaling for more details.
  float time_scaling;
};

/// Collects the necessary dependencies for the dummy radio unit.
struct ru_dummy_dependencies {
  /// Logger.
  srslog::basic_logger& logger;
  /// Asynchronous task executor.
  task_executor* executor = nullptr;
  /// Radio Unit uplink plane received symbol notifier.
  ru_uplink_plane_rx_symbol_notifier& symbol_notifier;
  /// Radio Unit timing notifier.
  ru_timing_notifier& timing_notifier;
  /// Radio Unit error notifier.
  ru_error_notifier& error_notifier;
};

} // namespace srsran
