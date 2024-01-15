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

namespace srsran {

/// \brief PDCP NR t-Reordering timer values.
/// This timer is used to detect loss of PDCP Data PDUs.
/// See TS 38.322 for timer description and TS 38.331 for valid timer durations.
enum class pdcp_t_reordering {
  ms0      = 0,
  ms1      = 1,
  ms2      = 2,
  ms4      = 4,
  ms5      = 5,
  ms8      = 8,
  ms10     = 10,
  ms15     = 15,
  ms20     = 20,
  ms30     = 30,
  ms40     = 40,
  ms50     = 50,
  ms60     = 60,
  ms80     = 80,
  ms100    = 100,
  ms120    = 120,
  ms140    = 140,
  ms160    = 160,
  ms180    = 180,
  ms200    = 200,
  ms220    = 220,
  ms240    = 240,
  ms260    = 260,
  ms280    = 280,
  ms300    = 300,
  ms500    = 500,
  ms750    = 750,
  ms1000   = 1000,
  ms1250   = 1250,
  ms1500   = 1500,
  ms1750   = 1750,
  ms2000   = 2000,
  ms2250   = 2250,
  ms2500   = 2500,
  ms2750   = 2750,
  ms3000   = 3000,
  infinity = -1
};

/// Conversion of t-Reordering from integer to enum.
inline bool pdcp_t_reordering_from_int(pdcp_t_reordering& t_reord, int num)
{
  switch (num) {
    case 0:
    case 1:
    case 2:
    case 4:
    case 5:
    case 8:
    case 10:
    case 15:
    case 20:
    case 30:
    case 40:
    case 50:
    case 60:
    case 80:
    case 100:
    case 120:
    case 140:
    case 160:
    case 180:
    case 200:
    case 220:
    case 240:
    case 260:
    case 280:
    case 300:
    case 500:
    case 750:
    case 1000:
    case 1250:
    case 1500:
    case 1750:
    case 2000:
    case 2250:
    case 2500:
    case 2750:
    case 3000:
    case -1:
      t_reord = static_cast<pdcp_t_reordering>(num);
      return true;
    default:
      return false;
  }
}

/// \brief Convert PDCP NR t-Reordering from enum to integer.
constexpr int16_t pdcp_t_reordering_to_int(pdcp_t_reordering t_reordering)
{
  return static_cast<int16_t>(t_reordering);
}

} // namespace srsran