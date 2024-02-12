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
///\file
///\brief pusch_constants - namespace containing constants related to PUSCH transmissions.

#pragma once

#include "srsran/phy/constants.h"
#include "srsran/support/units.h"

namespace srsran {

namespace pusch_constants {

/// \brief Maximum number of RE per resource block in a PUSCH transmission.
///
/// As per TS38.214 Section 6.1.4.2.
static constexpr unsigned MAX_NRE_PER_RB = 156;

/// \brief Maximum modulation order supported on PUSCH transmissions.
///
/// As per TS38.214 Section 6.1.4.1 with \c mcs-Table set to \c qam256.
static constexpr unsigned MAX_MODULATION_ORDER = 8;

/// \brief Maximum number of PUSCH transmission layers.
///
/// As per TS38.211 Section 6.3.1.3.
static constexpr unsigned MAX_NOF_LAYERS = 4;

/// Returns number of REs per codeword in a single transmission.
inline constexpr unsigned get_codeword_max_symbols(unsigned nof_prb, unsigned nof_layers)
{
  return nof_prb * pusch_constants::MAX_NRE_PER_RB * nof_layers;
}

/// Returns number of encoded bits per codeword in a single transmission.
inline constexpr units::bits get_max_codeword_size(unsigned nof_prb, unsigned nof_layers)
{
  return units::bits{get_codeword_max_symbols(nof_prb, nof_layers) * pusch_constants::MAX_MODULATION_ORDER};
}

/// Maximum number of OFDM symbols carrying DM-RS in a slot is at most \f$4 \times 2\f$, being 4 the maximum
/// number of positions \f$\bar{l}\f$ and 2 the maximum number of indices \f$l'\f$, as per TS38.211 Section 6.4.1.1.
static constexpr unsigned MAX_NOF_DMRS_SYMBOLS = 4 * 2;

/// Maximum number of subcarriers carrying DM-RS in a symbol. It is at most half of the total number of subcarriers
/// (i.e., <tt>MAX_RB * NRE / 2</tt>) is assigned a DM-RS symbol.
static constexpr unsigned MAX_NOF_DMRS_SUBC = MAX_RB * NRE / 2;

/// Maximum number of PUSCH time domain resource allocations. See TS 38.331, \c maxNrofUL-Allocations.
static constexpr unsigned MAX_NOF_PUSCH_TD_RES_ALLOCS = 16;

} // namespace pusch_constants

} // namespace srsran
