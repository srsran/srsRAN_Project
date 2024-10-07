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

namespace {

struct scheduler_alloc_limits {
  unsigned max_nof_ues_with_new_tx;
  unsigned nof_ues_to_be_scheduled_per_slot;
};

} // namespace

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

static unsigned get_max_ues_to_be_sched(const slice_ue_repository& ues, bool is_dl)
{
  unsigned nof_ue_with_new_tx = 0;
  unsigned lookup_idx         = 0;
  for (const auto& u : ues) {
    if ((is_dl and u.has_pending_dl_newtx_bytes()) or (not is_dl and u.pending_ul_newtx_bytes() > 0)) {
      // count UEs with pending data.
      ++nof_ue_with_new_tx;

      // check if we surpassed a limit of the lookup table.
      if (nof_ue_with_new_tx > scheduler_alloc_limits_lookup[lookup_idx].max_nof_ues_with_new_tx) {
        lookup_idx++;
        if (lookup_idx == scheduler_alloc_limits_lookup.size() - 1) {
          // no need to continue the search as we reached the maximum position of the lookup.
          break;
        }
      }
    }
  }
  if (nof_ue_with_new_tx == 0) {
    return 0;
  }
  return scheduler_alloc_limits_lookup[lookup_idx].nof_ues_to_be_scheduled_per_slot;
}

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
  unsigned nof_ues_to_be_scheduled_per_slot = get_max_ues_to_be_sched(ues, is_dl);
  if (nof_ues_to_be_scheduled_per_slot == 0) {
    return 0;
  }

  // > Apply limits if passed to scheduler.
  if (is_dl) {
    nof_ues_to_be_scheduled_per_slot = std::min(expert_cfg.max_pdschs_per_slot, nof_ues_to_be_scheduled_per_slot);
  } else {
    nof_ues_to_be_scheduled_per_slot = std::min(expert_cfg.max_puschs_per_slot, nof_ues_to_be_scheduled_per_slot);
  }

  // NOTE: All UEs use the same dedicated SearchSpace configuration.
  const slice_ue&              u      = *ues.begin();
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

