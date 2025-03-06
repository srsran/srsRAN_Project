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

#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config.h"

namespace srsran {

class ru_error_notifier;
class ru_timing_notifier;
class ru_uplink_plane_rx_symbol_notifier;
struct worker_manager;

namespace srs_du {
struct du_cell_config;
}

/// Flexible O-RAN DU unit configuration.
struct flexible_o_du_unit_config {
  struct ru_config {
    app_helpers::metrics_config config;
    bool                        enable_ru_metrics;
  };

  /// O-RAN DU high configuration.
  const o_du_high_unit_config& odu_high_cfg;
  /// DU low configuration.
  const du_low_unit_config& du_low_cfg;
  /// RU configuration.
  ru_config ru_cfg;
};

/// Radio Unit dependencies of the flexible O-RAN DU.
struct flexible_o_du_ru_config {
  span<const srs_du::du_cell_config> du_cells;
  unsigned                           max_processing_delay;
  unsigned                           prach_nof_ports;
};

/// Radio Unit dependencies of the flexible O-RAN DU.
struct flexible_o_du_ru_dependencies {
  worker_manager&                     workers;
  ru_uplink_plane_rx_symbol_notifier& symbol_notifier;
  ru_timing_notifier&                 timing_notifier;
  ru_error_notifier&                  error_notifier;
};

} // namespace srsran
