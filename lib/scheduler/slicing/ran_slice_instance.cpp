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

void ran_slice_instance::slot_indication()
{
  pdsch_rb_count = 0;
  pusch_rb_count = 0;
}

void ran_slice_instance::rem_ue(du_ue_index_t ue_idx, lcid_t lcid)
{
  if (lcid < MAX_NOF_RB_LCIDS) {
    if (bearers.contains(ue_idx)) {
      bearers[ue_idx].reset(lcid);
      if (bearers[ue_idx].none()) {
        bearers.erase(ue_idx);
        if (ues.contains(ue_idx)) {
          ues.erase(ue_idx);
        }
      }
    }
    return;
  }
  if (bearers.contains(ue_idx)) {
    bearers.erase(ue_idx);
  }
  if (ues.contains(ue_idx)) {
    ues.erase(ue_idx);
  }
}

const slice_ue_repository& ran_slice_instance::get_ues()
{
  return ues;
}

void ran_slice_instance::add_ue(const ue* u, lcid_t lcid)
{
  if (not ues.contains(u->ue_index)) {
    ues.emplace(u->ue_index, u);
  }
  if (not bearers.contains(u->ue_index)) {
    bearers.emplace(u->ue_index, MAX_NOF_RB_LCIDS);
  }
  bearers[u->ue_index].set(lcid);
}
