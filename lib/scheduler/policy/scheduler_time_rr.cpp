/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "../support/config_helpers.h"
#include "../support/rb_helper.h"

using namespace srsran;

/// \brief Algorithm to select next UE to allocate in a time-domain RR fashion
/// \param[in] ue_db map of "slot_ue"
/// \param[in] next_ue_index UE index with the highest priority to be allocated.
/// \param[in] alloc_ue callable with signature "bool(ue&)" that returns true if UE allocation was successful.
/// \param[in] stop_cond callable with signature "bool()" that verifies if the conditions are present for the
/// round-robin to early-stop the iteration.
/// \return Index of next UE to allocate.
template <typename AllocUEFunc, typename StopIterationFunc>
du_ue_index_t round_robin_apply(const ue_repository&     ue_db,
                                du_ue_index_t            next_ue_index,
                                const AllocUEFunc&       alloc_ue,
                                const StopIterationFunc& stop_cond)
{
  if (ue_db.empty()) {
    return next_ue_index;
  }
  auto it          = ue_db.lower_bound(next_ue_index);
  bool first_alloc = true;
  for (unsigned count = 0; count < ue_db.size(); ++count, ++it) {
    if (it == ue_db.end()) {
      // wrap-around
      it = ue_db.begin();
    }
    const ue& u = **it;
    if (alloc_ue(u)) {
      if (first_alloc) {
        next_ue_index = to_du_ue_index((unsigned)u.ue_index + 1U);
        first_alloc   = false;
      }
      if (stop_cond()) {
        break;
      }
    }
  }
  return next_ue_index;
}

