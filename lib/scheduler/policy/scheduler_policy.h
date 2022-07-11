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

#include "ue_allocator.h"

namespace srsgnb {

/// Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot
/// The data_scheduler object will be common to all cells and slots.
class scheduler_policy
{
public:
  virtual ~scheduler_policy() = default;

  /// Schedule UE DL grants for a given slot and one or more cells.
  /// \param[in/out] pdsch_alloc PDSCH grant allocator. This object provides a handle to allocate PDSCH grants in the
  ///                            gNB resource grid and observe the current DL gNB resource grid occupancy state.
  /// \param[in] ues List of eligible UEs to be scheduled in the given slot.
  virtual void dl_sched(ue_pdsch_allocator& pdsch_alloc, const ue_list& ues) = 0;

  /// Schedule UE UL grants for a given {slot, cell}.
  /// \param[in/out] pusch_alloc PUSCH grant allocator. This object provides a handle to allocate PUSCH grants in the
  ///                            gNB resource grid and observe the current UL gNB resource grid occupancy state.
  /// \param[in] ues List of eligible UEs to be scheduled in the given slot.
  virtual void ul_sched(ue_pusch_allocator& pusch_alloc, const ue_list& ues) = 0;
};

} // namespace srsgnb
