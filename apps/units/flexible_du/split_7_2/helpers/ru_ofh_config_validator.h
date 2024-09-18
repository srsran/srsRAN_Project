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

#include "apps/services/os_sched_affinity_manager.h"
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
bool validate_ru_ofh_config(const ru_ofh_unit_config&                 config,
                            span<const ru_ofh_cell_validation_config> cell_config,
                            const os_sched_affinity_bitmask&          available_cpus);

} // namespace srsran
