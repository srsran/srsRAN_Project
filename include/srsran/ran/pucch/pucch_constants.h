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

#include "srsran/ran/resource_block.h"

namespace srsran {

namespace pucch_constants {

/// PUCCH does not make use of spatial multiplexing.
static constexpr unsigned MAX_LAYERS = 1;

/// Maximum number of symbols (without DM-RS) that PUCCH Format 1 can transmit.
static constexpr unsigned FORMAT1_N_MAX = 7;

/// Minimum number of symbols that PUCCH Format 2 can transmit.
static constexpr unsigned FORMAT2_MIN_NSYMB = 1;

/// Maximum number of symbols that PUCCH Format 2 can transmit.
static constexpr unsigned FORMAT2_MAX_NSYMB = 2;

/// Maximum number of PRBs allocated for PUCCH Format 2.
static constexpr unsigned FORMAT2_MAX_NPRB = 16;

/// Minimum number of UCI payload bits carried by a PUCCH Format 2 resource.
static constexpr unsigned FORMAT2_MIN_UCI_NBITS = 3;

/// Number of control data subcarriers per Resource Block for PUCCH Format 2, as per TS38.213 Section 9.1.5.2.
static constexpr unsigned FORMAT2_NOF_DATA_SC = 8;

/// \brief Maximum number of resource elements used by PUCCH.
///
/// It corresponds to PUCCH Format 3 with a bandwidth of 16 PRBs and a duration of 14 symbols, two of which are occupied
/// by the DM-RS.
static constexpr unsigned MAX_NOF_RE = NOF_SUBCARRIERS_PER_RB * 16 * (14 - 2);

/// \brief Maximum number of LLRs corresponding to a PUCCH.
///
/// Derives from \ref PUCCH_MAX_NOF_RE assuming QPSK modulation.
static constexpr unsigned MAX_NOF_LLR = MAX_NOF_RE * 2;

/// \brief Maximum effective code rate for PUCCH formats 2, 3 and 4.
///
/// Maximum value given by TS38.213 Table 9.2.5.2-1 and TS38.331 Section 6.3.2 Enumeration \e PUCCH-MaxCodeRate within
/// Information Element \e PUCCH-FormatConfig.
static constexpr float MAX_CODE_RATE = 0.80F;

} // namespace pucch_constants
} // namespace srsran
