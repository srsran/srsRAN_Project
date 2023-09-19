/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/phy/constants.h"

namespace srsran {
namespace pdcch_constants {

/// Defines the number if resource blocks per frequency resource in a CORESET as per TS38.331 in ControlResourceSet
/// information element.
static constexpr unsigned NOF_RB_PER_FREQ_RESOURCE = 6;

/// Defines the maximum number of frequency resources in a CORESET as per TS38.331 in ControlResourceSet
/// information element.
static constexpr unsigned MAX_NOF_FREQ_RESOURCES = MAX_RB / NOF_RB_PER_FREQ_RESOURCE;

/// Maximum number of DCIs in a single transmission (it is not specified by the TS).
static constexpr unsigned MAX_NOF_DCI = 1;

/// Maximum payload size in bits (it is not specified by the TS).
static constexpr unsigned MAX_DCI_PAYLOAD_SIZE = 128;

/// Number of REs used for data in per RB and symbol as per TS38.211 section 7.3.2.5.
static constexpr unsigned NOF_RE_PDCCH_PER_RB = NRE - 3;

/// Number of REGs per CCE as per TS38.211 Section 7.3.2.2.
static constexpr unsigned NOF_REG_PER_CCE = 6;

/// Maximum CORESET duration in symbols as per TS38.211 Section 7.3.2.2. and TS 38.331 "maxCoReSetDuration".
static constexpr unsigned MAX_CORESET_DURATION = 3;

/// Maximum aggregation level as per TS38.211 Table 7.3.2.1-1.
static constexpr unsigned MAX_AGGREGATION_LEVEL = 16;

/// \brief Maximum number of RBs that can be used for a single PDCCH transmission.
///
/// It coincides with the maximum number of REG that can be used for a single transmission.
static constexpr unsigned MAX_NOF_RB_PDCCH = MAX_AGGREGATION_LEVEL * NOF_REG_PER_CCE;

/// Maximum number of REs for data in a single transmission.
static constexpr unsigned MAX_NOF_RE_PDCCH = MAX_NOF_RB_PDCCH * NOF_RE_PDCCH_PER_RB;

/// Define the maximum number of encoded data bits in a transmission.
static constexpr unsigned MAX_NOF_BITS = MAX_NOF_RE_PDCCH * 2;

/// Define the number of encoded data bits per CCE in a transmission.
static constexpr unsigned NOF_BITS_PER_CCE = NOF_REG_PER_CCE * NOF_RE_PDCCH_PER_RB * 2;

} // namespace pdcch_constants
} // namespace srsran
