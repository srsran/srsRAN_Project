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
  // We perform round-robin by assigning priorities based on the difference between the current slot and the last slot
  // the UE has been allocated.
  for (ue_newtx_candidate& candidate : ue_candidates) {
    candidate.priority = dl_alloc_count - ue_last_dl_alloc_count[candidate.ue->ue_index()];
  }
}

void scheduler_time_rr::compute_ue_ul_priorities(slot_point               pdcch_slot,
                                                 slot_point               pusch_slot,
                                                 span<ue_newtx_candidate> ue_candidates)
{
  // \ref compute_ue_dl_priorities
  for (ue_newtx_candidate& candidate : ue_candidates) {
    candidate.priority = ul_alloc_count - ue_last_ul_alloc_count[candidate.ue->ue_index()];
  }
}

void scheduler_time_rr::save_dl_newtx_grants(span<const dl_msg_alloc> dl_grants)
{
  if (dl_grants.empty()) {
    return;
  }

  // Mark the count for the allocated UEs.
  for (const auto& grant : dl_grants) {
    ue_last_dl_alloc_count[grant.context.ue_index] = dl_alloc_count;
  }
  ++dl_alloc_count;
}

void scheduler_time_rr::save_ul_newtx_grants(span<const ul_sched_info> ul_grants)
{
  if (ul_grants.empty()) {
    return;
  }

  // Mark the count for the allocated UEs.
  for (const auto& grant : ul_grants) {
    ue_last_ul_alloc_count[grant.context.ue_index] = ul_alloc_count;
  }
  ++ul_alloc_count;
}
