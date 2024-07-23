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
#include "../support/pusch/pusch_td_resource_indices.h"
#include "srsran/srslog/srslog.h"

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
/// \param[in] ues Map of UEs belonging to a slice.
/// \param[in] is_dl Flag indicating whether the computation is DL or UL.
/// \param[in] res_grid View of the resource grid.
/// \param[in] expert_cfg Scheduler UE expert configuration.
/// \param[in] slice_max_rbs Maximum nof. RBs to allocate to a slice.
/// \return Maximum nof. RBs to allocate per UE per slot for newTx.
static unsigned compute_max_nof_rbs_per_ue_per_slot(const slice_ue_repository&        ues,
                                                    bool                              is_dl,
                                                    const ue_resource_grid_view&      res_grid,
                                                    const scheduler_ue_expert_config& expert_cfg,
                                                    unsigned                          slice_max_rbs)
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

  return (std::min(bwp_crb_limits.length(), slice_max_rbs) / nof_ues_to_be_scheduled_per_slot);
}

/// \brief Fetches list of DL HARQ candidates to schedule.
static static_vector<const dl_harq_process*, MAX_NOF_HARQS> get_ue_dl_harq_candidates(const ue&       ue_ref,
                                                                                      ue_cell_index_t cell_index,
                                                                                      bool            is_retx,
                                                                                      bool ue_with_srb_data_only,
                                                                                      srslog::basic_logger& logger)
{
  static_vector<const dl_harq_process*, MAX_NOF_HARQS> dl_harq_candidates;

  const ue_cell& ue_cc = ue_ref.get_cell(cell_index);
  if (ue_cc.is_in_fallback_mode()) {
    return dl_harq_candidates;
  }
  if (is_retx) {
    // Create list of DL HARQ processes with pending retx, sorted from oldest to newest.
    for (unsigned i = 0; i != ue_cc.harqs.nof_dl_harqs(); ++i) {
      const dl_harq_process& h = ue_cc.harqs.dl_harq(i);
      if (h.has_pending_retx() and not h.last_alloc_params().is_fallback and
          (not ue_with_srb_data_only or (h.last_alloc_params().tb[0]->contains_srb_data))) {
        dl_harq_candidates.push_back(&h);
      }
    }
    std::sort(dl_harq_candidates.begin(),
              dl_harq_candidates.end(),
              [](const dl_harq_process* lhs, const dl_harq_process* rhs) { return lhs->slot_ack() < rhs->slot_ack(); });
  } else if (ue_cc.is_active()) {
    // If there are no pending new Tx bytes or UE in fallback, return.
    if (not ue_ref.has_pending_dl_newtx_bytes()) {
      return dl_harq_candidates;
    }

    // Find empty HARQ. If any, add to the list.
    const dl_harq_process* h = ue_cc.harqs.find_empty_dl_harq();
    if (h != nullptr) {
      dl_harq_candidates.push_back(h);
    } else {
      // No empty HARQs are available. Log this occurrence.
      if (ue_cc.harqs.find_pending_dl_retx() != nullptr) {
        // HARQs are waiting for a grant for a retransmission.
        logger.debug("ue={} rnti={} PDSCH allocation skipped. Cause: No available HARQs for new transmissions.",
                     ue_cc.ue_index,
                     ue_cc.rnti());
      } else {
        // All HARQs are waiting for their respective HARQ-ACK. This may be a symptom of a long RTT for the PDSCH
        // and HARQ-ACK.
        logger.warning(
            "ue={} rnti={} PDSCH allocation skipped. Cause: All the HARQs are allocated and waiting for their "
            "respective HARQ-ACK. Check if any HARQ-ACK went missing in the lower layers or is arriving too late to "
            "the scheduler.",
            ue_cc.ue_index,
            ue_cc.rnti());
      }
    }
  }
  return dl_harq_candidates;
}

