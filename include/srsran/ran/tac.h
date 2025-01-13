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

#include <cstdint>

namespace srsran {

/// 3GPP TS 38.331, TrackingAreaCode ::= BIT STRING (SIZE (24)).
using tac_t                 = uint32_t;
constexpr tac_t INVALID_TAC = 16777216;

/// Returns true if the given struct is valid, otherwise false.
constexpr bool is_valid(tac_t tac)
{
  // TAC must be in [0..16777215].
  return tac < INVALID_TAC;
}

} // namespace srsran
