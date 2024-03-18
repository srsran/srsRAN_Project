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

#include "scheduler_time_rr.h"
#include "../support/pdsch/pdsch_resource_allocation.h"
#include "../ue_scheduling/ue_pdsch_param_candidate_searcher.h"

using namespace srsran;

struct scheduler_alloc_limits {
  unsigned max_nof_ues_with_new_tx;
  unsigned nof_ues_to_be_scheduled_per_slot;
};

// [Implementation-defined] The following lookup of nof. UEs to be scheduled per slot is based on simple heuristic
// and to ensure multiple UEs are scheduled per slot rather than single UE hogging all the resource in a slot under
// full buffer scenario.
static const std::vector<scheduler_alloc_limits> scheduler_alloc_limits_lookup = {
    {3, 1},
    {7, 3},
    {15, 4},
    {32, 6},
    {MAX_NOF_DU_UES, 8},
};

/// \brief Computes maximum nof. RBs to allocate per UE per slot for newTx.
/// \param[in] ue_db map of ues.
/// \param[in] is_dl Flag indicating whether the computation is DL or UL.
/// \return Maximum nof. RBs to allocate per UE per slot for newTx.
static unsigned compute_max_nof_rbs_per_ue_per_slot(const ue_repository&              ues,
                                                    bool                              is_dl,
                                                    const ue_resource_grid_view&      res_grid,
                                                    const scheduler_ue_expert_config& expert_cfg)
{
  if (ues.empty()) {
    return 0;
  }

  unsigned nof_ue_with_new_tx               = 0;
  unsigned nof_ues_to_be_scheduled_per_slot = 0;

  for (const auto& u : ues) {
    if ((is_dl and u->has_pending_dl_newtx_bytes()) or (not is_dl and u->pending_ul_newtx_bytes() > 0)) {
      ++nof_ue_with_new_tx;
    }
  }

  // NOTE: All UEs use the same dedicated SearchSpace configuration.
  const ue&                    u      = **ues.begin();
  const ue_cell_configuration& ue_cfg = u.get_pcell().cfg();
  const auto*                  ss_info =
      ue_cfg.find_search_space(ue_cfg.cfg_dedicated().init_dl_bwp.pdcch_cfg->search_spaces.back().get_id());
  if (ss_info == nullptr) {
    return 0;
  }
  crb_interval bwp_crb_limits = {};
  if (is_dl) {
    bwp_crb_limits = pdsch_helper::get_ra_crb_limits(ss_info->get_dl_dci_format(),
                                                     ue_cfg.cell_cfg_common.dl_cfg_common.init_dl_bwp,
                                                     *ss_info->bwp->dl_common,
                                                     *ss_info->cfg,
                                                     *ss_info->coreset);
  } else {
    bwp_crb_limits = ss_info->ul_crb_lims;
  }

  // > Fetch nof. UEs to be scheduled per slot based on the lookup.
  for (const auto& lims : scheduler_alloc_limits_lookup) {
    if (nof_ue_with_new_tx <= lims.max_nof_ues_with_new_tx) {
      nof_ues_to_be_scheduled_per_slot = lims.nof_ues_to_be_scheduled_per_slot;
      break;
    }
  }

  // > Apply limits if passed to scheduler.
  if (is_dl) {
    nof_ues_to_be_scheduled_per_slot = std::min(expert_cfg.max_pdschs_per_slot, nof_ues_to_be_scheduled_per_slot);
  } else {
    nof_ues_to_be_scheduled_per_slot = std::min(expert_cfg.max_puschs_per_slot, nof_ues_to_be_scheduled_per_slot);
  }

  // > Compute maximum nof. PDCCH candidates allowed for each direction.
  // [Implementation-defined]
  // - Assume aggregation level 2 while computing nof. candidates that can be fit in CORESET.
  // - CORESET CCEs are divided by 2 to provide equal PDCCH resources to DL and UL.
  unsigned max_nof_candidates = (ss_info->coreset->get_nof_cces() / 2) / to_nof_cces(aggregation_level::n2);

  // > Subtract candidates used for DL retransmission since DL gets scheduled first over UL.
  // [Implementation-defined] Assumes that retransmissions use PDCCH candidates of aggregation level 2.
  if (is_dl) {
    max_nof_candidates -= (res_grid.get_dl_pdcch_sched_results(u.get_pcell().cell_index).size() > max_nof_candidates)
                              ? 0
                              : res_grid.get_dl_pdcch_sched_results(u.get_pcell().cell_index).size();
  }

  // > Ensure fairness in PDCCH allocation between DL and UL.
  // [Implementation-defined] To avoid running out of PDCCH candidates for UL allocation in multi-UE scenario and short
  // BW (e.g. TDD and 10Mhz BW), apply further limits on nof. UEs to be scheduled per slot.
  nof_ues_to_be_scheduled_per_slot = std::min(max_nof_candidates, nof_ues_to_be_scheduled_per_slot);

  if (nof_ues_to_be_scheduled_per_slot == 0) {
    // Skip allocation.
    return 0;
  }

  return (bwp_crb_limits.length() / nof_ues_to_be_scheduled_per_slot);
}

