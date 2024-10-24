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

#include "du_low_config.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/prach/prach_subcarrier_spacing.h"

namespace srsran {

/// DU low PRACH configuration for validation.
struct du_low_prach_validation_config {
  /// PRACH format.
  prach_format_type format;
  /// PRACH subcarrier spacing.
  prach_subcarrier_spacing prach_scs;
  /// PRACH zero correlation zone.
  unsigned zero_correlation_zone;
  /// PRACH number of ports.
  unsigned nof_prach_ports;
  /// Number of uplink antennas.
  unsigned nof_antennas_ul;
};

/// Validates the given DU low configuration. Returns true on success, false otherwise.
bool validate_du_low_config(const du_low_unit_config&                  config,
                            span<const du_low_prach_validation_config> prach_cells_config,
                            const os_sched_affinity_bitmask&           available_cpus);

} // namespace srsran
