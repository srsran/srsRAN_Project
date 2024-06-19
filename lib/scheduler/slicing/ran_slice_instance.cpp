/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

void ran_slice_instance::add_logical_channel(du_ue_index_t ue_idx, lcid_t lcid)
{
  if (not bearers.contains(ue_idx)) {
    bearers.emplace(ue_idx, MAX_NOF_RB_LCIDS);
  }
  bearers[ue_idx].set(lcid);
}

void ran_slice_instance::rem_logical_channel(du_ue_index_t ue_idx, lcid_t lcid)
{
  if (not bearers.contains(ue_idx)) {
    return;
  }
  bearers[ue_idx].reset(lcid);
  if (bearers[ue_idx].none()) {
    bearers.erase(ue_idx);
  }
}

void ran_slice_instance::rem_ue(du_ue_index_t ue_idx)
{
  bearers.erase(ue_idx);
}