/// \brief Fetches a list of PUSCH Time Domain resource indexes based on cell, UE configuration and nof. symbols in
/// PUSCH slot.
/// \param[in] res_grid View of the current resource grid state to the PDSCH and PUSCH allocators.
/// \param[in] ues map of ues.
/// \return List of PUSCH Time Domain resource indexes.
static static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>
get_pusch_td_resource_indexes(const ue_resource_grid_view& res_grid, const ue_repository& ues)
{
  // Compute list of PUSCH time domain resource index list relevant for the PUSCH slot.
  static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS> pusch_td_res_index_list;

  // NOTE: All UEs use the same PUSCH Time Domain Resource configuration.
  const ue&                    ref_u  = **ues.begin();
  const ue_cell_configuration& ue_cfg = ref_u.get_pcell().cfg();
  const search_space_info*     ss_info =
      ue_cfg.find_search_space(ue_cfg.cfg_dedicated().init_dl_bwp.pdcch_cfg->search_spaces.back().get_id());
  if (ss_info != nullptr) {
    optional<unsigned> nof_full_ul_slots = nullopt;
    optional<unsigned> nof_full_dl_slots = nullopt;
    if (ue_cfg.cell_cfg_common.is_tdd()) {
      nof_full_ul_slots = nof_full_ul_slots_per_tdd_period(ue_cfg.cell_cfg_common.tdd_cfg_common.value());
      nof_full_dl_slots = nof_full_dl_slots_per_tdd_period(ue_cfg.cell_cfg_common.tdd_cfg_common.value());
    }
    const unsigned min_k1 = *std::min(ss_info->get_k1_candidates().begin(), ss_info->get_k1_candidates().end());
    // [Implementation-defined] It is assumed that UE is not configured pusch-TimeDomainAllocationList in
    // pusch-Config and configured only in pusch-ConfigCommon (SIB1).
    for (const pusch_time_domain_resource_allocation& pusch_td_res : ss_info->pusch_time_domain_list) {
      if (not ue_cfg.cell_cfg_common.is_tdd() or
          pusch_td_res.symbols.length() ==
              get_active_tdd_ul_symbols(
                  ue_cfg.cell_cfg_common.tdd_cfg_common.value(),
                  res_grid.get_pusch_slot(ref_u.get_pcell().cell_index, pusch_td_res.k2).slot_index(),
                  cyclic_prefix::NORMAL)
                  .length()) {
        if ((not ue_cfg.cell_cfg_common.is_tdd() or (*nof_full_dl_slots >= *nof_full_ul_slots)) and
            pusch_td_res.k2 <= min_k1) {
          // NOTE: Generated PUSCH time domain resources are sorted based on ascending order of k2 values and
          // descending order of nof. UL symbols for PUSCH.
          // [Implementation-defined] For DL heavy TDD configuration, only one entry in the PUSCH time domain
          // resources list with k2 value less than or equal to minimum value of k1(s) and, which matches nof. active
          // UL symbols in a slot is used.
          pusch_td_res_index_list.push_back(std::distance(ss_info->pusch_time_domain_list.begin(), &pusch_td_res));
          break;
        }
        if (ue_cfg.cell_cfg_common.is_tdd() and (*nof_full_ul_slots > *nof_full_dl_slots)) {
          // [Implementation-defined] For UL heavy TDD configuration multiple k2 values are considered for scheduling
          // since it allows multiple UL PDCCH allocations in the same slot for same UE but with different k2 values.
          pusch_td_res_index_list.push_back(std::distance(ss_info->pusch_time_domain_list.begin(), &pusch_td_res));
        }
      }
    }
  }
  return pusch_td_res_index_list;
}

