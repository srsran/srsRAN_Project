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

#include "apps/units/flexible_du/du_high/du_high_config.h"
#include "apps/units/flexible_du/du_low/du_low_config.h"
#include "apps/units/flexible_du/fapi/fapi_config.h"

namespace srsran {

class ru_error_notifier;
class ru_timing_notifier;
class ru_uplink_plane_rx_symbol_notifier;
struct worker_manager;

namespace srs_du {
struct du_cell_config;
}

/// Flexible DU unit configuration.
struct flexible_du_unit_config {
  /// DU high configuration.
  const du_high_parsed_config& du_high_cfg;
  /// DU low configuration.
  const du_low_unit_config& du_low_cfg;
  /// FAPI configuration.
  const fapi_unit_config& fapi_cfg;
};

/// Radio Unit dependencies of the flexible DU.
struct flexible_du_ru_config {
  span<const srs_du::du_cell_config> du_cells;
  unsigned                           max_processing_delay;
  unsigned                           prach_nof_ports;
};

/// Radio Unit dependencies of the flexible DU.
struct flexible_du_ru_dependencies {
  worker_manager&                     workers;
  ru_uplink_plane_rx_symbol_notifier& symbol_notifier;
  ru_timing_notifier&                 timing_notifier;
  ru_error_notifier&                  error_notifier;
};

} // namespace srsran
