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

#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

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
    float                       dBFS_calibration_value = 1.F;
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
  struct cell_config {
    subcarrier_spacing                     scs;
    cyclic_prefix                          cp;
    bs_channel_bandwidth                   bw;
    unsigned                               nof_rx_antennas;
    unsigned                               nof_tx_antennas;
    std::optional<tdd_ul_dl_config_common> tdd_config;
    unsigned                               dl_arfcn;
    unsigned                               ul_arfcn;
    frequency_range                        freq_range;
  };

  std::vector<cell_config> cells;
  unsigned                 max_processing_delay;
  unsigned                 prach_nof_ports;
};

/// Radio Unit dependencies of the flexible O-RAN DU.
struct flexible_o_du_ru_dependencies {
  worker_manager&                     workers;
  ru_uplink_plane_rx_symbol_notifier& symbol_notifier;
  ru_timing_notifier&                 timing_notifier;
  ru_error_notifier&                  error_notifier;
};

} // namespace srsran
