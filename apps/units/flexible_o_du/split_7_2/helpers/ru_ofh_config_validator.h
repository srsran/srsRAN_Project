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

#include "apps/services/worker_manager/os_sched_affinity_manager.h"
#include "ru_ofh_config.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

/// Open Fronthaul cell configuration for validation.
struct ru_ofh_cell_validation_config {
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Downlink number of antennas.
  unsigned nof_antennas_dl;
  /// Uplink number of antennas.
  unsigned nof_antennas_ul;
  /// PRACH number of ports.
  unsigned nof_prach_ports;
};

/// Validates the given Open Fronthaul Radio Unit configuration. Returns true on success, false otherwise.
bool validate_ru_ofh_config(const ru_ofh_unit_config& config, span<const ru_ofh_cell_validation_config> cell_config);

} // namespace srsran
