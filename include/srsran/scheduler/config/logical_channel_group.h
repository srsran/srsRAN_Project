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

#include <cstdint>
#include <type_traits>

namespace srsran {

/// Logical Channel Group as per TS38.331.
enum lcg_id_t : uint8_t { MAX_LCG_ID = 7, MAX_NOF_LCGS = 8, LCG_ID_INVALID = 8 };

inline lcg_id_t uint_to_lcg_id(std::underlying_type_t<lcg_id_t> val)
{
  return static_cast<lcg_id_t>(val);
}

} // namespace srsran