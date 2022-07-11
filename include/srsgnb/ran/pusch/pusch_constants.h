/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {

namespace pusch_constants {

/// \brief Maximum number of RE per resource block in a PUSCH transmission.
///
/// As per TS38.214 Section 6.1.4.2.
static constexpr unsigned MAX_NRE_PER_RB = 156;

/// \brief Maximum modulation order supported on PUSCH transmissions.
///
/// As per TS38.214 Section 6.1.4.1 with \c mcs-Table set to \c qam256.
static constexpr unsigned MAX_MODULATION_ORDER = 8;

}

} // namespace srsgnb
