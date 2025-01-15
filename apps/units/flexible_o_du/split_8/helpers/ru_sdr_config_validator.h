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
  bs_channel_bandwidth channel_bw_mhz;
  /// Duplex mode.
  duplex_mode dplx_mode;
};

/// Validates the given SDR Radio Unit configuration. Returns true on success, false otherwise.
bool validate_ru_sdr_config(const ru_sdr_unit_config&                 config,
                            span<const ru_sdr_cell_validation_config> cell_config,
                            const os_sched_affinity_bitmask&          available_cpus);

} // namespace srsran
