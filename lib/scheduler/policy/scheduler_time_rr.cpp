/*
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_time_rr.h"
#include "../support/config_helpers.h"

using namespace srsgnb;

/// \brief Algorithm to select next UE to allocate in a time-domain RR fashion
/// \param[in] ue_db map of "slot_ue"
/// \param[in] rr_count starting index to select next UE
/// \param[in] p callable with signature "bool(slot_ue&)" that returns true if UE allocation was successful
/// \return true if a UE was allocated
template <typename Predicate>
bool round_robin_apply(const ue_list& ue_db, uint32_t rr_count, const Predicate& p)
{
  if (ue_db.empty()) {
    return false;
  }
  auto next_it = ue_db.lower_bound(to_du_ue_index(rr_count % MAX_NOF_DU_UES));
  for (uint32_t count = 0; count < ue_db.size(); ++count, ++next_it) {
    if (next_it == ue_db.end()) {
      // wrap-around
      next_it = ue_db.begin();
    }
    if (p(*next_it)) {
      return true;
    }
  }
  return false;
}

/// \brief Gets SearchSpace configurations prioritized based on nof. candidates for a given aggregation level in a UE
/// cell.
/// \param[in] ue_cc UE's cell context.
/// \param[in] agg_lvl Aggregation level.
/// \return List of SearchSpace configuration.
static static_vector<const search_space_configuration*, MAX_NOF_SEARCH_SPACE_PER_BWP>
get_ue_cell_prioritized_ss_for_agg_lvl(const ue_cell& ue_cc, aggregation_level agg_lvl)
{
  auto search_spaces = ue_cc.cfg().get_search_spaces(ue_cc.active_bwp_id());
  std::sort(search_spaces.begin(),
            search_spaces.end(),
            [agg_lvl](const search_space_configuration* lhs, const search_space_configuration* rhs) -> bool {
              if (lhs->nof_candidates[to_aggregation_level_index(agg_lvl)] ==
                  rhs->nof_candidates[to_aggregation_level_index(agg_lvl)]) {
                // In case nof. candidates are equal, choose the SS with higher CORESET Id (i.e. try to use CORESET#0 as
                // less as possible).
                return lhs->cs_id > rhs->cs_id;
              }
              return lhs->nof_candidates[to_aggregation_level_index(agg_lvl)] >
                     rhs->nof_candidates[to_aggregation_level_index(agg_lvl)];
            });
  return search_spaces;
}

/// Allocate UE PDSCH grant.
static bool
alloc_dl_ue(const ue& u, const ue_resource_grid_view& res_grid, ue_pdsch_allocator& pdsch_alloc, bool is_retx)
{
  if (not is_retx and not u.has_pending_dl_newtx_bytes()) {
    return false;
  }
  // TODO: Set aggregation level based on link quality.
  const aggregation_level agg_lvl    = srsgnb::aggregation_level::n4;
  slot_point              pdcch_slot = res_grid.get_pdcch_slot();

  // Prioritize PCell over SCells.
  for (unsigned i = 0; i != u.nof_cells(); ++i) {
    const ue_cell& ue_cc = u.get_cell(to_ue_cell_index(i));
    if (not res_grid.get_cell_cfg_common(ue_cc.cell_index).is_dl_enabled(pdcch_slot)) {
      // DL needs to be active for PDCCH in this slot.
      continue;
    }

    // Search available HARQ.
    const dl_harq_process* h = is_retx ? ue_cc.harqs.find_pending_dl_retx() : ue_cc.harqs.find_empty_dl_harq();
    if (h == nullptr) {
      continue;
    }

    // Search for available symbolxRB resources in different SearchSpaces.
    const cell_configuration& cell_cfg_cmn = ue_cc.cfg().cell_cfg_common;
    for (const search_space_configuration* ss_cfg : get_ue_cell_prioritized_ss_for_agg_lvl(ue_cc, agg_lvl)) {
      const span<const pdsch_time_domain_resource_allocation> pdsch_list =
          ue_cc.cfg().get_pdsch_time_domain_list(ss_cfg->id);

      bwp_configuration bwp_cfg = ue_cc.cfg().dl_bwp_common(ue_cc.active_bwp_id()).generic_params;
      if (ss_cfg->type == search_space_configuration::type_t::common) {
        // See TS 38.214, 5.1.2.2.2, Downlink resource allocation type 1.
        bwp_cfg = ue_cc.cfg().dl_bwp_common(to_bwp_id(0)).generic_params;
        if (cell_cfg_cmn.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value()) {
          bwp_cfg.crbs = get_coreset0_crbs(cell_cfg_cmn.dl_cfg_common.init_dl_bwp.pdcch_common);
        }
      }

      for (unsigned time_res = 0; time_res != pdsch_list.size(); ++time_res) {
        const pdsch_time_domain_resource_allocation& pdsch = pdsch_list[time_res];
        if (not res_grid.get_cell_cfg_common(ue_cc.cell_index).is_dl_enabled(pdcch_slot + pdsch.k0)) {
          // DL needs to be active for PDSCH in this slot.
          continue;
        }
        const cell_slot_resource_grid& grid           = res_grid.get_pdsch_grid(ue_cc.cell_index, pdsch.k0);
        const prb_bitmap               used_crbs      = grid.used_crbs(bwp_cfg, pdsch.symbols);
        const unsigned                 nof_req_prbs   = is_retx ? h->last_alloc_params().prbs.prbs().length()
                                                                : ue_cc.required_dl_prbs(time_res, u.pending_dl_newtx_bytes());
        const crb_interval             ue_grant_crbs  = find_empty_interval_of_length(used_crbs, nof_req_prbs, 0);
        bool                           are_crbs_valid = not ue_grant_crbs.empty(); // Cannot be empty.
        if (is_retx) {
          // In case of Retx, the #CRBs need to stay the same.
          are_crbs_valid = ue_grant_crbs.length() == h->last_alloc_params().prbs.prbs().length();
        }
        if (are_crbs_valid) {
          const bool res_allocated = pdsch_alloc.allocate_dl_grant(ue_pdsch_grant{
              &u, ue_cc.cell_index, h->id, ss_cfg->id, time_res, ue_grant_crbs, dci_dl_format::f1_0, agg_lvl});
          if (res_allocated) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

/// Allocate UE PUSCH grant.
static bool
alloc_ul_ue(const ue& u, const ue_resource_grid_view& res_grid, ue_pusch_allocator& pusch_alloc, bool is_retx)
{
  unsigned pending_newtx_bytes = 0;
  if (not is_retx) {
    pending_newtx_bytes = u.pending_ul_newtx_bytes();
    if (pending_newtx_bytes == 0) {
      return false;
    }
  }
  // TODO: Set aggregation level based on link quality.
  const aggregation_level agg_lvl    = srsgnb::aggregation_level::n4;
  slot_point              pdcch_slot = res_grid.get_pdcch_slot();

  // Prioritize PCell over SCells.
  for (unsigned i = 0; i != u.nof_cells(); ++i) {
    const ue_cell&               ue_cc       = u.get_cell(to_ue_cell_index(i));
    const ue_cell_configuration& ue_cell_cfg = ue_cc.cfg();
    if (not res_grid.get_cell_cfg_common(ue_cc.cell_index).is_dl_enabled(res_grid.get_pdcch_slot())) {
      // DL needs to be active for PDCCH in this slot.
      continue;
    }

    const ul_harq_process* h = nullptr;
    if (is_retx) {
      h = ue_cc.harqs.find_pending_ul_retx();
    } else {
      h = ue_cc.harqs.find_empty_ul_harq();
    }
    if (h == nullptr) {
      continue;
    }

    for (const search_space_configuration* ss_cfg : get_ue_cell_prioritized_ss_for_agg_lvl(ue_cc, agg_lvl)) {
      const span<const pusch_time_domain_resource_allocation> pusch_list =
          ue_cc.cfg().get_pusch_time_domain_list(ss_cfg->id);
      // See TS 38.212, 7.3.1.0 DCI size alignment.
      const bwp_uplink_common& bwp_ul = ue_cell_cfg.ul_bwp_common(
          ss_cfg->type == search_space_configuration::type_t::common ? to_bwp_id(0) : ue_cc.active_bwp_id());

      for (unsigned time_res = 0; time_res != pusch_list.size(); ++time_res) {
        const unsigned k2 = pusch_list[time_res].k2;
        if (not res_grid.get_cell_cfg_common(ue_cc.cell_index).is_ul_enabled(pdcch_slot + k2)) {
          // UL needs to be active for PUSCH in this slot.
          continue;
        }
        const ofdm_symbol_range        pusch_symbols = bwp_ul.pusch_cfg_common->pusch_td_alloc_list[time_res].symbols;
        const cell_slot_resource_grid& grid          = res_grid.get_pusch_grid(ue_cc.cell_index, k2);
        const prb_bitmap               used_crbs     = grid.used_crbs(bwp_ul.generic_params, pusch_symbols);
        const unsigned                 nof_req_prbs =
            is_retx ? h->last_tx_params().prbs.prbs().length() : ue_cc.required_ul_prbs(time_res, pending_newtx_bytes);
        const crb_interval ue_grant_crbs  = find_empty_interval_of_length(used_crbs, nof_req_prbs, 0);
        bool               are_crbs_valid = not ue_grant_crbs.empty(); // Cannot be empty.
        if (is_retx) {
          // In case of Retx, the #CRBs need to stay the same.
          are_crbs_valid = ue_grant_crbs.length() == h->last_tx_params().prbs.prbs().length();
        }
        if (are_crbs_valid) {
          const bool res_allocated = pusch_alloc.allocate_ul_grant(
              ue_pusch_grant{&u, ue_cc.cell_index, h->id, ue_grant_crbs, pusch_symbols, time_res, ss_cfg->id, agg_lvl});
          if (res_allocated) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void scheduler_time_rr::dl_sched(ue_pdsch_allocator&          pdsch_alloc,
                                 const ue_resource_grid_view& res_grid,
                                 const ue_list&               ues,
                                 bool                         is_retx)
{
  if (not is_retx) {
    // Increment Round-robin counter to prioritize different UEs.
    rr_count++;
  }

  auto tx_ue_function = [&res_grid, &pdsch_alloc, is_retx](const ue& u) {
    return alloc_dl_ue(u, res_grid, pdsch_alloc, is_retx);
  };
  if (round_robin_apply(ues, rr_count, tx_ue_function)) {
    return;
  }
}

void scheduler_time_rr::ul_sched(ue_pusch_allocator&          pusch_alloc,
                                 const ue_resource_grid_view& res_grid,
                                 const ue_list&               ues,
                                 bool                         is_retx)
{
  if (not is_retx) {
    // Increment Round-robin counter to prioritize different UEs.
    rr_count++;
  }

  auto tx_ue_function = [&res_grid, &pusch_alloc, is_retx](const ue& u) {
    return alloc_ul_ue(u, res_grid, pusch_alloc, is_retx);
  };
  if (round_robin_apply(ues, rr_count, tx_ue_function)) {
    return;
  }
}
