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

#include "strong_type.h"
#include <cstdint>
#include <cstring>

namespace srsran {

/// \brief Brain floating point (\c bfloat16).
///
/// Custom 16-bit floating point. It consits of 1-bit sign, 8-bit exponent and 7-bit fraction.
/// \note This type is meant for storage purposes only, no operations other than equality comparison are allowed.
using bf16_t = strong_type<uint16_t, struct strong_bf16_tag, strong_equality>;

/// Converts an IEEE 754 single-precision 32-bit float to \c bfloat16.
inline bf16_t to_bf16(float value)
{
  // Reinterpret the value as a 32-bit unsigned integer.
  uint32_t temp_u32;
  ::memcpy(&temp_u32, reinterpret_cast<void*>(&value), 4);

  // The 16 least significant bits of the fractional part are removed. The remaining 7 (most significant) bits are
  // rounded according to the "half to nearest even" method: when the removed part is exactly 0.5 (i.e., a single one
  // followed by 15 zeros), we take the closest even value. In base-10 notation, this means that 2.5 is rounded to 2,
  // while 3.5 is rounded to 4. All other values are rounded to the closest integer (e.g., 3.4 to 3 and 3.8 to 4).
  temp_u32 += 0x7fff + ((temp_u32 >> 16) & 1);

  // Discard the 16 least significant bits.
  uint16_t temp_u16 = temp_u32 >> 16;

  // Reinterpret 16-bit unsigned integer as a BF16.
  return bf16_t(temp_u16);
}

/// Converts a \c bfloat16 to IEEE 754 single-precision 32-bit float.
inline float to_float(bf16_t value)
{
  // Reinterpret BF16 as 16-bit unsigned integer.
  uint16_t temp_u16 = value.value();

  // Extend value as a 32-bit unsigned integer.
  uint32_t temp_u32;
  temp_u32 = static_cast<uint32_t>(temp_u16) << 16;

  // Reinterpret the value as a single-precision 32-bit float.
  float ret;
  ::memcpy(&ret, &temp_u32, 4);

  return ret;
}

} // namespace srsran
