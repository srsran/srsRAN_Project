/*
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_ADT_BITMAP_UTILS_H
#define SRSGNB_ADT_BITMAP_UTILS_H

#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

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

} // namespace srsgnb

#endif // SRSGNB_ADT_BITMAP_UTILS_H
