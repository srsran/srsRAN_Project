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
#include "srsran/ran/resource_block.h"
#include "srsran/support/math_utils.h"

namespace srsran {

namespace precoding_constants {

/// Minimum precoding PRG size.
static constexpr unsigned MIN_PRG_SIZE = 4;

/// Maximum number of PRG.
static constexpr unsigned MAX_NOF_PRG = divide_ceil(MAX_NOF_PRBS, MIN_PRG_SIZE);

/// Maximum number of layers supported by the precoder.
static constexpr unsigned MAX_NOF_LAYERS = 4;

/// Maximum number of ports supported by the precoder.
static constexpr unsigned MAX_NOF_PORTS = 4;

} // namespace precoding_constants

} // namespace srsran