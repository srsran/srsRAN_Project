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

#include "srsran/support/units.h"

namespace srsran {

/// \brief Maximum segment length.
///
/// This is given by the maximum lifting size (i.e., 384) times the maximum number of information bits in base graph
/// BG1 (i.e., 22), as per TS38.212 Section 5.2.2.
static constexpr units::bits MAX_SEG_LENGTH{22 * 384};

/// \brief Maximum number of segments per transport block.
///
/// It assumes 156 resource elements for a maximum of 275 PRB, four layers and eight bits per RE.
static constexpr unsigned MAX_NOF_SEGMENTS = (156 * 275 * 4 * 8) / MAX_SEG_LENGTH.value();

} // namespace srsran