/// Allocate UE PDSCH grant.
static bool alloc_dl_ue(const ue&                    u,
                        const ue_resource_grid_view& res_grid,
                        ue_pdsch_allocator&          pdsch_alloc,
                        bool                         is_retx,
                        srslog::basic_logger&        logger)
{
  if (not is_retx and not u.has_pending_dl_newtx_bytes()) {
    return false;
  }
  const slot_point pdcch_slot = res_grid.get_pdcch_slot();

  // Prioritize PCell over SCells.
  for (unsigned i = 0; i != u.nof_cells(); ++i) {
    const ue_cell& ue_cc = u.get_cell(to_ue_cell_index(i));

    // Search available HARQ.
    const dl_harq_process* h = is_retx ? ue_cc.harqs.find_pending_dl_retx() : ue_cc.harqs.find_empty_dl_harq();
    if (h == nullptr) {
      if (not is_retx) {
        logger.warning(
            "ue={} rnti={:#x} PDSCH allocation skipped. Cause: No available HARQs. Check if any HARQ-ACK went missing"
            " or is arriving to the scheduler too late.",
            ue_cc.ue_index,
            ue_cc.rnti());
      }
      continue;
    }

    // Search for available symbolxRB resources in different SearchSpaces.
    optional<dci_dl_rnti_config_type> preferred_dci_rnti_type;
    if (is_retx) {
      preferred_dci_rnti_type = h->last_alloc_params().dci_cfg_type;
    }
    static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP> search_spaces =
        ue_cc.get_active_dl_search_spaces(preferred_dci_rnti_type);
    for (const search_space_info* ss : search_spaces) {
      if (ss->cfg->is_search_space0()) {
        continue;
      }

      for (unsigned time_res = 0; time_res != ss->pdsch_time_domain_list.size(); ++time_res) {
        const pdsch_time_domain_resource_allocation& pdsch = ss->pdsch_time_domain_list[time_res];
        if (not res_grid.get_cell_cfg_common(ue_cc.cell_index).is_dl_enabled(pdcch_slot + pdsch.k0)) {
          // DL needs to be active for PDSCH in this slot.
          continue;
        }
        // Check whether PDSCH time domain resource does not overlap with CORESET.
        if (pdsch.symbols.start() < ss->cfg->get_first_symbol_index() + ss->coreset->duration) {
          continue;
        }
        // Check whether PDSCH time domain resource fits in DL symbols of the slot.
        if (pdsch.symbols.stop() >
            res_grid.get_cell_cfg_common(ue_cc.cell_index).get_nof_dl_symbol_per_slot(pdcch_slot + pdsch.k0)) {
          // DL needs to be active for PDSCH in this slot.
          continue;
        }
        // If it is a retx, we need to ensure we use a time_domain_resource with the same number of symbols as used for
        // the first transmission.
        if (is_retx and pdsch.symbols.length() != h->last_alloc_params().nof_symbols) {
          continue;
        }

        const cell_slot_resource_grid& grid = res_grid.get_pdsch_grid(ue_cc.cell_index, pdsch.k0);
        const crb_bitmap               used_crbs =
            grid.used_crbs(ss->bwp->dl_common->generic_params.scs, ss->dl_crb_lims, pdsch.symbols);

        grant_prbs_mcs mcs_prbs = is_retx ? grant_prbs_mcs{h->last_alloc_params().tb.front().value().mcs,
                                                           h->last_alloc_params().rbs.type1().length()}
                                          : ue_cc.required_dl_prbs(pdsch, *ss, u.pending_dl_newtx_bytes());

        if (mcs_prbs.n_prbs == 0) {
          logger.debug("ue={} rnti={:#x} PDSCH allocation skipped. Cause: UE's CQI=0 ", ue_cc.ue_index, ue_cc.rnti());
          return false;
        }

        // [Implementation-defined] In case of partial slots and nof. PRBs allocated equals to 1 probability of KO is
        // high due to code not being able to cope with interference. So the solution is to increase the PRB allocation
        // to greater than 1 PRB.
        const auto& cell_cfg = res_grid.get_cell_cfg_common(ue_cc.cell_index);
        if (not cell_cfg.is_fully_dl_enabled(pdcch_slot + pdsch.k0) and mcs_prbs.n_prbs == 1) {
          mcs_prbs.n_prbs = 2;
        }

        const crb_interval ue_grant_crbs  = rb_helper::find_empty_interval_of_length(used_crbs, mcs_prbs.n_prbs, 0);
        bool               are_crbs_valid = not ue_grant_crbs.empty(); // Cannot be empty.
        if (is_retx) {
          // In case of Retx, the #CRBs need to stay the same.
          are_crbs_valid = ue_grant_crbs.length() == h->last_alloc_params().rbs.type1().length();
        }
        if (are_crbs_valid) {
          const bool res_allocated = pdsch_alloc.allocate_dl_grant(ue_pdsch_grant{&u,
                                                                                  ue_cc.cell_index,
                                                                                  h->id,
                                                                                  ss->cfg->get_id(),
                                                                                  time_res,
                                                                                  ue_grant_crbs,
                                                                                  ue_cc.get_aggregation_level(),
                                                                                  mcs_prbs.mcs});
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
static bool alloc_ul_ue(const ue&                    u,
                        const ue_resource_grid_view& res_grid,
                        ue_pusch_allocator&          pusch_alloc,
                        bool                         is_retx,
                        srslog::basic_logger&        logger)
{
  unsigned pending_newtx_bytes = 0;
  if (not is_retx) {
    pending_newtx_bytes = u.pending_ul_newtx_bytes();
    if (pending_newtx_bytes == 0) {
      return false;
    }
  }
  const slot_point pdcch_slot = res_grid.get_pdcch_slot();

  // Prioritize PCell over SCells.
  for (unsigned i = 0; i != u.nof_cells(); ++i) {
    const ue_cell&            ue_cc           = u.get_cell(to_ue_cell_index(i));
    const cell_configuration& cell_cfg_common = res_grid.get_cell_cfg_common(ue_cc.cell_index);
    const ul_harq_process*    h = is_retx ? ue_cc.harqs.find_pending_ul_retx() : ue_cc.harqs.find_empty_ul_harq();
    if (h == nullptr) {
      // No HARQs available.
      if (not is_retx) {
        logger.warning("ue={} rnti={:#x} PUSCH allocation skipped. Cause: No available HARQs. Check if any CRC PDU "
                       "went missing or is arriving to the scheduler too late.",
                       ue_cc.ue_index,
                       ue_cc.rnti());
      }
      continue;
    }

    optional<dci_ul_rnti_config_type> preferred_dci_rnti_type;
    if (is_retx) {
      preferred_dci_rnti_type = h->last_tx_params().dci_cfg_type;
    }
    static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP> search_spaces =
        ue_cc.get_active_ul_search_spaces(preferred_dci_rnti_type);
    for (const search_space_info* ss : search_spaces) {
      if (ss->cfg->is_search_space0()) {
        continue;
      }

      // - [Implementation-defined] k2 value which is less than or equal to minimum value of k1(s) is used.
      const unsigned                               time_res   = 0;
      const pusch_time_domain_resource_allocation& pusch_td   = ss->pusch_time_domain_list[time_res];
      const slot_point                             pusch_slot = pdcch_slot + pusch_td.k2;
      const unsigned                               start_ul_symbols =
          NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg_common.get_nof_ul_symbol_per_slot(pusch_slot);
      // If it is a retx, we need to ensure we use a time_domain_resource with the same number of symbols as used for
      // the first transmission.
      const bool sym_length_match_prev_grant_for_retx =
          is_retx ? pusch_td.symbols.length() != h->last_tx_params().nof_symbols : true;
      if (not cell_cfg_common.is_ul_enabled(pusch_slot) or pusch_td.symbols.start() < start_ul_symbols or
          !sym_length_match_prev_grant_for_retx) {
        // UL needs to be active for PUSCH in this slot.
        continue;
      }

      const cell_slot_resource_grid& grid = res_grid.get_pusch_grid(ue_cc.cell_index, pusch_td.k2);
      if (res_grid.has_ue_ul_grant(ue_cc.cell_index, ue_cc.rnti(), pusch_td.k2)) {
        // only one PUSCH per UE per slot.
        continue;
      }
      const prb_bitmap used_crbs =
          grid.used_crbs(ss->bwp->ul_common->generic_params.scs, ss->ul_crb_lims, pusch_td.symbols);

      // Compute the MCS and the number of PRBs, depending on the pending bytes to transmit.
      const grant_prbs_mcs mcs_prbs =
          is_retx ? grant_prbs_mcs{h->last_tx_params().mcs, h->last_tx_params().rbs.type1().length()}
                  : ue_cc.required_ul_prbs(pusch_td, pending_newtx_bytes, ss->get_crnti_ul_dci_format());

      // NOTE: this should never happen, but it's safe not to proceed if we get n_prbs == 0.
      if (mcs_prbs.n_prbs == 0) {
        logger.debug("ue={} rnti={:#x} PUSCH allocation skipped. Cause: MCS and PRBs computation resulted in no PRBs "
                     "allocated to this UE",
                     ue_cc.ue_index,
                     ue_cc.rnti());
        return false;
      }

      const crb_interval ue_grant_crbs  = rb_helper::find_empty_interval_of_length(used_crbs, mcs_prbs.n_prbs, 0);
      bool               are_crbs_valid = not ue_grant_crbs.empty(); // Cannot be empty.
      if (is_retx) {
        // In case of Retx, the #CRBs need to stay the same.
        are_crbs_valid = ue_grant_crbs.length() == h->last_tx_params().rbs.type1().length();
      }
      if (are_crbs_valid) {
        const bool res_allocated = pusch_alloc.allocate_ul_grant(ue_pusch_grant{&u,
                                                                                ue_cc.cell_index,
                                                                                h->id,
                                                                                ue_grant_crbs,
                                                                                pusch_td.symbols,
                                                                                time_res,
                                                                                ss->cfg->get_id(),
                                                                                ue_cc.get_aggregation_level(),
                                                                                mcs_prbs.mcs});
        if (res_allocated) {
          return true;
        }
      }
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

scheduler_time_rr::scheduler_time_rr() :
  logger(srslog::fetch_basic_logger("SCHED")),
  next_dl_ue_index(INVALID_DU_UE_INDEX),
  next_ul_ue_index(INVALID_DU_UE_INDEX)
{
}

void scheduler_time_rr::dl_sched(ue_pdsch_allocator&          pdsch_alloc,
                                 const ue_resource_grid_view& res_grid,
                                 const ue_repository&         ues,
                                 bool                         is_retx)
{
  auto tx_ue_function = [this, &res_grid, &pdsch_alloc, is_retx](const ue& u) {
    return alloc_dl_ue(u, res_grid, pdsch_alloc, is_retx, logger);
  };
  auto stop_iter = [&res_grid]() {
    // TODO: Account for different BWPs and cells.
    const du_cell_index_t cell_idx    = to_du_cell_index(0);
    const auto&           init_dl_bwp = res_grid.get_cell_cfg_common(cell_idx).dl_cfg_common.init_dl_bwp;
    // If all RBs are occupied, stop iteration.
    return res_grid.get_pdsch_grid(cell_idx, init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0)
        .used_crbs(init_dl_bwp.generic_params, init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].symbols)
        .all();
  };
  next_dl_ue_index = round_robin_apply(ues, next_dl_ue_index, tx_ue_function, stop_iter);
}

void scheduler_time_rr::ul_sched(ue_pusch_allocator&          pusch_alloc,
                                 const ue_resource_grid_view& res_grid,
                                 const ue_repository&         ues,
                                 bool                         is_retx)
{
  auto tx_ue_function = [this, &res_grid, &pusch_alloc, is_retx](const ue& u) {
    return alloc_ul_ue(u, res_grid, pusch_alloc, is_retx, logger);
  };
  auto stop_iter = [&res_grid]() {
    // TODO: Account for different BWPs and cells.
    const du_cell_index_t cell_idx    = to_du_cell_index(0);
    auto&                 init_ul_bwp = res_grid.get_cell_cfg_common(cell_idx).ul_cfg_common.init_ul_bwp;
    // If all RBs are occupied, stop iteration.
    return res_grid.get_pusch_grid(cell_idx, init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].k2)
        .used_crbs(init_ul_bwp.generic_params, init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].symbols)
        .all();
  };
  next_ul_ue_index = round_robin_apply(ues, next_ul_ue_index, tx_ue_function, stop_iter);
}
