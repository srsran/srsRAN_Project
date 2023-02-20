/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
///\file
///\brief pusch_constants - namespace containing constants related to PUSCH transmissions.

#pragma once

#include "srsgnb/phy/constants.h"

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

/// Maximum number of PUSCH transmission layers, as per TS38.211 Section 6.3.1.3.
static constexpr unsigned MAX_NOF_LAYERS = 4;

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
