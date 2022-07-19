/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/cell_configuration.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

enum class ssb_pattern_case;
struct cell_slot_resource_allocator;

using ssb_information_list = srsgnb::static_vector<ssb_information, MAX_SSB_PER_SLOT>;

/// Derive SSB Subcarrier Spacing from SSB pattern case.
subcarrier_spacing ssb_case_to_scs(ssb_pattern_case ssb_case);

/// @brief Schedule grant for SSB.
///
/// The functions schedules the SSB according to a given periodicity, depending on the frequency and SSB case
///
/// @param[out] slot_allocator        Slot allocator object with SSB grants and DL PRBs.
/// @param[in]  sl_point              Slot point carrying information about current slot.
/// @param[in]  ssb_periodicity       Periodicity of SSB in ms.
/// @param[in]  offset_to_point_A     Offset_to_point_A (see TS 38.211, Section 7.4.3).
/// @param[in]  freq_arfcn            Determines which OFDM symbols to use (see TS 38.213, Section 4.1).
/// @param[in]  ssb_subcarrier_offset k_SSB, as per TS 38.211, Section 7.4.3.1 and TS 38.213, Section 4.1.
/// @param[in]  in_burst_bitmap       Bitmap for ssb-PositionsInBurst (see TS 38.331).
/// @param[in]  ssb_case              Determines which OFDM symbols to use (see TS 38.213, Section 4.1).
/// @param[in]  scs_common            subCarrierSpacingCommon as per TS 38.331, MIB. This corresponds to the SCS of the
///                                   initial_DL_BWP.
/// @param[in]  paired_spectrum       Indicates whether the band is in paired spectrum.
///
/// @remark This function only works for FR1, or L_max = 4 or 8.
void schedule_ssb(cell_slot_resource_allocator& slot_allocator,
                  const slot_point&             sl_point,
                  const cell_configuration&     cell_cfg);

} // namespace srsgnb
