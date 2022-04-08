#ifndef SRSGNB_SCHED_SSB_H
#define SRSGNB_SCHED_SSB_H

#include "sched_prb.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/mac/sched_result.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Enum based on O-RAN .WG8.AAD.0-v0, Section 9.2.3.3.8
enum class ssb_pattern_case;

/// @brief Schedule grant for SSB.
///
/// The functions schedules the SSB according to a given periodicity, depending on the frequency and SSB case
///
/// @param[in]  sl_point            Slot point carrying information about current slot.
/// @param[in]  ssb_periodicity     Periodicity of SSB in ms.
/// @param[in]  offset_to_point_A   (see TS 38.213, Section 4.1)
/// @param[in]  freq_arfcn          Determines which OFDM symbols to use (see TS 38.214, Section 4.1)
/// @param[in]  in_burst_bitmap     Bitmap for ssb-PositionsInBurst (see TS 38.331)
/// @param[in]  ssb_case            Determines which OFDM symbols to use (see TS 38.214, Section 4.1)
/// @param[out]  ssb_list           List of SSB messages to be sent to MAC.
///
/// @remark This function a is basic scheduling function that uses the following simplified assumption:
void sched_ssb(ssb_informatin_list& ssb_list,
               const slot_point&    sl_point,
               uint8_t              ssb_periodicity,
               uint16_t             offset_to_point_A,
               uint32_t             freq_arfcn,
               uint64_t             ssb_in_burst_bitmap,
               ssb_pattern_case     ssb_case,
               bool                 paired_spectrum);

} // namespace srsgnb

#endif /// SRSGNB_SCHED_SSB_H
