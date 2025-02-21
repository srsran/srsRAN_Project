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

#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/scheduler/result/pusch_info.h"

namespace srsran {

class slice_ue;
class ue_cell;

/// Type used to represent a UE priority.
using ue_sched_priority = double;

/// Priority value used to represent that the UE must not be scheduled.
const ue_sched_priority forbid_sched_priority = std::numeric_limits<ue_sched_priority>::min();

/// UE candidate for DL or UL scheduling.
struct ue_newtx_candidate {
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

  /// Compute UE candidate priorities for DL scheduling.
  virtual void compute_ue_dl_priorities(slot_point               pdcch_slot,
                                        slot_point               pdsch_slot,
                                        du_cell_index_t          cell_index,
                                        span<ue_newtx_candidate> ue_candidates) = 0;

  /// Compute UE candidate priorities for UL scheduling.
  virtual void compute_ue_ul_priorities(slot_point               pdcch_slot,
                                        slot_point               pusch_slot,
                                        du_cell_index_t          cell_index,
                                        span<ue_newtx_candidate> ue_candidates) = 0;

  /// Save UE DL newtx grants.
  virtual void save_dl_newtx_grants(span<const dl_msg_alloc> dl_grants) = 0;

  /// Save UE UL newtx grants.
  virtual void save_ul_newtx_grants(span<const ul_sched_info> dl_grants) = 0;
};

} // namespace srsran