/// \brief Algorithm to select next UE to allocate in a time-domain RR fashion
/// \param[in] ue_db map of "slot_ue"
/// \param[in] next_ue_index UE index with the highest priority to be allocated.
/// \param[in] alloc_ue callable with signature "bool(ue&)" that returns true if UE allocation was successful.
/// \return Index of next UE to allocate.
template <typename AllocUEFunc>
du_ue_index_t round_robin_apply(const ue_repository& ue_db, du_ue_index_t next_ue_index, const AllocUEFunc& alloc_ue)
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
    const ue&           u            = **it;
    const alloc_outcome alloc_result = alloc_ue(u);
    if (alloc_result == alloc_outcome::skip_slot) {
      // Grid allocator directed policy to stop allocations for this slot.
      break;
    }

    if (alloc_result == alloc_outcome::success and first_alloc) {
      // Mark the next UE to be the first UE to get allocated in the following slot.
      // It is important that we equally distribute the opportunity to be the first UE being allocated in a slot for
      // all UEs. Otherwise, we could end up in a situation, where a UE is always the last one to be allocated and
      // can only use the RBs that were left from the previous UE allocations.
      next_ue_index = to_du_ue_index((unsigned)u.ue_index + 1U);
      first_alloc   = false;
    }
  }
  return next_ue_index;
}

