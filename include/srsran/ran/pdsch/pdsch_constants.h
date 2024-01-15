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
///\brief pdsch_constants - namespace containing constants related to PDSCH transmissions.

#pragma once

#include "srsran/ran/resource_block.h"
#include "srsran/support/units.h"
#include <cstddef>

namespace srsran {

namespace pdsch_constants {

/// \brief Maximum number of RE per resource block in a PDSCH transmission.
///
/// As per TS38.214 Section 5.1.3.2.
static constexpr unsigned MAX_NRE_PER_RB = 156;

/// \brief Maximum modulation order supported on PDSCH transmissions.
///
/// As per TS38.214 Section 5.1.3.1 with \c mcs-Table set to \c qam256.
static constexpr unsigned MAX_MODULATION_ORDER = 8;

/// \brief Maximum number of PDSCH transmission layers per codeword.
///
/// As per TS38.211 Table 7.3.1.3-1.
static constexpr unsigned CODEWORD_MAX_NOF_LAYERS = 4;

/// \brief Maximuym number of codewords that can be contained in a PDSCH transmission.
///
/// As per TS38.211 Table 7.3.1.3-1.
static constexpr unsigned MAX_NOF_CODEWORDS = 2;

/// \brief Maximum number of PDSCH transmission layers.
///
/// As per TS38.211 Table 7.3.1.3-1.
static constexpr unsigned MAX_NOF_LAYERS = CODEWORD_MAX_NOF_LAYERS * MAX_NOF_CODEWORDS;

/// \brief Maximum number of resource elements that can be mapped into a single codeword.
///
/// Calculated as the product of maximum number or RE per PRB and the maximum number of PRB in the a resource grid.
static constexpr unsigned CODEWORD_MAX_NOF_RE = MAX_NRE_PER_RB * MAX_NOF_PRBS;

/// \brief Maximum number of symbols that can be mapped into a single codeword.
///
/// Calculated as the product of the maximum number of resource elements and the maximum layers
static constexpr unsigned CODEWORD_MAX_SYMBOLS = CODEWORD_MAX_NOF_RE * CODEWORD_MAX_NOF_LAYERS;

/// \brief Maximum number of bits that can be modulated into a single codeword.
///
/// Calculated as the product of the maximum number of resource elements and the maximum modulation order.
static constexpr units::bits CODEWORD_MAX_SIZE{CODEWORD_MAX_SYMBOLS * MAX_MODULATION_ORDER};

} // namespace pdsch_constants

/// \brief Maximum size in bytes of a PDSCH PDU for a given UE.
static constexpr size_t MAX_DL_PDU_LENGTH = pdsch_constants::CODEWORD_MAX_SIZE.round_up_to_bytes().value();

} // namespace srsran
