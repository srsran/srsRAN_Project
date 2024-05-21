/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "ru_sdr_config.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

/// SDR cell configuration for validation.
struct ru_sdr_cell_validation_config {
  /// Cell subcarrier spacing.
  subcarrier_spacing common_scs;
  /// PRACH format.
  prach_format_type prach_format;
  /// PRACH preamble information.
  prach_preamble_information preamble_info;
  /// Channel bandwidth in MHz.
  bs_channel_bandwidth_fr1 channel_bw_mhz;
  /// Duplex mode.
  duplex_mode dplx_mode;
};

/// Validates the given SDR Radio Unit configuration. Returns true on success, false otherwise.
bool validate_ru_sdr_config(const ru_sdr_unit_config&                 config,
                            span<const ru_sdr_cell_validation_config> cell_config,
                            const os_sched_affinity_bitmask&          available_cpus);

} // namespace srsran
