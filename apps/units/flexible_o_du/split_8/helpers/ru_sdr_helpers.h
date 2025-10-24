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