/// \brief Fetches list of UL HARQ candidates to schedule.
static static_vector<const ul_harq_process*, MAX_NOF_HARQS>
get_ue_ul_harq_candidates(const ue& ue_ref, ue_cell_index_t cell_index, bool is_retx, srslog::basic_logger& logger)
{
  static_vector<const ul_harq_process*, MAX_NOF_HARQS> ul_harq_candidates;

  const ue_cell& ue_cc = ue_ref.get_cell(cell_index);
  if (ue_cc.is_in_fallback_mode()) {
    return ul_harq_candidates;
  }
  if (is_retx) {
    // Create list of UL HARQ processes with pending retx, sorted from oldest to newest.
    for (unsigned i = 0; i != ue_cc.harqs.nof_ul_harqs(); ++i) {
      const ul_harq_process& h = ue_cc.harqs.ul_harq(i);
      if (h.has_pending_retx()) {
        ul_harq_candidates.push_back(&h);
      }
    }
    std::sort(ul_harq_candidates.begin(),
              ul_harq_candidates.end(),
              [](const ul_harq_process* lhs, const ul_harq_process* rhs) { return lhs->slot_ack() < rhs->slot_ack(); });
  } else if (ue_cc.is_active()) {
    // If there are no pending new Tx bytes, return.
    if (ue_ref.pending_ul_newtx_bytes() == 0) {
      return ul_harq_candidates;
    }

    // Find empty HARQ. If any, add to the list.
    const ul_harq_process* h = ue_cc.harqs.find_empty_ul_harq();
    if (h != nullptr) {
      ul_harq_candidates.push_back(h);
    } else {
      // No empty HARQs are available. Log this occurrence.
      if (ue_cc.harqs.find_pending_ul_retx() != nullptr) {
        // HARQs are waiting for a grant for a retransmission.
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
  }
  return ul_harq_candidates;
}

/// \brief Algorithm to select next UE to allocate in a time-domain RR fashion
/// \param[in] ue_db Map of UEs belonging to a slice.
/// \param[in] next_ue_index UE index with the highest priority to be allocated.
/// \param[in] alloc_ue callable with signature "bool(ue&)" that returns true if UE allocation was successful.
/// \return Index of next UE to allocate.
template <typename AllocUEFunc>
du_ue_index_t
round_robin_apply(const slice_ue_repository& ue_db, du_ue_index_t next_ue_index, const AllocUEFunc& alloc_ue)
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
    const ue&          u      = **it;
    const alloc_result result = alloc_ue(u);
    if (result.status == alloc_status::skip_slot) {
      // Grid allocator directed policy to stop allocations for this slot.
      break;
    }

    if (result.status == alloc_status::success and first_alloc) {
      // Mark the next UE to be the first UE to get allocated in the following slot.
      // It is important that we equally distribute the opportunity to be the first UE being allocated in a slot for
      // all UEs. Otherwise, we could end up in a situation, where a UE is always the last one to be allocated and
      // can only use the RBs that were left from the previous UE allocations.
      next_ue_index = to_du_ue_index(static_cast<unsigned>(u.ue_index) + 1U);
      first_alloc   = false;
    }
  }
  return next_ue_index;
}

/// Allocate UE PDSCH grant.
static alloc_result alloc_dl_ue(const ue&                         u,
                                const ue_resource_grid_view&      res_grid,
                                ue_pdsch_allocator&               pdsch_alloc,
                                bool                              is_retx,
                                bool                              ue_with_srb_data_only,
                                srslog::basic_logger&             logger,
                                const scheduler_ue_expert_config& ue_expert_cfg,
                                std::optional<unsigned>           dl_new_tx_max_nof_rbs_per_ue_per_slot = {})
{
  if (not is_retx) {
    if (ue_with_srb_data_only and not u.has_pending_dl_srb_newtx_bytes()) {
      return {alloc_status::skip_ue};
    }
    if (not u.has_pending_dl_newtx_bytes()) {
      return {alloc_status::skip_ue};
    }
  }

  // Prioritize PCell over SCells.
  for (unsigned i = 0; i != u.nof_cells(); ++i) {
    const ue_cell& ue_cc = u.get_cell(to_ue_cell_index(i));

    if (ue_cc.is_in_fallback_mode()) {
      // Skip allocation for UEs in fallback mode, as it is handled by the SRB fallback scheduler.
      return {alloc_status::skip_ue};
    }

    // UE is already allocated in the PDCCH for this slot (e.g. we should skip a newTx if a reTx has already been
    // allocated for this UE).
    if (res_grid.has_ue_dl_pdcch(ue_cc.cell_index, u.crnti)) {
      return {alloc_status::skip_ue};
    }

    // Get DL HARQ candidates.
    const auto harq_candidates =
        get_ue_dl_harq_candidates(u, to_ue_cell_index(i), is_retx, ue_with_srb_data_only, logger);
    if (harq_candidates.empty()) {
      // The conditions for a new PDSCH allocation for this UE were not met (e.g. lack of available HARQs).
      continue;
    }

    // Iterate through allocation parameter candidates.
    for (const dl_harq_process* h_dl : harq_candidates) {
      ue_pdsch_grant grant{&u, ue_cc.cell_index, h_dl->id};
      if (not is_retx) {
        grant.recommended_nof_bytes =
            ue_with_srb_data_only ? u.pending_dl_srb_newtx_bytes() : u.pending_dl_newtx_bytes();
        grant.max_nof_rbs = dl_new_tx_max_nof_rbs_per_ue_per_slot;
      }
      const alloc_result result = pdsch_alloc.allocate_dl_grant(grant);
      // If the allocation failed due to invalid parameters, we continue iteration.
      if (result.status != alloc_status::invalid_params) {
        return result;
      }
    }
  }
  return {alloc_status::skip_ue};
}

