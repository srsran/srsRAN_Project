#ifndef SRSGNB_ADT_BITMAP_UTILS_H
#define SRSGNB_ADT_BITMAP_UTILS_H

#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

/// \brief Changes the value of a bit in the bitmap. When enable is true, it sets the bit, otherwise it clears the bit.
/// \param[in] bitmap Bitmap to modify.
/// \param[in] bit Bit to change.
/// \param[in] enable Value to set. If true, sets the bit(1), otherwise clears it(0).
/// \note Use this function with integer data types, otherwise it produces undefined behaviour.
template <typename Integer>
void change_bitmap_status(Integer& bitmap, unsigned bit, bool enable)
{
  srsran_assert(sizeof(bitmap) * 8 > bit, "Requested bit ({}), exceeds the bitmap size({})", bit, sizeof(bitmap) * 8);

  if (enable) {
    bitmap |= (1U << bit);
  } else {
    bitmap &= ~(1U << bit);
  }
}

} // namespace srsgnb

#endif // SRSGNB_ADT_BITMAP_UTILS_H
