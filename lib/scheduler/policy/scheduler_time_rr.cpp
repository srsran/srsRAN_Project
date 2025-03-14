/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "scheduler_time_rr.h"
#include "../slicing/slice_ue_repository.h"

using namespace srsran;

scheduler_time_rr::scheduler_time_rr(const scheduler_ue_expert_config& expert_cfg_) : expert_cfg(expert_cfg_) {}

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
  // \ref compute_ue_dl_priorities
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

  // Mark the UE after the second last allocation to be the first UE to get allocated in the following slot. For
  // example, if {1,...,M,N} UEs are allocated, then the next UE should be M+1. We do this because the last UE might
  // have been allocated with a smaller grant size and we want to give it a chance to get a bigger grant size in the
  // next slot.
  auto& prev_full_grant_ue = dl_grants.size() > 1 ? dl_grants[dl_grants.size() - 2] : dl_grants[0];
  next_dl_ue_index         = to_du_ue_index(prev_full_grant_ue.context.ue_index + 1);
}

void scheduler_time_rr::save_ul_newtx_grants(span<const ul_sched_info> ul_grants)
{
  if (ul_grants.empty()) {
    return;
  }
  // \ref save_dl_newtx_grants
  auto& prev_full_grant_ue = ul_grants.size() > 1 ? ul_grants[ul_grants.size() - 2] : ul_grants[0];
  next_ul_ue_index         = to_du_ue_index(prev_full_grant_ue.context.ue_index + 1);
}