/// Allocate UE PDSCH grant.
static alloc_outcome alloc_dl_ue(const ue&                    u,
                                 const ue_resource_grid_view& res_grid,
                                 ue_pdsch_allocator&          pdsch_alloc,
                                 bool                         is_retx,
                                 bool                         ue_with_srb_data_only,
                                 srslog::basic_logger&        logger,
                                 optional<unsigned>           dl_new_tx_max_nof_rbs_per_ue_per_slot = {})
{
  if (not is_retx) {
    if (not u.has_pending_dl_newtx_bytes()) {
      return alloc_outcome::skip_ue;
    }
    if (ue_with_srb_data_only and not u.has_pending_dl_newtx_bytes(LCID_SRB1) and
        not u.has_pending_dl_newtx_bytes(LCID_SRB2)) {
      return alloc_outcome::skip_ue;
    }
  }

  // Prioritize PCell over SCells.
  for (unsigned i = 0; i != u.nof_cells(); ++i) {
    const ue_cell&   ue_cc      = u.get_cell(to_ue_cell_index(i));
    const slot_point pdcch_slot = res_grid.get_pdcch_slot(ue_cc.cell_index);

    if (ue_cc.is_in_fallback_mode()) {
      // Skip allocation for UEs in fallback mode, as it is handled by the SRB fallback scheduler.
      return alloc_outcome::skip_ue;
    }

    // UE is already allocated in the PDCCH for this slot (e.g. we should skip a newTx if a reTx has already been
    // allocated for this UE).
    if (res_grid.has_ue_dl_pdcch(ue_cc.cell_index, u.crnti)) {
      return alloc_outcome::skip_ue;
    }

    // Create PDSCH param candidate search object.
    ue_pdsch_param_candidate_searcher candidates{u, to_ue_cell_index(i), is_retx, pdcch_slot, logger};
    if (candidates.dl_harqs().empty()) {
      // The conditions for a new PDSCH allocation for this UE were not met (e.g. lack of available HARQs).
      continue;
    }

    // Iterate through allocation parameter candidates.
    for (const ue_pdsch_param_candidate_searcher::candidate& param_candidate : candidates) {
      const pdsch_time_domain_resource_allocation& pdsch    = param_candidate.pdsch_td_res();
      const search_space_info&                     ss       = param_candidate.ss();
      const dl_harq_process&                       h        = param_candidate.harq();
      const dci_dl_rnti_config_type                dci_type = param_candidate.dci_dl_rnti_cfg_type();
      const cell_slot_resource_grid&               grid     = res_grid.get_pdsch_grid(ue_cc.cell_index, pdsch.k0);
      const crb_bitmap used_crbs = grid.used_crbs(ss.bwp->dl_common->generic_params.scs, ss.dl_crb_lims, pdsch.symbols);
      if (used_crbs.all()) {
        logger.debug(
            "ue={} rnti={} PDSCH allocation skipped. Cause: No more RBs available", ue_cc.ue_index, ue_cc.rnti());
        return alloc_outcome::skip_slot;
      }
      grant_prbs_mcs mcs_prbs = is_retx ? grant_prbs_mcs{h.last_alloc_params().tb.front().value().mcs,
                                                         h.last_alloc_params().rbs.type1().length()}
                                        : ue_cc.required_dl_prbs(pdsch, u.pending_dl_newtx_bytes(), dci_type);
      // Limit the grant PRBs.
      if (not is_retx and dl_new_tx_max_nof_rbs_per_ue_per_slot.has_value()) {
        mcs_prbs.n_prbs = std::min(mcs_prbs.n_prbs, dl_new_tx_max_nof_rbs_per_ue_per_slot.value());
        // [Implementation-defined]
        // Check whether to allocate all remaining RBs or not. This is done to ensure we allocate only X nof. UEs for
        // which dl_new_tx_max_nof_rbs_per_ue_per_slot was computed. One way is by checking if the emtpy interval is
        // less than 2 times the required RBs. If so, allocate all remaining RBs. NOTE: This approach won't hold good in
        // case of low traffic scenario.
        const unsigned twice_grant_crbs_length =
            rb_helper::find_empty_interval_of_length(used_crbs, mcs_prbs.n_prbs * 2, 0).length();
        if (twice_grant_crbs_length < (mcs_prbs.n_prbs * 2)) {
          mcs_prbs.n_prbs = twice_grant_crbs_length;
        }
      }

      if (mcs_prbs.n_prbs == 0) {
        logger.debug("ue={} rnti={} PDSCH allocation skipped. Cause: UE's CQI=0 ", ue_cc.ue_index, ue_cc.rnti());
        return alloc_outcome::skip_ue;
      }

      // In case of retx, ensure the RI does not change.
      const unsigned nof_dl_layers =
          is_retx ? h.last_alloc_params().nof_layers : ue_cc.channel_state_manager().get_nof_dl_layers();

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
        are_crbs_valid = ue_grant_crbs.length() == h.last_alloc_params().rbs.type1().length();
      }
      if (are_crbs_valid) {
        const aggregation_level aggr_lvl =
            ue_cc.get_aggregation_level(ue_cc.channel_state_manager().get_wideband_cqi(), ss, true);
        const alloc_outcome result = pdsch_alloc.allocate_dl_grant(ue_pdsch_grant{&u,
                                                                                  ue_cc.cell_index,
                                                                                  h.id,
                                                                                  ss.cfg->get_id(),
                                                                                  param_candidate.pdsch_td_res_index(),
                                                                                  ue_grant_crbs,
                                                                                  aggr_lvl,
                                                                                  mcs_prbs.mcs,
                                                                                  nof_dl_layers});
        // If the allocation failed due to invalid parameters, we continue iteration.
        if (result != alloc_outcome::invalid_params) {
          return result;
        }
      }
    }
  }
  return alloc_outcome::skip_ue;
}

