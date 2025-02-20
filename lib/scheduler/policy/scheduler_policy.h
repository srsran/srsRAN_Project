/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "slice_allocator.h"

namespace srsran {

using ue_sched_priority                       = double;
const ue_sched_priority forbid_sched_priority = std::numeric_limits<ue_sched_priority>::min();

struct ue_pdsch_newtx_candidate {
  const slice_ue*   ue;
  const ue_cell*    ue_cc;
  unsigned          pending_bytes;
  ue_sched_priority priority;
};

/// Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot
/// The data_scheduler object will be common to all cells and slots.
class scheduler_policy
{
public:
  virtual ~scheduler_policy() = default;

  /// Compute UE candidate priorities for scheduling.
  virtual void compute_ue_priorities(slot_point                     pdcch_slot,
                                     slot_point                     pdsch_slot,
                                     du_cell_index_t                cell_index,
                                     span<ue_pdsch_newtx_candidate> ue_candidates) = 0;

  /// Save UE DL newtx grants.
  virtual void save_dl_newtx_grants(span<const dl_msg_alloc> dl_grants) = 0;

  /// Schedule UE DL grants for a given slot and one or more cells.
  virtual void dl_sched(slice_dl_sched_context& dl_ctxt) = 0;

  /// Schedule UE UL grants for a given {slot, cell}.
  virtual void ul_sched(slice_ul_sched_context& ul_ctxt) = 0;
};

} // namespace srsran
