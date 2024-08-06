/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ran_slice_instance.h"

using namespace srsran;

ran_slice_instance::ran_slice_instance(ran_slice_id_t                 id_,
                                       const cell_configuration&      cell_cfg_,
                                       const slice_rrm_policy_config& cfg_) :
  id(id_), cell_cfg(&cell_cfg_), cfg(cfg_)
{
}

void ran_slice_instance::slot_indication(const ue_repository& cell_ues)
{
  pdsch_rb_count = 0;
  pusch_rb_count = 0;
  // Remove UEs from slice UEs list if it's removed from UE repository.
  auto* ue_to_rem_it = slice_ues_to_rem.begin();
  while (ue_to_rem_it != slice_ues_to_rem.end()) {
    if (not cell_ues.contains(*ue_to_rem_it)) {
      slice_ues.erase(*ue_to_rem_it);
      ue_to_rem_it = slice_ues_to_rem.erase(ue_to_rem_it);
      continue;
    }
    ++ue_to_rem_it;
  }
}

void ran_slice_instance::rem_logical_channel(du_ue_index_t ue_idx, lcid_t lcid)
{
  if (not slice_ues.contains(ue_idx)) {
    return;
  }
  if (lcid < MAX_NOF_RB_LCIDS) {
    slice_ues[ue_idx].rem_logical_channel(lcid);
    if (not slice_ues[ue_idx].has_bearers_in_slice()) {
      slice_ues_to_rem.push_back(ue_idx);
    }
    return;
  }
  // Disable all DRBs.
  for (unsigned lcid_to_rem = LCID_MIN_DRB; lcid_to_rem < MAX_NOF_RB_LCIDS; ++lcid_to_rem) {
    if (slice_ues[ue_idx].contains(uint_to_lcid(lcid_to_rem))) {
      slice_ues[ue_idx].rem_logical_channel(uint_to_lcid(lcid_to_rem));
    }
  }
  slice_ues_to_rem.push_back(ue_idx);
}

const slice_ue_repository& ran_slice_instance::get_ues()
{
  return slice_ues;
}

void ran_slice_instance::add_logical_channel(const ue& u, lcid_t lcid, lcg_id_t lcg_id)
{
  if (not slice_ues.contains(u.ue_index)) {
    slice_ues.emplace(u.ue_index, u);
  }
  slice_ues[u.ue_index].add_logical_channel(lcid, lcg_id);
}
