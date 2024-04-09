/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pusch_td_resource_indices.h"
#include "../../cell_scheduler.h"
#include "../../policy/scheduler_policy.h"

using namespace srsran;

static static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>
compute_pusch_td_resource_indices(span<const pusch_time_domain_resource_allocation> pusch_time_domain_list,
                                  const cell_configuration&                         cell_cfg,
                                  slot_point                                        pdcch_slot,
                                  unsigned                                          min_k1)
{
  // Compute list of PUSCH time domain resource index list relevant for the PUSCH slot.
  static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS> pusch_td_res_index_list;

  optional<unsigned> nof_full_ul_slots = nullopt;
  optional<unsigned> nof_full_dl_slots = nullopt;
  if (cell_cfg.is_tdd()) {
    nof_full_ul_slots = nof_full_ul_slots_per_tdd_period(cell_cfg.tdd_cfg_common.value());
    nof_full_dl_slots = nof_full_dl_slots_per_tdd_period(cell_cfg.tdd_cfg_common.value());
  }

  for (const pusch_time_domain_resource_allocation& pusch_td_res : pusch_time_domain_list) {
    if (not cell_cfg.is_tdd() or
        pusch_td_res.symbols.length() == get_active_tdd_ul_symbols(cell_cfg.tdd_cfg_common.value(),
                                                                   (pdcch_slot + pusch_td_res.k2).slot_index(),
                                                                   cyclic_prefix::NORMAL)
                                             .length()) {
      if ((not cell_cfg.is_tdd() or (*nof_full_dl_slots >= *nof_full_ul_slots)) and pusch_td_res.k2 <= min_k1) {
        // NOTE: Generated PUSCH time domain resources are sorted based on ascending order of k2 values and
        // descending order of nof. UL symbols for PUSCH.
        // [Implementation-defined] For DL heavy TDD configuration, only one entry in the PUSCH time domain
        // resources list with k2 value less than or equal to minimum value of k1(s) and, which matches nof. active
        // UL symbols in a slot is used.
        pusch_td_res_index_list.push_back(std::distance(pusch_time_domain_list.begin(), &pusch_td_res));
        break;
      }
      if (cell_cfg.is_tdd() and (*nof_full_ul_slots > *nof_full_dl_slots)) {
        // [Implementation-defined] For UL heavy TDD configuration multiple k2 values are considered for scheduling
        // since it allows multiple UL PDCCH allocations in the same slot for same UE but with different k2 values.
        pusch_td_res_index_list.push_back(std::distance(pusch_time_domain_list.begin(), &pusch_td_res));
      }
    }
  }

  return pusch_td_res_index_list;
}

static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>
srsran::get_pusch_td_resource_indices(const ue_resource_grid_view& res_grid, const ue_repository& ues)
{
  // NOTE: All UEs use the same PUSCH Time Domain Resource configuration.
  const ue&                    ref_u    = **ues.begin();
  const ue_cell_configuration& ue_cfg   = ref_u.get_pcell().cfg();
  const cell_configuration&    cell_cfg = ue_cfg.cell_cfg_common;
  const search_space_info*     ss_info =
      ue_cfg.find_search_space(ue_cfg.cfg_dedicated().init_dl_bwp.pdcch_cfg->search_spaces.back().get_id());
  if (ss_info == nullptr) {
    return {};
  }
  const unsigned min_k1 = *std::min(ss_info->get_k1_candidates().begin(), ss_info->get_k1_candidates().end());
  return compute_pusch_td_resource_indices(
      cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list,
      cell_cfg,
      res_grid.get_pdcch_slot(ref_u.get_pcell().cell_index),
      min_k1);
}

static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>
srsran::get_pusch_td_res_indices_common(const cell_configuration& cell_cfg, slot_point pdcch_slot)
{
  // Compute list of PUSCH time domain resource index list relevant for the PUSCH slot.
  static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS> pusch_td_res_index_list;
  const unsigned                                                        min_k1 = 4U;
  return compute_pusch_td_resource_indices(
      cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list, cell_cfg, pdcch_slot, min_k1);
}
