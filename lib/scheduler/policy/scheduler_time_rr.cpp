/*
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_time_rr.h"
#include "../slicing/slice_ue_repository.h"

using namespace srsran;

scheduler_time_rr::scheduler_time_rr(const scheduler_ue_expert_config& expert_cfg_, du_cell_index_t cell_index_) :
  expert_cfg(expert_cfg_), cell_index(cell_index_)
{
}

void scheduler_time_rr::compute_ue_dl_priorities(slot_point               pdcch_slot,
                                                 slot_point               pdsch_slot,
                                                 span<ue_newtx_candidate> ue_candidates)
{
  // We perform round-robin by assigning priorities based on the distance between each UE index and the next UE index
  // to be allocated. The distance is computed as -((MAX_NOF_DU_UES + ue_index - next_ue_index) % MAX_NOF_DU_UES). That
  // means that if ue_index==next_ue_index, it has the highest priority of 0, and if ue_index==next_ue_index-1, the UE
  // has the lowest priority of -(MAX_NOF_DU_UES - 1). We avoid the mod operation.
  for (ue_newtx_candidate& candidate : ue_candidates) {
    int index_diff     = candidate.ue->ue_index() - next_dl_ue_index;
    index_diff         = (index_diff < 0) ? (MAX_NOF_DU_UES + index_diff) : index_diff;
    candidate.priority = static_cast<double>(-index_diff);
  }
}

void scheduler_time_rr::compute_ue_ul_priorities(slot_point               pdcch_slot,
                                                 slot_point               pusch_slot,
                                                 span<ue_newtx_candidate> ue_candidates)
{
  for (ue_newtx_candidate& candidate : ue_candidates) {
    int index_diff     = candidate.ue->ue_index() - next_ul_ue_index;
    index_diff         = (index_diff < 0) ? (MAX_NOF_DU_UES + index_diff) : index_diff;
    candidate.priority = static_cast<double>(-index_diff);
  }
}

void scheduler_time_rr::save_dl_newtx_grants(span<const dl_msg_alloc> dl_grants)
{
  if (dl_grants.empty()) {
    return;
  }
  // Mark the UE after the first allocation to be the first UE to get allocated in the following slot.
  // It is important that we equally distribute the opportunity to be the first UE being allocated in a slot for
  // all UEs. Otherwise, we could end up in a situation, where a UE is always the last one to be allocated and
  // can only use the RBs that were left from the previous UE allocations.
  next_dl_ue_index = to_du_ue_index(dl_grants[0].context.ue_index + 1);
}

void scheduler_time_rr::save_ul_newtx_grants(span<const ul_sched_info> ul_grants)
{
  if (ul_grants.empty()) {
    return;
  }
  next_ul_ue_index = to_du_ue_index(ul_grants[0].context.ue_index + 1);
}