/// Allocate UE PUSCH grant.
static alloc_result alloc_ul_ue(const ue&                    u,
                                const ue_resource_grid_view& res_grid,
                                ue_pusch_allocator&          pusch_alloc,
                                bool                         is_retx,
                                bool                         schedule_sr_only,
                                bool                         ue_with_srb_data_only,
                                srslog::basic_logger&        logger,
                                std::optional<unsigned>      ul_new_tx_max_nof_rbs_per_ue_per_slot = {})
{
  unsigned pending_newtx_bytes     = 0;
  unsigned pending_srb_newtx_bytes = 0;
  if (not is_retx) {
    if (schedule_sr_only and not u.has_pending_sr()) {
      return {alloc_status::skip_ue};
    }
    // Fetch pending bytes of SRBs.
    pending_srb_newtx_bytes = u.pending_ul_srb_newtx_bytes();
    if (ue_with_srb_data_only and pending_srb_newtx_bytes == 0) {
      return {alloc_status::skip_ue};
    }
    pending_newtx_bytes = u.pending_ul_newtx_bytes();
    if (pending_newtx_bytes == 0) {
      return {alloc_status::skip_ue};
    }
  }

  // Prioritize PCell over SCells.
  for (unsigned i = 0; i != u.nof_cells(); ++i) {
    const ue_cell& ue_cc = u.get_cell(to_ue_cell_index(i));
    if (ue_cc.is_in_fallback_mode()) {
      // Skip allocation for UEs in fallback mode, as it is handled by the SRB fallback scheduler.
      return {alloc_status::skip_ue};
    }

    // Get UL HARQ candidates.
    const auto harq_candidates = get_ue_ul_harq_candidates(u, to_ue_cell_index(i), is_retx, logger);
    if (harq_candidates.empty()) {
      // The conditions for a new PUSCH allocation for this UE were not met (e.g. lack of available HARQs).
      continue;
    }

    // Iterate through allocation parameter candidates.
    for (const ul_harq_process* h_ul : harq_candidates) {
      ue_pusch_grant grant{&u, ue_cc.cell_index, h_ul->id};
      if (not is_retx) {
        grant.recommended_nof_bytes = ue_with_srb_data_only ? pending_srb_newtx_bytes : pending_newtx_bytes;
        grant.max_nof_rbs           = ul_new_tx_max_nof_rbs_per_ue_per_slot;
      }
      const alloc_result result = pusch_alloc.allocate_ul_grant(grant);
      // If the allocation failed due to invalid parameters, we continue iteration.
      if (result.status != alloc_status::invalid_params) {
        return result;
      }
    }
  }
  return {alloc_status::skip_ue};
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
                                 dl_ran_slice_candidate&      slice_candidate)
{
  const slice_ue_repository& ues     = slice_candidate.get_slice_ues();
  const unsigned             max_rbs = slice_candidate.remaining_rbs();

  if (ues.empty() or max_rbs == 0) {
    // No UEs to be scheduled or if there are no RBs to be scheduled in slice.
    return;
  }

  // First, schedule UEs with SRB data re-transmissions.
  auto srb_retx_ue_function = [this, &res_grid, &pdsch_alloc](const ue& u) {
    return alloc_dl_ue(u, res_grid, pdsch_alloc, true, true, logger, expert_cfg);
  };
  next_dl_ue_index = round_robin_apply(ues, next_dl_ue_index, srb_retx_ue_function);

  const unsigned dl_new_tx_max_nof_rbs_per_ue_per_slot =
      compute_max_nof_rbs_per_ue_per_slot(ues, true, res_grid, expert_cfg, max_rbs);
  if (dl_new_tx_max_nof_rbs_per_ue_per_slot > 0) {
    // Second, schedule UEs with SRB data new transmission.
    auto srb_newtx_ue_function = [this, &res_grid, &pdsch_alloc, dl_new_tx_max_nof_rbs_per_ue_per_slot](const ue& u) {
      return alloc_dl_ue(
          u, res_grid, pdsch_alloc, false, true, logger, expert_cfg, dl_new_tx_max_nof_rbs_per_ue_per_slot);
    };
    next_dl_ue_index = round_robin_apply(ues, next_dl_ue_index, srb_newtx_ue_function);
  }

  // Third, schedule UEs with DRB re-transmissions.
  auto drb_retx_ue_function = [this, &res_grid, &pdsch_alloc](const ue& u) {
    return alloc_dl_ue(u, res_grid, pdsch_alloc, true, false, logger, expert_cfg);
  };
  next_dl_ue_index = round_robin_apply(ues, next_dl_ue_index, drb_retx_ue_function);

  if (dl_new_tx_max_nof_rbs_per_ue_per_slot > 0) {
    // Then, schedule UEs with DRB new transmissions.
    auto drb_newtx_ue_function = [this, &res_grid, &pdsch_alloc, dl_new_tx_max_nof_rbs_per_ue_per_slot](const ue& u) {
      return alloc_dl_ue(
          u, res_grid, pdsch_alloc, false, false, logger, expert_cfg, dl_new_tx_max_nof_rbs_per_ue_per_slot);
    };
    next_dl_ue_index = round_robin_apply(ues, next_dl_ue_index, drb_newtx_ue_function);
  }
}