static bool can_allocate_dl_newtx(const slice_ue& ue_ref, ue_cell_index_t cell_index, srslog::basic_logger& logger)
{
  const ue_cell& ue_cc = ue_ref.get_cell(cell_index);
  if (not ue_cc.harqs.has_empty_dl_harqs()) {
    // No empty HARQs are available. Log this occurrence.
    if (ue_cc.harqs.find_pending_dl_retx().has_value()) {
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
    return false;
  }
  return true;
}

static bool can_allocate_ul_newtx(const slice_ue& ue_ref, ue_cell_index_t cell_index, srslog::basic_logger& logger)
{
  const ue_cell& ue_cc = ue_ref.get_cell(cell_index);
  if (not ue_cc.harqs.has_empty_ul_harqs()) {
    // No empty HARQs are available. Log this occurrence.
    if (ue_cc.harqs.find_pending_ul_retx().has_value()) {
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
    return false;
  }
  return true;
}

/// \brief Algorithm to select next UE to allocate in a time-domain RR fashion
/// \param[in] ue_db Map of UEs belonging to a slice.
/// \param[in] next_ue_index UE index with the highest priority to be allocated.
/// \param[in] alloc_ue callable with signature "bool(ue&)" that returns true if UE allocation was successful.
/// \return Index of next UE to allocate and the allocation status.
template <typename AllocUEFunc>
static std::pair<du_ue_index_t, alloc_status>
round_robin_apply(const slice_ue_repository& ue_db, du_ue_index_t next_ue_index, const AllocUEFunc& alloc_ue)
{
  if (ue_db.empty()) {
    return std::make_pair(next_ue_index, alloc_status::skip_slot);
  }
  auto it          = ue_db.lower_bound(next_ue_index);
  bool first_alloc = true;
  for (unsigned count = 0; count < ue_db.size(); ++count, ++it) {
    if (it == ue_db.end()) {
      // wrap-around
      it = ue_db.begin();
    }
    const slice_ue&    u      = *it;
    const alloc_result result = alloc_ue(u);
    if (result.status == alloc_status::skip_slot) {
      // Grid allocator directed policy to stop allocations for this slot.
      return std::make_pair(next_ue_index, result.status);
    }

    if (result.status == alloc_status::success and first_alloc) {
      // Mark the next UE to be the first UE to get allocated in the following slot.
      // It is important that we equally distribute the opportunity to be the first UE being allocated in a slot for
      // all UEs. Otherwise, we could end up in a situation, where a UE is always the last one to be allocated and
      // can only use the RBs that were left from the previous UE allocations.
      next_ue_index = to_du_ue_index(static_cast<unsigned>(u.ue_index()) + 1U);
      first_alloc   = false;
    }
  }
  return std::make_pair(next_ue_index, alloc_status::success);
}

/// Allocates UE PDSCH grant for retransmissions.
static alloc_result alloc_dl_retxs(const slice_ue_repository&   ue_db,
                                   const ue_resource_grid_view& res_grid,
                                   ue_pdsch_allocator&          pdsch_alloc,
                                   ran_slice_id_t               slice_id,
                                   dl_harq_pending_retx_list    harq_list)
{
  for (auto it = harq_list.begin(); it != harq_list.end();) {
    // Note: During retx alloc, the pending HARQ list will mutate. So, we prefetch the next node.
    auto prev_it = it++;
    auto h       = *prev_it;
    if (h.get_grant_params().slice_id != slice_id or not ue_db.contains(h.ue_index())) {
      continue;
    }
    const slice_ue& u = ue_db[h.ue_index()];
    // Prioritize PCell over SCells.
    for (unsigned i = 0; i != u.nof_cells(); ++i) {
      const ue_cell& ue_cc = u.get_cell(to_ue_cell_index(i));
      srsran_assert(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(),
                    "policy scheduler called for UE={} in fallback",
                    ue_cc.ue_index);

      // [Implementation-defined] Skip UE if PDCCH is already allocated for this UE in this slot.
      if (res_grid.has_ue_dl_pdcch(ue_cc.cell_index, u.crnti())) {
        continue;
      }

      ue_pdsch_grant     grant{&u, ue_cc.cell_index, h.id()};
      const alloc_result result = pdsch_alloc.allocate_dl_grant(grant);
      // Continue iteration until skip slot indication is received.
      // NOTE: Allocation status other than skip_slot can be ignored because allocation of reTxs is done from oldest
      // HARQ pending to newest. Hence, other allocation status are redundant.
      if (result.status == alloc_status::skip_slot) {
        return result;
      }
    }
  }
  // Return successful outcome in all other cases.
  // Other cases:
  //  - No pending HARQs to allocate.
  //  - At the end of pending HARQs iteration.
  return {alloc_status::success};
}

/// Allocate UE PDSCH grant for new transmissions.
static alloc_result alloc_dl_ue_newtx(const slice_ue&              u,
                                      const ue_resource_grid_view& res_grid,
                                      ue_pdsch_allocator&          pdsch_alloc,
                                      srslog::basic_logger&        logger,
                                      std::optional<unsigned>      dl_new_tx_max_nof_rbs_per_ue_per_slot = {})
{
  if (not u.has_pending_dl_newtx_bytes()) {
    return {alloc_status::skip_ue};
  }

  // Prioritize PCell over SCells.
  for (unsigned i = 0; i != u.nof_cells(); ++i) {
    const ue_cell& ue_cc = u.get_cell(to_ue_cell_index(i));
    srsran_assert(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(),
                  "policy scheduler called for UE={} in fallback",
                  ue_cc.ue_index);

    // UE is already allocated in the PDCCH for this slot (e.g. we should skip a newTx if a reTx has already been
    // allocated for this UE).
    if (res_grid.has_ue_dl_pdcch(ue_cc.cell_index, u.crnti())) {
      return {alloc_status::skip_ue};
    }

    if (can_allocate_dl_newtx(u, to_ue_cell_index(i), logger)) {
      ue_pdsch_grant grant{
          &u, ue_cc.cell_index, INVALID_HARQ_ID, u.pending_dl_newtx_bytes(), dl_new_tx_max_nof_rbs_per_ue_per_slot};
      const alloc_result result = pdsch_alloc.allocate_dl_grant(grant);
      // If the allocation failed due to invalid parameters, we continue iteration.
      if (result.status != alloc_status::invalid_params) {
        return result;
      }
    }
  }
  return {alloc_status::skip_ue};
}

/// Allocates UE PUSCH grant for retransmissions.
static alloc_result alloc_ul_retxs(const slice_ue_repository& ue_db,
                                   ue_pusch_allocator&        pusch_alloc,
                                   ran_slice_id_t             slice_id,
                                   ul_harq_pending_retx_list  harq_list)
{
  for (auto it = harq_list.begin(); it != harq_list.end();) {
    // Note: During retx alloc, the pending HARQ list will mutate. So, we prefetch the next node.
    auto prev_it = it++;
    auto h       = *prev_it;
    if (h.get_grant_params().slice_id != slice_id or not ue_db.contains(h.ue_index())) {
      continue;
    }
    const slice_ue& u = ue_db[h.ue_index()];
    // Prioritize PCell over SCells.
    for (unsigned i = 0; i != u.nof_cells(); ++i) {
      const ue_cell& ue_cc = u.get_cell(to_ue_cell_index(i));
      srsran_assert(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(),
                    "policy scheduler called for UE={} in fallback",
                    ue_cc.ue_index);

      ue_pusch_grant     grant{&u, ue_cc.cell_index, h.id()};
      const alloc_result result = pusch_alloc.allocate_ul_grant(grant);
      // Continue iteration until skip slot indication is received.
      // NOTE: Allocation status other than skip_slot can be ignored because allocation of reTxs is done from oldest
      // HARQ pending to newest. Hence, other allocation status are redundant.
      if (result.status == alloc_status::skip_slot) {
        return result;
      }
    }
  }
  // Return successful outcome in all other cases.
  // Other cases:
  //  - No pending HARQs to allocate.
  //  - At the end of pending HARQs iteration.
  return {alloc_status::success};
}

/// Allocate UE PUSCH grant for new transmissions.
static alloc_result alloc_ul_ue_newtx(const slice_ue&         u,
                                      ue_pusch_allocator&     pusch_alloc,
                                      srslog::basic_logger&   logger,
                                      std::optional<unsigned> ul_new_tx_max_nof_rbs_per_ue_per_slot = {})
{
  unsigned pending_newtx_bytes = 0;
  pending_newtx_bytes          = u.pending_ul_newtx_bytes();
  if (pending_newtx_bytes == 0) {
    return {alloc_status::skip_ue};
  }

  // Prioritize PCell over SCells.
  for (unsigned i = 0; i != u.nof_cells(); ++i) {
    const ue_cell& ue_cc = u.get_cell(to_ue_cell_index(i));
    srsran_assert(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(),
                  "policy scheduler called for UE={} in fallback",
                  ue_cc.ue_index);

    if (can_allocate_ul_newtx(u, to_ue_cell_index(i), logger)) {
      ue_pusch_grant grant{
          &u, ue_cc.cell_index, INVALID_HARQ_ID, pending_newtx_bytes, ul_new_tx_max_nof_rbs_per_ue_per_slot};
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
                                 dl_ran_slice_candidate&      slice_candidate,
                                 dl_harq_pending_retx_list    harq_pending_retx_list)
{
  const slice_ue_repository& ues      = slice_candidate.get_slice_ues();
  const unsigned             max_rbs  = slice_candidate.remaining_rbs();
  const ran_slice_id_t       slice_id = slice_candidate.id();

  if (ues.empty() or max_rbs == 0) {
    // No UEs to be scheduled or if there are no RBs to be scheduled in slice.
    return;
  }

  // First, schedule UEs with re-transmissions.
  auto retx_result = alloc_dl_retxs(ues, res_grid, pdsch_alloc, slice_id, harq_pending_retx_list);
  if (retx_result.status == alloc_status::skip_slot) {
    return;
  }

  const unsigned dl_new_tx_max_nof_rbs_per_ue_per_slot =
      compute_max_nof_rbs_per_ue_per_slot(ues, true, res_grid, expert_cfg, max_rbs);
  if (dl_new_tx_max_nof_rbs_per_ue_per_slot > 0) {
    // Then, schedule UEs with new transmissions.
    auto drb_newtx_ue_function =
        [this, &res_grid, &pdsch_alloc, dl_new_tx_max_nof_rbs_per_ue_per_slot](const slice_ue& u) {
          return alloc_dl_ue_newtx(u, res_grid, pdsch_alloc, logger, dl_new_tx_max_nof_rbs_per_ue_per_slot);
        };
    auto result      = round_robin_apply(ues, next_dl_ue_index, drb_newtx_ue_function);
    next_dl_ue_index = result.first;
  }
}

void scheduler_time_rr::ul_sched(ue_pusch_allocator&          pusch_alloc,
                                 const ue_resource_grid_view& res_grid,
                                 ul_ran_slice_candidate&      slice_candidate,
                                 ul_harq_pending_retx_list    harq_pending_retx_list)
{
  const slice_ue_repository& ues      = slice_candidate.get_slice_ues();
  const unsigned             max_rbs  = slice_candidate.remaining_rbs();
  const ran_slice_id_t       slice_id = slice_candidate.id();

  if (ues.empty() or max_rbs == 0) {
    // No UEs to be scheduled or if there are no RBs to be scheduled in slice.
    return;
  }

  // First, schedule UEs with re-transmissions.
  auto retx_result = alloc_ul_retxs(ues, pusch_alloc, slice_id, harq_pending_retx_list);
  if (retx_result.status == alloc_status::skip_slot) {
    return;
  }

  // Then, schedule UEs with new transmissions.
  const unsigned ul_new_tx_max_nof_rbs_per_ue_per_slot =
      compute_max_nof_rbs_per_ue_per_slot(ues, false, res_grid, expert_cfg, max_rbs);
  if (ul_new_tx_max_nof_rbs_per_ue_per_slot > 0) {
    auto data_tx_ue_function = [this, &pusch_alloc, ul_new_tx_max_nof_rbs_per_ue_per_slot](const slice_ue& u) {
      return alloc_ul_ue_newtx(u, pusch_alloc, logger, ul_new_tx_max_nof_rbs_per_ue_per_slot);
    };
    auto result      = round_robin_apply(ues, next_ul_ue_index, data_tx_ue_function);
    next_ul_ue_index = result.first;
  }
}
