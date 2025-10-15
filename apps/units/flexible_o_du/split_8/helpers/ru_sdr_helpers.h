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
#include <cmath>

namespace srsran {

/// Calculates the dBFS calibration value as sqrt(sampling rate / subcarrier spacing). This factor is the magnitude of a
/// single subcarrier in normalized PHY linear units equivalent to a constant signal with a power of 0 dBFS.
inline float calculate_dBFS_calibration_value(double sampling_rate_MHz, subcarrier_spacing scs)
{
  return std::sqrt(sampling_rate_MHz * 1e3 / scs_to_khz(scs));
}

} // namespace srsran