/// Allocate UE PUSCH grant.
static alloc_outcome alloc_ul_ue(const ue&                    u,
                                 const ue_resource_grid_view& res_grid,
                                 ue_pusch_allocator&          pusch_alloc,
                                 bool                         is_retx,
                                 bool                         schedule_sr_only,
                                 srslog::basic_logger&        logger,
                                 unsigned                     pusch_td_res_idx,
                                 optional<unsigned>           ul_new_tx_max_nof_rbs_per_ue_per_slot = {})
{
  unsigned pending_newtx_bytes = 0;
  if (not is_retx) {
    if (schedule_sr_only and not u.has_pending_sr()) {
      return alloc_outcome::skip_ue;
    }
    pending_newtx_bytes = u.pending_ul_newtx_bytes();
    if (pending_newtx_bytes == 0) {
      return alloc_outcome::skip_ue;
    }
  }

  // Prioritize PCell over SCells.
  for (unsigned i = 0; i != u.nof_cells(); ++i) {
    const ue_cell&   ue_cc      = u.get_cell(to_ue_cell_index(i));
    const slot_point pdcch_slot = res_grid.get_pdcch_slot(ue_cc.cell_index);

    const cell_configuration& cell_cfg_common = res_grid.get_cell_cfg_common(ue_cc.cell_index);
    const ul_harq_process*    h = is_retx ? ue_cc.harqs.find_pending_ul_retx() : ue_cc.harqs.find_empty_ul_harq();
    if (h == nullptr) {
      // No HARQs available.
      if (u.get_cell(to_ue_cell_index(i)).is_active() and not is_retx) {
        if (res_grid.has_ue_ul_pdcch(ue_cc.cell_index, u.crnti) or ue_cc.harqs.find_ul_harq_waiting_ack() == nullptr) {
          // A HARQ is already being retransmitted, or all HARQs are waiting for a grant for a retransmission.
          logger.debug("ue={} rnti={} PUSCH allocation skipped. Cause: No available HARQs for new transmissions.",
                       ue_cc.ue_index,
                       ue_cc.rnti());
        } else {
          // All HARQs are waiting for their respective CRC. This may be a symptom of a slow PUSCH processing chain.
          logger.warning("ue={} rnti={} PUSCH allocation skipped. Cause: All the UE HARQs are busy waiting for "
                         "their respective CRC result. Check if any CRC PDU went missing in the lower layers or is "
                         "arriving too late to the scheduler.",
                         ue_cc.ue_index,
                         ue_cc.rnti());
        }
      }
      continue;
    }

    optional<dci_ul_rnti_config_type> preferred_dci_rnti_type;
    if (is_retx) {
      preferred_dci_rnti_type = h->last_tx_params().dci_cfg_type;
    }
    static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP> search_spaces =
        ue_cc.get_active_ul_search_spaces(pdcch_slot, preferred_dci_rnti_type);
    for (const search_space_info* ss : search_spaces) {
      if (ss->cfg->is_search_space0()) {
        continue;
      }

      const pusch_time_domain_resource_allocation& pusch_td = ss->pusch_time_domain_list[pusch_td_res_idx];

      if (res_grid.has_ue_ul_grant(ue_cc.cell_index, ue_cc.rnti(), pusch_td.k2 + cell_cfg_common.ntn_cs_koffset)) {
        // Only one PUSCH per UE per slot.
        return alloc_outcome::skip_ue;
      }

      const slot_point pusch_slot = pdcch_slot + pusch_td.k2 + cell_cfg_common.ntn_cs_koffset;
      if (not cell_cfg_common.is_ul_enabled(pusch_slot)) {
        // Try next PUSCH time domain resource value.
        return alloc_outcome::invalid_params;
      }
      const unsigned start_ul_symbols =
          NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg_common.get_nof_ul_symbol_per_slot(pusch_slot);
      // If it is a retx, we need to ensure we use a time_domain_resource with the same number of symbols as used for
      // the first transmission.
      const bool sym_length_match_prev_grant_for_retx =
          is_retx ? pusch_td.symbols.length() != h->last_tx_params().nof_symbols : true;
      if (pusch_td.symbols.start() < start_ul_symbols or
          pusch_td.symbols.stop() > (start_ul_symbols + cell_cfg_common.get_nof_ul_symbol_per_slot(pusch_slot)) or
          !sym_length_match_prev_grant_for_retx) {
        // Try next PUSCH time domain resource value.
        return alloc_outcome::invalid_params;
      }

      const cell_slot_resource_grid& grid =
          res_grid.get_pusch_grid(ue_cc.cell_index, pusch_td.k2 + cell_cfg_common.ntn_cs_koffset);
      const prb_bitmap used_crbs =
          grid.used_crbs(ss->bwp->ul_common->generic_params.scs, ss->ul_crb_lims, pusch_td.symbols);
      if (used_crbs.all()) {
        logger.debug(
            "ue={} rnti={} PUSCH allocation skipped. Cause: No more RBs available", ue_cc.ue_index, ue_cc.rnti());
        return alloc_outcome::skip_slot;
      }

      // Compute the MCS and the number of PRBs, depending on the pending bytes to transmit.
      grant_prbs_mcs mcs_prbs = is_retx
                                    ? grant_prbs_mcs{h->last_tx_params().mcs, h->last_tx_params().rbs.type1().length()}
                                    : ue_cc.required_ul_prbs(pusch_td,
                                                             pending_newtx_bytes,
                                                             ss->get_ul_dci_format() == srsran::dci_ul_format::f0_0
                                                                 ? srsran::dci_ul_rnti_config_type::c_rnti_f0_0
                                                                 : srsran::dci_ul_rnti_config_type::c_rnti_f0_1);
      // Limit the grant PRBs.
      if (not is_retx and not schedule_sr_only and ul_new_tx_max_nof_rbs_per_ue_per_slot.has_value()) {
        mcs_prbs.n_prbs = std::min(mcs_prbs.n_prbs, ul_new_tx_max_nof_rbs_per_ue_per_slot.value());
        // [Implementation-defined]
        // Check whether it's the last UE to be scheduled in this slot i.e. if the emtpy interval is less than 2 times
        // the required RBs. If so, allocate all remaining RBs.
        const unsigned twice_grant_crbs_length =
            rb_helper::find_empty_interval_of_length(used_crbs, mcs_prbs.n_prbs * 2, 0).length();
        if (twice_grant_crbs_length < (mcs_prbs.n_prbs * 2)) {
          mcs_prbs.n_prbs = twice_grant_crbs_length;
        }
      }
      // NOTE: this should never happen, but it's safe not to proceed if we get n_prbs == 0.
      if (mcs_prbs.n_prbs == 0) {
        logger.debug("ue={} rnti={} PUSCH allocation skipped. Cause: MCS and PRBs computation resulted in no PRBs "
                     "allocated to this UE",
                     ue_cc.ue_index,
                     ue_cc.rnti());
        return alloc_outcome::skip_ue;
      }

      // Due to the pre-allocated UCI bits, MCS 0 and PRB 1 would not leave any space for the payload on the TBS, as
      // all the space would be taken by the UCI bits. As a result of this, the effective code rate would be 0 and the
      // allocation would fail and be postponed to the next slot.
      // [Implementation-defined] In our tests, we have seen that MCS 5 with 1 PRB can lead (depending on the
      // configuration) to a non-valid MCS-PRB allocation; therefore, we set 6 as minimum value for 1 PRB.
      // TODO: Remove this part and handle the problem with a loop that is general for any configuration.
      const sch_mcs_index min_mcs_for_1_prb  = static_cast<sch_mcs_index>(6U);
      const unsigned      min_allocable_prbs = 1U;
      if (mcs_prbs.mcs < min_mcs_for_1_prb and mcs_prbs.n_prbs <= min_allocable_prbs) {
        ++mcs_prbs.n_prbs;
      }

      const crb_interval ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, mcs_prbs.n_prbs, 0);
      // There must be at least one available CRB.
      bool are_crbs_valid = not ue_grant_crbs.empty();
      // For low MCS, we need to allocate more than min_allocable_prbs PRBs; else, the overhead due to UCI-on-PUSCH will
      // make the effective code-rate exceed 0.95.
      // TODO: Remove this part and handle the problem with a loop that is general for any configuration.
      if (ue_grant_crbs.length() <= min_allocable_prbs and mcs_prbs.mcs < min_mcs_for_1_prb) {
        logger.debug("ue={} rnti={} PUSCH allocation skipped. Cause: the scheduler couldn't allocate the min. "
                     "number of PRBs={} for MCS={}",
                     ue_cc.ue_index,
                     ue_cc.rnti(),
                     mcs_prbs.n_prbs,
                     mcs_prbs.mcs.to_uint());
        return alloc_outcome::skip_ue;
      }

      if (is_retx) {
        // In case of Retx, the #CRBs need to stay the same.
        are_crbs_valid = ue_grant_crbs.length() == h->last_tx_params().rbs.type1().length();
      }
      if (are_crbs_valid) {
        const aggregation_level aggr_lvl =
            ue_cc.get_aggregation_level(ue_cc.channel_state_manager().get_wideband_cqi(), *ss, false);
        const alloc_outcome result = pusch_alloc.allocate_ul_grant(ue_pusch_grant{&u,
                                                                                  ue_cc.cell_index,
                                                                                  h->id,
                                                                                  ue_grant_crbs,
                                                                                  pusch_td.symbols,
                                                                                  pusch_td_res_idx,
                                                                                  ss->cfg->get_id(),
                                                                                  aggr_lvl,
                                                                                  mcs_prbs.mcs});
        // If the allocation failed due to invalid parameters, we continue the iteration.
        if (result != alloc_outcome::invalid_params) {
          return result;
        }
      }
    }
  }
  return alloc_outcome::skip_ue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

