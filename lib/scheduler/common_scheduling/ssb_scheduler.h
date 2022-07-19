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
/// @param[out] slot_allocator Slot allocator object with SSB grants and DL PRBs.
/// @param[in]  sl_point       Slot point carrying information about current slot.
/// @param[in]  cell_cfg       Reference to scheduler cell configuration.
///
/// @remark This function only works for FR1, or L_max = 4 or 8.
void schedule_ssb(cell_slot_resource_allocator& slot_allocator,
                  const slot_point&             sl_point,
                  const cell_configuration&     cell_cfg);

} // namespace srsgnb
