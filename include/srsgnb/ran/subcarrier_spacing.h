
#ifndef SRSGNB_RAN_SUBCARRIER_SPACING_H
#define SRSGNB_RAN_SUBCARRIER_SPACING_H

#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

/// Representation of subcarrier spacing.
enum subcarrier_spacing { kHz15 = 0, kHz30, kHz60, kHz120, kHz240, invalid };

/// Check if SCS value is valid.
inline bool is_scs_valid(subcarrier_spacing scs)
{
  return scs <= subcarrier_spacing::kHz240;
}

/// Convert SCS into integer in kHz
inline unsigned scs_to_khz(subcarrier_spacing scs)
{
  constexpr static unsigned values[] = {15, 30, 60, 120, 240};
  srsran_sanity_check(is_scs_valid(scs), "Invalid SCS");
  return values[(unsigned)scs];
}

} // namespace srsgnb

#endif // SRSGNB_RAN_SUBCARRIER_SPACING_H