scheduler_time_rr::scheduler_time_rr(const scheduler_ue_expert_config& expert_cfg_) :
  logger(srslog::fetch_basic_logger("SCHED")),
  next_dl_ue_index(INVALID_DU_UE_INDEX),
  next_ul_ue_index(INVALID_DU_UE_INDEX),
  expert_cfg(expert_cfg_)
{
}

void scheduler_time_rr::dl_sched(ue_pdsch_allocator&          pdsch_alloc,
                                 const ue_resource_grid_view& res_grid,
                                 const ue_repository&         ues)
{
  // First schedule re-transmissions.
  auto retx_ue_function = [this, &res_grid, &pdsch_alloc](const ue& u) {
    return alloc_dl_ue(u, res_grid, pdsch_alloc, true, false, logger);
  };
  next_dl_ue_index = round_robin_apply(ues, next_dl_ue_index, retx_ue_function);

  const unsigned dl_new_tx_max_nof_rbs_per_ue_per_slot =
      compute_max_nof_rbs_per_ue_per_slot(ues, true, res_grid, expert_cfg);
  if (dl_new_tx_max_nof_rbs_per_ue_per_slot > 0) {
    // Second, schedule UEs with SRB data.
    auto srb_newtx_ue_function = [this, &res_grid, &pdsch_alloc, dl_new_tx_max_nof_rbs_per_ue_per_slot](const ue& u) {
      return alloc_dl_ue(u, res_grid, pdsch_alloc, false, true, logger, dl_new_tx_max_nof_rbs_per_ue_per_slot);
    };
    next_dl_ue_index = round_robin_apply(ues, next_dl_ue_index, srb_newtx_ue_function);

    // Then, schedule new transmissions.
    auto tx_ue_function = [this, &res_grid, &pdsch_alloc, dl_new_tx_max_nof_rbs_per_ue_per_slot](const ue& u) {
      return alloc_dl_ue(u, res_grid, pdsch_alloc, false, false, logger, dl_new_tx_max_nof_rbs_per_ue_per_slot);
    };
    next_dl_ue_index = round_robin_apply(ues, next_dl_ue_index, tx_ue_function);
  }
}

