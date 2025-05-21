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

#include "ran_slice_instance.h"

using namespace srsran;

static unsigned get_min_k2(span<const pusch_time_domain_resource_allocation> pusch_td_res)
{
  return std::min_element(pusch_td_res.begin(),
                          pusch_td_res.end(),
                          [](const pusch_time_domain_resource_allocation& lhs,
                             const pusch_time_domain_resource_allocation& rhs) { return lhs.k2 < rhs.k2; })
      ->k2;
}

ran_slice_instance::ran_slice_instance(ran_slice_id_t                 id_,
                                       const cell_configuration&      cell_cfg_,
                                       const slice_rrm_policy_config& cfg_) :
  id(id_),
  cell_cfg(&cell_cfg_),
  cfg(cfg_),
  min_k2(get_min_k2(cell_cfg->ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list)),
  pusch_rb_count_per_slot(get_allocator_ring_size_gt_min(SCHEDULER_MAX_K2 + cell_cfg->ntn_cs_koffset)),
  slice_ues(id, cell_cfg->cell_index)
{
  std::fill(pusch_rb_count_per_slot.begin(), pusch_rb_count_per_slot.end(), 0);
}

void ran_slice_instance::slot_indication(slot_point slot_tx)
{
  static constexpr float exp_avg_coeff = 0.1;

  // Recompute rate of DL allocation for slice and clear PDSCH RB count in previous slot.
  avg_pdsch_rbs_per_slot += exp_avg_coeff * (pdsch_rb_count - avg_pdsch_rbs_per_slot);
  pdsch_rb_count = 0;

  // Recompute rate of UL allocation for slice and clear PUSCH RB count in previous slot.
  auto& pusch_slot_to_clear =
      pusch_rb_count_per_slot[(slot_tx + min_k2 - 1).to_uint() % pusch_rb_count_per_slot.size()];
  avg_pusch_rbs_per_slot += exp_avg_coeff * (pusch_slot_to_clear - avg_pusch_rbs_per_slot);
  pusch_slot_to_clear = 0;

  // Reset last alloc slot if the difference becomes too large, to avoid ambiguity.
  if (last_pdsch_alloc_slot.valid() and
      slot_tx > last_pdsch_alloc_slot + MAX_SLOTS_SINCE_LAST_PXSCH + cell_cfg->ntn_cs_koffset) {
    last_pdsch_alloc_slot.clear();
  }
  if (last_pusch_alloc_slot.valid() and
      slot_tx > last_pusch_alloc_slot + MAX_SLOTS_SINCE_LAST_PXSCH + cell_cfg->ntn_cs_koffset) {
    last_pusch_alloc_slot.clear();
  }
}
