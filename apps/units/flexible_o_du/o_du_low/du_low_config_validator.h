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
                            span<const du_low_prach_validation_config> prach_cells_config);

} // namespace srsran
