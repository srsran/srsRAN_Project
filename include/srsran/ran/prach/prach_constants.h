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

#include "srsran/ran/phy_time_unit.h"

namespace srsran {

namespace prach_constants {

/// Long sequence length as per TS38.211 Section 6.3.3.1, case \f$L_{RA}=839\f$.
static constexpr unsigned LONG_SEQUENCE_LENGTH = 839U;

/// Maximum number of OFDM symbols per slot containing long-sequence PRACH preambles. Inferred from TS38.211
/// Table 6.3.3.1-1.
static constexpr unsigned LONG_SEQUENCE_MAX_NOF_SYMBOLS = 4U;

/// Short sequence length as per TS38.211 Section 6.3.3.1, case \f$L_{RA}=139\f$.
static constexpr unsigned SHORT_SEQUENCE_LENGTH = 139U;

/// Maximum number of OFDM symbols per slot containing short-sequence PRACH preambles. Inferred from TS38.211
/// Table 6.3.3.1-2.
static constexpr unsigned SHORT_SEQUENCE_MAX_NOF_SYMBOLS = 12U;

/// Maximum PRACH reception window length in the time domain equal to half frame. Inferred from TS38.211 Table
/// 6.3.3.1-1, taking the maximum of \f$N_u+N_{CP}^{RA}\f$ rounded to the next subframe.
static constexpr phy_time_unit MAX_WINDOW_LENGTH = phy_time_unit::from_seconds(5e-3);

/// Maximum number of preambles per time-frequency PRACH occasion as per TS38.211 Section 6.3.3.1.
static constexpr unsigned MAX_NUM_PREAMBLES = 64;

/// Maximum number of PRACH time-domain occasions within a slot as per TS38.211, Tables 6.3.3.2-[2-4].
static constexpr unsigned MAX_NOF_PRACH_TD_OCCASIONS = 7;

/// Maximum number of PRACH frequency-domain occasions within a slot. It is given by the maximum value of \e msg1-FDM in
/// TS38.331 Section 6.3.2 Information Element \e RACH-ConfigGeneric.
static constexpr unsigned MAX_NOF_PRACH_FD_OCCASIONS = 8;

/// Maximum number of PRACH occasions within a slot as per TS38.211, Tables 6.3.3.2-[2-4] and maximum msg1-FDM of 8
/// according to TS 38.331.
static constexpr size_t MAX_NOF_PRACH_OCCASIONS_PER_SLOT = MAX_NOF_PRACH_TD_OCCASIONS * MAX_NOF_PRACH_FD_OCCASIONS;

} // namespace prach_constants

} // namespace srsran