void scheduler_time_rr::ul_sched(ue_pusch_allocator&          pusch_alloc,
                                 const ue_resource_grid_view& res_grid,
                                 ul_ran_slice_candidate&      slice_candidate)
{
  const slice_ue_repository& ues     = slice_candidate.get_slice_ues();
  const unsigned             max_rbs = slice_candidate.remaining_rbs();

  if (ues.empty() or max_rbs == 0) {
    // No UEs to be scheduled or if there are no RBs to be scheduled in slice.
    return;
  }

  const unsigned ul_new_tx_max_nof_rbs_per_ue_per_slot =
      compute_max_nof_rbs_per_ue_per_slot(ues, false, res_grid, expert_cfg, max_rbs);
  // First, schedule UEs with pending SR.
  auto sr_ue_function = [this, &res_grid, &pusch_alloc, ul_new_tx_max_nof_rbs_per_ue_per_slot](const ue& u) {
    return alloc_ul_ue(u, res_grid, pusch_alloc, false, true, false, logger, ul_new_tx_max_nof_rbs_per_ue_per_slot);
  };
  next_ul_ue_index = round_robin_apply(ues, next_ul_ue_index, sr_ue_function);

  if (ul_new_tx_max_nof_rbs_per_ue_per_slot > 0) {
    // Second, schedule UEs with SRB data new transmissions.
    auto srb_newtx_ue_function = [this, &res_grid, &pusch_alloc, ul_new_tx_max_nof_rbs_per_ue_per_slot](const ue& u) {
      return alloc_ul_ue(u, res_grid, pusch_alloc, false, false, true, logger, ul_new_tx_max_nof_rbs_per_ue_per_slot);
    };
    next_ul_ue_index = round_robin_apply(ues, next_ul_ue_index, srb_newtx_ue_function);
  }

  // Third, schedule UEs with re-transmissions.
  auto data_retx_ue_function = [this, &res_grid, &pusch_alloc](const ue& u) {
    return alloc_ul_ue(u, res_grid, pusch_alloc, true, false, false, logger);
  };
  next_ul_ue_index = round_robin_apply(ues, next_ul_ue_index, data_retx_ue_function);

  // Then, schedule UEs with new transmissions.
  if (ul_new_tx_max_nof_rbs_per_ue_per_slot > 0) {
    auto data_tx_ue_function = [this, &res_grid, &pusch_alloc, ul_new_tx_max_nof_rbs_per_ue_per_slot](const ue& u) {
      return alloc_ul_ue(u, res_grid, pusch_alloc, false, false, false, logger, ul_new_tx_max_nof_rbs_per_ue_per_slot);
    };
    next_ul_ue_index = round_robin_apply(ues, next_ul_ue_index, data_tx_ue_function);
  }
}
