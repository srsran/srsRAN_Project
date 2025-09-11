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

#include "srsran/ran/frequency_range.h"

namespace srsran {
namespace split6_du_low {

/// [Implementation defined] PRACH is always in port 0.
constexpr unsigned PRACH_PORT = 0U;

/// [Implementation defined] Cell identifier for building the split.
constexpr unsigned CELL_ID = 0U;

/// [Implementation defined] Number of PRACH ports.
constexpr unsigned PRACH_NOF_PORTS = 1U;

/// [Implementation defined] Maximum number of layers for PUSCH.
constexpr unsigned PUSCH_MAX_NOF_LAYERS = 1U;

/// [Implementation defined] Maximum number of cells supported by the split 6 O-DU low.
constexpr unsigned NOF_CELLS_SUPPORTED = 1U;

/// [Implementation defined] Maximum number of transmission antennas supported.
constexpr unsigned NOF_TX_ANTENNA_SUPPORTED = 4U;

/// [Implementation defined] Frequency range supported for O-DU low split 6.
constexpr frequency_range freq_range = frequency_range::FR1;

} // namespace split6_du_low
} // namespace srsran
