#ifndef SRSGNB_RAN_FRAME_TYPES_H
#define SRSGNB_RAN_FRAME_TYPES_H

#include <cstddef>

namespace srsgnb {

/// The number of OFDM symbols per slot is constant for all numerologies.
const unsigned NOF_OFDM_SYM_PER_SLOT_NORMAL_CP   = 14;
const unsigned NOF_OFDM_SYM_PER_SLOT_EXTENDED_CP = 12;

} // namespace srsgnb

#endif // SRSGNB_RAN_FRAME_TYPES_H
