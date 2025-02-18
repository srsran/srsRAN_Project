/*
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

static unsigned get_max_ues_to_be_sched(const slice_ue_repository& ues, bool is_dl, unsigned max_ues)
{
  if (max_ues == 0) {
    return 0;
  }
  unsigned nof_ue_with_new_tx = 0;
  unsigned lookup_idx         = 0;
  for (const auto& u : ues) {
    if ((is_dl and u.has_pending_dl_newtx_bytes()) or (not is_dl and u.pending_ul_newtx_bytes() > 0)) {
      // count UEs with pending data.
      ++nof_ue_with_new_tx;

      // check if we surpassed a limit of the lookup table.
      if (nof_ue_with_new_tx > scheduler_alloc_limits_lookup[lookup_idx].max_nof_ues_with_new_tx) {
        lookup_idx++;
        if (lookup_idx == scheduler_alloc_limits_lookup.size() - 1 or
            scheduler_alloc_limits_lookup[lookup_idx].nof_ues_to_be_scheduled_per_slot >= max_ues) {
          // no need to continue the search as we reached the maximum number of ues to be scheduled in the slot.
          break;
        }
      }
    }
  }
  if (nof_ue_with_new_tx == 0) {
    return 0;
  }
  return std::min(scheduler_alloc_limits_lookup[lookup_idx].nof_ues_to_be_scheduled_per_slot, max_ues);
}

/// \brief Computes maximum nof. RBs to allocate per UE per slot for newTx.
/// \param[in] expert_cfg Scheduler UE expert configuration.
/// \param[in] ctxt Scheduling context
/// \return Maximum nof. RBs to allocate per UE per slot for newTx.
template <typename SchedContext>
static unsigned compute_max_nof_rbs_per_ue_per_slot(const scheduler_ue_expert_config& expert_cfg, SchedContext& ctxt)
{
  const auto& ues = ctxt.get_slice().get_slice_ues();
  if (ues.empty()) {
    return 0;
  }

  bool     is_dl;
  unsigned nof_ues_to_be_scheduled_per_slot;
  unsigned nof_pxsch_already_alloc;
  unsigned nof_pdcchs_alloc;
  if constexpr (std::is_same_v<std::decay_t<SchedContext>, slice_dl_sched_context>) {
    is_dl                            = true;
    nof_ues_to_be_scheduled_per_slot = expert_cfg.max_pdschs_per_slot;
    nof_pxsch_already_alloc          = ctxt.scheduled_pdschs().size();
    nof_pdcchs_alloc                 = ctxt.scheduled_dl_pdcchs().size();
  } else {
    is_dl                            = false;
    nof_ues_to_be_scheduled_per_slot = expert_cfg.max_puschs_per_slot;
    nof_pxsch_already_alloc          = ctxt.scheduled_puschs().size();
    nof_pdcchs_alloc                 = ctxt.scheduled_ul_pdcchs().size();
  }
  nof_ues_to_be_scheduled_per_slot -= std::min(nof_ues_to_be_scheduled_per_slot, nof_pxsch_already_alloc);

  // > Compute number of UEs to allocate in slot, given the number of UEs with pending data.
  nof_ues_to_be_scheduled_per_slot = get_max_ues_to_be_sched(ues, is_dl, nof_ues_to_be_scheduled_per_slot);
  if (nof_ues_to_be_scheduled_per_slot == 0) {
    return 0;
  }

  // NOTE: All UEs use the same dedicated SearchSpace configuration.
  const slice_ue&              u      = *ues.begin();
  const ue_cell_configuration& ue_cfg = u.get_pcell().cfg();
  const auto*                  ss_info =
      ue_cfg.find_search_space(ue_cfg.bwp(to_bwp_id(0)).dl_ded.value()->pdcch_cfg->search_spaces.back().get_id());
  if (ss_info == nullptr) {
    return 0;
  }
  crb_interval bwp_crb_limits = {};
  if (is_dl) {
    bwp_crb_limits = pdsch_helper::get_ra_crb_limits(ss_info->get_dl_dci_format(),
                                                     ue_cfg.cell_cfg_common.dl_cfg_common.init_dl_bwp,
                                                     *ss_info->bwp->dl_common.value(),
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
    max_nof_candidates -= (nof_pdcchs_alloc > max_nof_candidates) ? 0 : nof_pdcchs_alloc;
  }

  // > Ensure fairness in PDCCH allocation between DL and UL.
  // [Implementation-defined] To avoid running out of PDCCH candidates for UL allocation in multi-UE scenario and short
  // BW (e.g. TDD and 10Mhz BW), apply further limits on nof. UEs to be scheduled per slot.
  nof_ues_to_be_scheduled_per_slot = std::min(max_nof_candidates, nof_ues_to_be_scheduled_per_slot);

  if (nof_ues_to_be_scheduled_per_slot == 0) {
    // Skip allocation.
    return 0;
  }

  return (std::min(bwp_crb_limits.length(), ctxt.get_slice().remaining_rbs()) / nof_ues_to_be_scheduled_per_slot);
}

static bool can_allocate_dl_newtx(slice_dl_sched_context& ctxt, const ue_cell& ue_cc, srslog::basic_logger& logger)
{
  srsran_assert(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(),
                "policy scheduler called for UE={} in fallback",
                fmt::underlying(ue_cc.ue_index));

  if (ctxt.has_ue_dl_grant(ue_cc.rnti()) or not ue_cc.is_pdcch_enabled(ctxt.pdcch_slot()) or
      not ue_cc.is_pdsch_enabled(ctxt.pdsch_slot())) {
    // UE is either already allocated for this slot (e.g. a reTx already took place) or it is not active.
    return false;
  }

  if (not ue_cc.harqs.has_empty_dl_harqs()) {
    // No empty HARQs are available. Log this occurrence.
    if (ue_cc.harqs.find_pending_dl_retx().has_value()) {
      // HARQs are waiting for a grant for a retransmission.
      logger.debug("ue={} rnti={} PDSCH allocation skipped. Cause: No available HARQs for new transmissions.",
                   fmt::underlying(ue_cc.ue_index),
                   ue_cc.rnti());
    } else {
      // All HARQs are waiting for their respective HARQ-ACK. This may be a symptom of a long RTT for the PDSCH
      // and HARQ-ACK.
      logger.warning(
          "ue={} rnti={} PDSCH allocation skipped. Cause: All the HARQs are allocated and waiting for their "
          "respective HARQ-ACK. Check if any HARQ-ACK went missing in the lower layers or is arriving too late to "
          "the scheduler.",
          fmt::underlying(ue_cc.ue_index),
          ue_cc.rnti());
    }
    return false;
  }
  return true;
}

static bool can_allocate_ul_newtx(const slice_ue&       ue_ref,
                                  const ue_cell&        ue_cc,
                                  slot_point            pdcch_slot,
                                  slot_point            pusch_slot,
                                  srslog::basic_logger& logger)
{
  srsran_assert(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(),
                "policy scheduler called for UE={} in fallback",
                fmt::underlying(ue_cc.ue_index));

  if (not ue_cc.is_pdcch_enabled(pdcch_slot) or not ue_cc.is_ul_enabled(pusch_slot)) {
    // Either the PDCCH slot or PUSCH slots are not available.
    return false;
  }

  if (not ue_cc.harqs.has_empty_ul_harqs()) {
    // No empty HARQs are available. Log this occurrence.
    if (ue_cc.harqs.find_pending_ul_retx().has_value()) {
      // HARQs are waiting for a grant for a retransmission.
      logger.debug("ue={} rnti={} PUSCH allocation skipped. Cause: No available HARQs for new transmissions.",
                   fmt::underlying(ue_cc.ue_index),
                   ue_cc.rnti());
    } else {
      // All HARQs are waiting for their respective CRC. This may be a symptom of a slow PUSCH processing chain.
      logger.warning("ue={} rnti={} PUSCH allocation skipped. Cause: All the UE HARQs are busy waiting for "
                     "their respective CRC result. Check if any CRC PDU went missing in the lower layers or is "
                     "arriving too late to the scheduler.",
                     fmt::underlying(ue_cc.ue_index),
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
round_robin_apply(const slice_ue_repository& ue_db, du_ue_index_t next_ue_index, AllocUEFunc& alloc_ue)
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
    const slice_ue& u      = *it;
    const auto      result = alloc_ue(u);
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
static dl_alloc_result alloc_dl_retxs(slice_dl_sched_context& ctxt)
{
  auto pending_harqs = ctxt.get_harqs_with_pending_retx();
  for (auto it = pending_harqs.begin(); it != pending_harqs.end();) {
    // Note: During retx alloc, the pending HARQ list will mutate. So, we prefetch the next node.
    auto prev_it = it++;
    auto h       = *prev_it;
    if (h.get_grant_params().slice_id != ctxt.slice_id()) {
      continue;
    }
    const slice_ue& u     = ctxt.get_slice().get_slice_ues()[h.ue_index()];
    const ue_cell&  ue_cc = *u.find_cell(ctxt.cell_index());

    // [Implementation-defined] Skip UE if PDSCH is already allocated for this UE in this slot.
    if (ctxt.has_ue_dl_grant(u.crnti()) or not ue_cc.is_pdcch_enabled(ctxt.pdcch_slot())) {
      continue;
    }

    ue_pdsch_grant  grant{&u, ue_cc.cell_index, h.id()};
    dl_alloc_result result = ctxt.allocate_dl_grant(grant);
    // Continue iteration until skip slot indication is received.
    // NOTE: Allocation status other than skip_slot can be ignored because allocation of reTxs is done from oldest
    // HARQ pending to newest. Hence, other allocation status are redundant.
    if (result.status == alloc_status::skip_slot) {
      return result;
    }
  }

  // Return successful outcome in all other cases.
  // Other cases:
  //  - No pending HARQs to allocate.
  //  - At the end of pending HARQs iteration.
  return {alloc_status::success};
}

/// Allocate UE PDSCH grant for new transmissions.
static dl_alloc_result alloc_dl_ue_newtx(const slice_ue&         u,
                                         slice_dl_sched_context& ctxt,
                                         srslog::basic_logger&   logger,
                                         std::optional<unsigned> max_pdsch_rbs = {})
{
  if (not u.has_pending_dl_newtx_bytes()) {
    return {alloc_status::skip_ue};
  }
  const ue_cell& ue_cc = *u.find_cell(ctxt.cell_index());

  if (can_allocate_dl_newtx(ctxt, ue_cc, logger)) {
    ue_pdsch_grant        grant{&u, ue_cc.cell_index, INVALID_HARQ_ID, u.pending_dl_newtx_bytes(), max_pdsch_rbs};
    const dl_alloc_result result = ctxt.allocate_dl_grant(grant);
    // If the allocation failed due to invalid parameters, we continue iteration.
    if (result.status != alloc_status::invalid_params) {
      return result;
    }
  }

  return {alloc_status::skip_ue};
}

/// Allocates UE PUSCH grant for retransmissions.
static ul_alloc_result alloc_ul_retxs(const slice_ue_repository& ue_db, slice_ul_sched_context& ctxt)
{
  auto pending_harqs = ctxt.get_harqs_with_pending_retx();
  for (auto it = pending_harqs.begin(); it != pending_harqs.end();) {
    // Note: During retx alloc, the pending HARQ list will mutate. So, we prefetch the next node.
    auto prev_it = it++;
    auto h       = *prev_it;
    if (h.get_grant_params().slice_id != ctxt.slice_id()) {
      continue;
    }
    const slice_ue& u     = ctxt.get_slice().get_slice_ues()[h.ue_index()];
    const ue_cell&  ue_cc = *u.find_cell(ctxt.cell_index());

    if (not ue_cc.is_pdcch_enabled(ctxt.pdcch_slot()) or not ue_cc.is_ul_enabled(ctxt.pusch_slot())) {
      // Either the PDCCH slot or PUSCH slots are not available.
      continue;
    }

    ue_pusch_grant        grant{&u, ue_cc.cell_index, h.id()};
    const ul_alloc_result result = ctxt.allocate_ul_grant(grant);
    // Continue iteration until skip slot indication is received.
    // NOTE: Allocation status other than skip_slot can be ignored because allocation of reTxs is done from oldest
    // HARQ pending to newest. Hence, other allocation status are redundant.
    if (result.status == alloc_status::skip_slot) {
      return result;
    }
  }

  // Return successful outcome in all other cases.
  // Other cases:
  //  - No pending HARQs to allocate.
  //  - At the end of pending HARQs iteration.
  return {alloc_status::success};
}

/// Allocate UE PUSCH grant for new transmissions.
static ul_alloc_result alloc_ul_ue_newtx(const slice_ue&         u,
                                         slice_ul_sched_context& ctxt,
                                         srslog::basic_logger&   logger,
                                         std::optional<unsigned> max_grant_rbs = {})
{
  unsigned pending_newtx_bytes = u.pending_ul_newtx_bytes();
  if (pending_newtx_bytes == 0) {
    return {alloc_status::skip_ue};
  }
  const ue_cell& ue_cc = *u.find_cell(ctxt.cell_index());

  if (can_allocate_ul_newtx(u, ue_cc, ctxt.pdcch_slot(), ctxt.pusch_slot(), logger)) {
    ue_pusch_grant        grant{&u, ue_cc.cell_index, INVALID_HARQ_ID, pending_newtx_bytes, max_grant_rbs};
    const ul_alloc_result result = ctxt.allocate_ul_grant(grant);
    // If the allocation failed due to invalid parameters, we continue iteration.
    if (result.status != alloc_status::invalid_params) {
      return result;
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

void scheduler_time_rr::dl_sched(slice_dl_sched_context& ctxt)
{
  const slice_ue_repository& ues = ctxt.get_slice().get_slice_ues();

  unsigned max_pdsch_rbs = ctxt.get_slice().remaining_rbs();
  if (ues.empty() or max_pdsch_rbs == 0) {
    // No UEs to be scheduled or if there are no RBs to be scheduled in slice.
    return;
  }

  // First, schedule UEs with re-transmissions.
  auto retx_result = alloc_dl_retxs(ctxt);
  if (retx_result.status == alloc_status::skip_slot) {
    return;
  }

  // Schedule new Txs.
  dl_sched_newtx(ctxt);
}

void scheduler_time_rr::dl_sched_newtx(slice_dl_sched_context& ctxt)
{
  const slice_ue_repository& ues            = ctxt.get_slice().get_slice_ues();
  unsigned                   max_rbs_per_ue = compute_max_nof_rbs_per_ue_per_slot(expert_cfg, ctxt);
  if (max_rbs_per_ue == 0) {
    return;
  }

  int rbs_missing = 0;

  auto drb_newtx_ue_function = [this, &ctxt, max_rbs_per_ue, rbs_missing](const slice_ue& u) mutable {
    // Determine the max grant size in RBs, accounting the RBs that were left to be allocated earlier and the slice
    // candidate max RBs, that changes on each allocation.
    unsigned max_grant_size = std::max((int)max_rbs_per_ue + rbs_missing, 0);
    max_grant_size          = std::min(max_grant_size, ctxt.get_slice().remaining_rbs());
    if (max_grant_size == 0) {
      return dl_alloc_result{alloc_status::skip_slot};
    }

    // Allocate UE newtx.
    auto result = alloc_dl_ue_newtx(u, ctxt, logger, max_grant_size);

    if (result.status == alloc_status::success) {
      // Check if the grant was too small and we need to compensate in the next grants.
      rbs_missing += (max_rbs_per_ue - result.alloc_nof_rbs);
    }
    return result;
  };
  auto result      = round_robin_apply(ues, next_dl_ue_index, drb_newtx_ue_function);
  next_dl_ue_index = result.first;
}

void scheduler_time_rr::ul_sched(slice_ul_sched_context& ctxt)
{
  const slice_ue_repository& ues     = ctxt.get_slice().get_slice_ues();
  const unsigned             max_rbs = ctxt.get_slice().remaining_rbs();

  if (ues.empty() or max_rbs == 0) {
    // No UEs to be scheduled or if there are no RBs to be scheduled in slice.
    return;
  }

  // First, schedule UEs with re-transmissions.
  auto retx_result = alloc_ul_retxs(ues, ctxt);
  if (retx_result.status == alloc_status::skip_slot) {
    return;
  }

  // Then, schedule UEs with new transmissions.
  ul_sched_newtx(ctxt);
}

void scheduler_time_rr::ul_sched_newtx(slice_ul_sched_context& ctxt)
{
  const slice_ue_repository& ues            = ctxt.get_slice().get_slice_ues();
  const unsigned             max_rbs_per_ue = compute_max_nof_rbs_per_ue_per_slot(expert_cfg, ctxt);
  if (max_rbs_per_ue == 0) {
    return;
  }

  int  rbs_missing         = 0;
  auto data_tx_ue_function = [this, &ctxt, max_rbs_per_ue, rbs_missing](const slice_ue& u) mutable {
    // Determine the max grant size in RBs, accounting the RBs that were left to be allocated earlier and the slice
    // candidate max RBs, that changes on each allocation.
    unsigned max_grant_size = std::max((int)max_rbs_per_ue + rbs_missing, 0);
    max_grant_size          = std::min(max_grant_size, ctxt.get_slice().remaining_rbs());
    if (max_grant_size == 0) {
      return ul_alloc_result{alloc_status::skip_slot};
    }

    ul_alloc_result result = alloc_ul_ue_newtx(u, ctxt, logger, max_grant_size);
    if (result.status == alloc_status::success) {
      // Check if the grant was too small and we need to compensate in the next grants.
      rbs_missing += (max_rbs_per_ue - result.alloc_nof_rbs);
    }
    return result;
  };
  auto result      = round_robin_apply(ues, next_ul_ue_index, data_tx_ue_function);
  next_ul_ue_index = result.first;
}
