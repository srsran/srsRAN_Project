/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SCHED_SSB_H
#define SRSGNB_SCHED_SSB_H

#include "sched_prb.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

enum class ssb_pattern_case;
struct cell_slot_resource_allocator;

using ssb_information_list = srsgnb::static_vector<ssb_information, MAX_SSB_PER_SLOT>;

/// @brief Schedule grant for SSB.
///
/// The functions schedules the SSB according to a given periodicity, depending on the frequency and SSB case
///
/// @param[out] slot_allocator      Slot allocator object with SSB grants and DL PRBs.
/// @param[in]  sl_point            Slot point carrying information about current slot.
/// @param[in]  ssb_periodicity     Periodicity of SSB in ms.
/// @param[in]  offset_to_point_A   Offset_to_point_A (see TS 38.211, Section 7.4.3).
/// @param[in]  freq_arfcn          Determines which OFDM symbols to use (see TS 38.213, Section 4.1).
/// @param[in]  in_burst_bitmap     Bitmap for ssb-PositionsInBurst (see TS 38.331).
/// @param[in]  ssb_case            Determines which OFDM symbols to use (see TS 38.213, Section 4.1).
///
/// @remark This function only works for FR1, or L_max = 4 or 8.
void sched_ssb(cell_slot_resource_allocator& slot_allocator,
               const slot_point&        sl_point,
               uint8_t                  ssb_periodicity,
               uint16_t                 offset_to_point_A,
               uint32_t                 freq_arfcn,
               uint64_t                 ssb_in_burst_bitmap,
               ssb_pattern_case         ssb_case,
               bool                     paired_spectrum);

} // namespace srsgnb

#endif /// SRSGNB_SCHED_SSB_H
