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

#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief Sets the value of a bit in the bitmap. When enable is true, it sets the bit, otherwise it clears the bit.
/// \param[in] bitmap Bitmap to modify.
/// \param[in] bit Bit to change.
/// \param[in] enable Value to set. If true, sets the bit(1), otherwise clears it(0).
/// \note Use this function with integer data types, otherwise it produces undefined behaviour.
template <typename Integer>
void set_bitmap_bit(Integer& bitmap, unsigned bit, bool enable)
{
  static_assert(std::is_integral<Integer>::value, "Integral required");
  srsran_assert(sizeof(bitmap) * 8 > bit, "Requested bit ({}), exceeds the bitmap size({})", bit, sizeof(bitmap) * 8);

  if (enable) {
    bitmap |= (1U << bit);
  } else {
    bitmap &= ~(1U << bit);
  }
}

/// \brief Checks the value of a bit in the bitmap and returns a true if the bit is set, otherwise false.
/// \param[in] bitmap Bitmap to check.
/// \param[in] bit Bit to check.
/// \return True when the bit equals 1, otherwise false.
/// \note Use this function with integer data types, otherwise it produces undefined behaviour.
template <typename Integer>
bool check_bitmap_bit(Integer bitmap, unsigned bit)
{
  static_assert(std::is_integral<Integer>::value, "Integral required");
  srsran_assert(sizeof(bitmap) * 8 > bit, "Requested bit ({}), exceeds the bitmap size({})", bit, sizeof(bitmap) * 8);

  return (bitmap & (1U << bit));
}

} // namespace srsran