void scheduler_time_rr::ul_sched(ue_pusch_allocator&          pusch_alloc,
                                 const ue_resource_grid_view& res_grid,
                                 const ue_repository&         ues)
{
  if (ues.empty()) {
    // No UEs to be scheduled.
    return;
  }

  // Fetch applicable PUSCH Time Domain resource index list.
  static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS> pusch_td_res_index_list =
      get_pusch_td_resource_indexes(res_grid, ues);

  // First schedule UL data re-transmissions.
  for (unsigned pusch_td_res_idx : pusch_td_res_index_list) {
    auto data_retx_ue_function = [this, &res_grid, &pusch_alloc, pusch_td_res_idx](const ue& u) {
      return alloc_ul_ue(u, res_grid, pusch_alloc, true, false, logger, pusch_td_res_idx);
    };
    next_ul_ue_index = round_robin_apply(ues, next_ul_ue_index, data_retx_ue_function);
  }

  // Then, schedule all pending SR.
  for (unsigned pusch_td_res_idx : pusch_td_res_index_list) {
    auto sr_ue_function = [this, &res_grid, &pusch_alloc, pusch_td_res_idx](const ue& u) {
      return alloc_ul_ue(u, res_grid, pusch_alloc, false, true, logger, pusch_td_res_idx);
    };
    next_ul_ue_index = round_robin_apply(ues, next_ul_ue_index, sr_ue_function);
  }

  // Finally, schedule UL data new transmissions.
  const unsigned ul_new_tx_max_nof_rbs_per_ue_per_slot =
      compute_max_nof_rbs_per_ue_per_slot(ues, false, res_grid, expert_cfg);
  if (ul_new_tx_max_nof_rbs_per_ue_per_slot > 0) {
    for (unsigned pusch_td_res_idx : pusch_td_res_index_list) {
      auto data_tx_ue_function = [this,
                                  &res_grid,
                                  &pusch_alloc,
                                  ul_new_tx_max_nof_rbs_per_ue_per_slot,
                                  pusch_td_res_idx](const ue& u) {
        return alloc_ul_ue(
            u, res_grid, pusch_alloc, false, false, logger, pusch_td_res_idx, ul_new_tx_max_nof_rbs_per_ue_per_slot);
      };
      next_ul_ue_index = round_robin_apply(ues, next_ul_ue_index, data_tx_ue_function);
    }
  }
}
