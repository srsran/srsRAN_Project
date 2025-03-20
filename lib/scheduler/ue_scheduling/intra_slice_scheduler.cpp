/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "intra_slice_scheduler.h"

using namespace srsran;

/// \brief Helper function to determine the maximum number of PDSCHs to allocate per slot in a manner that ensures fair
/// distribution of PDSCHs across slots.
static unsigned compute_max_pdschs_per_slot(const cell_configuration& cell_cfg)
{
  unsigned pdschs_per_slot =
      std::min(static_cast<unsigned>(MAX_PDSCH_PDUS_PER_SLOT), cell_cfg.expert_cfg.ue.max_pdschs_per_slot);
  // Note: We subtract to -1 to max_ul_grants_per_slot to always leave a grant for PUSCH which gets scheduled
  // afterwards.
  const unsigned max_pucchs_per_slot =
      std::min(cell_cfg.expert_cfg.ue.max_pucchs_per_slot, cell_cfg.expert_cfg.ue.max_ul_grants_per_slot - 1);

  if (cell_cfg.tdd_cfg_common.has_value()) {
    // TDD.
    const unsigned nof_dl_slots = nof_dl_slots_per_tdd_period(cell_cfg.tdd_cfg_common.value());
    // Note: use all UL slots once partial UL slots are used for UCI.
    const unsigned nof_ul_slots = nof_full_ul_slots_per_tdd_period(cell_cfg.tdd_cfg_common.value());
    const unsigned max_pucchs   = nof_ul_slots * max_pucchs_per_slot;
    pdschs_per_slot             = std::min(pdschs_per_slot, max_pucchs / nof_dl_slots);
  } else {
    // FDD.
    pdschs_per_slot = std::min(max_pucchs_per_slot, pdschs_per_slot);
  }

  pdschs_per_slot = std::max(1U, pdschs_per_slot);
  return pdschs_per_slot;
}

intra_slice_scheduler::intra_slice_scheduler(const scheduler_ue_expert_config& expert_cfg_,
                                             ue_repository&                    ues,
                                             pdcch_resource_allocator&         pdcch_alloc,
                                             uci_allocator&                    uci_alloc_,
                                             cell_resource_allocator&          cell_alloc_,
                                             cell_harq_manager&                cell_harqs_,
                                             srslog::basic_logger&             logger_) :
  expert_cfg(expert_cfg_),
  cell_alloc(cell_alloc_),
  cell_harqs(cell_harqs_),
  uci_alloc(uci_alloc_),
  logger(logger_),
  max_pdschs_per_slot(compute_max_pdschs_per_slot(cell_alloc.cfg)),
  ue_alloc(expert_cfg, ues, pdcch_alloc, uci_alloc, cell_alloc_, logger_)
{
  newtx_candidates.reserve(MAX_NOF_DU_UES);
  pending_dl_newtxs.reserve(MAX_UE_PDUS_PER_SLOT);
  pending_ul_newtxs.reserve(MAX_UE_PDUS_PER_SLOT);
}

void intra_slice_scheduler::slot_indication(slot_point sl_tx)
{
  pdcch_slot        = sl_tx;
  dl_attempts_count = 0;
  ul_attempts_count = 0;
}

void intra_slice_scheduler::post_process_results()
{
  ue_alloc.post_process_results();
}

void intra_slice_scheduler::dl_sched(dl_ran_slice_candidate slice, scheduler_policy& policy)
{
  srsran_sanity_check(slice.remaining_rbs() > 0, "Invalid slice slice");
  if (slice.get_slot_tx() != pdsch_slot) {
    pdsch_slot = slice.get_slot_tx();
    update_used_dl_crbs();
  }

  // Determine max number of UE grants that can be scheduled in this slot.
  unsigned pdschs_to_alloc = max_pdschs_to_alloc(slice);
  if (pdschs_to_alloc == 0) {
    return;
  }

  // Schedule reTxs.
  unsigned nof_retxs_alloc = schedule_dl_retx_candidates(slice, pdschs_to_alloc);
  pdschs_to_alloc -= std::min(pdschs_to_alloc, nof_retxs_alloc);
  if (pdschs_to_alloc == 0) {
    return;
  }

  // Allocate UE newTx grants.
  schedule_dl_newtx_candidates(slice, policy, pdschs_to_alloc);
}

void intra_slice_scheduler::ul_sched(ul_ran_slice_candidate slice, scheduler_policy& ul_policy)
{
  srsran_sanity_check(slice.remaining_rbs() > 0, "Invalid slice slice");
  if (slice.get_slot_tx() != pusch_slot) {
    pusch_slot = slice.get_slot_tx();
    update_used_ul_crbs();
  }

  // Determine max number of UE grants that can be scheduled in this slot.
  unsigned puschs_to_alloc = max_puschs_to_alloc(slice);
  if (puschs_to_alloc == 0) {
    return;
  }

  // Schedule reTxs.
  unsigned nof_retxs_alloc = schedule_ul_retx_candidates(slice, puschs_to_alloc);
  puschs_to_alloc -= std::min(puschs_to_alloc, nof_retxs_alloc);
  if (puschs_to_alloc == 0) {
    return;
  }

  // Allocate UE newTx grants.
  schedule_ul_newtx_candidates(slice, ul_policy, puschs_to_alloc);
}

/// \brief Helper function that returns a pair with the remaining number of RBs to allocate in this slice scheduling
/// opportunity and the recommended number of RBs per UE grant.
template <typename SliceCandidate>
static std::pair<unsigned, unsigned> get_max_grants_and_rb_grant_size(span<const ue_newtx_candidate> ue_candidates,
                                                                      const cell_resource_allocator& cell_alloc,
                                                                      const SliceCandidate&          slice,
                                                                      const crb_bitmap&              used_crbs,
                                                                      unsigned max_ue_grants_to_alloc)
{
  constexpr bool is_dl = std::is_same_v<dl_ran_slice_candidate, SliceCandidate>;
  static_assert(is_dl or std::is_same_v<ul_ran_slice_candidate, SliceCandidate>, "Invalid slice candidate");

  unsigned ues_to_alloc = max_ue_grants_to_alloc;

  // [Implementation-defined] We use the same searchSpace config to determine the number of RBs available.
  const ue_cell_configuration& ue_cfg  = ue_candidates[0].ue_cc->cfg();
  const search_space_id        ss_id   = ue_cfg.init_bwp().dl_ded.value()->pdcch_cfg->search_spaces.back().get_id();
  const auto*                  ss_info = ue_cfg.find_search_space(ss_id);
  if (ss_info == nullptr) {
    return std::make_pair(0, 0);
  }

  // Determine how many UE grants to allocate in the slot (assuming full buffer). As an heuristic, we divide the number
  // of candidates by 4 and set 8 as the maximum number of UEs to be scheduled per slot, assuming full buffer. This
  // heuristic is a result of a tradeoff between minimizing latency and ensuring we don't deplete the PDCCH resources.
  static constexpr unsigned MAX_UE_GRANT_PER_SLOT = 8;
  ues_to_alloc = std::min({ues_to_alloc, std::max((unsigned)ue_candidates.size() / 4U, 1U), MAX_UE_GRANT_PER_SLOT});

  // > Compute maximum nof. PDCCH candidates allowed for each direction.
  // [Implementation-defined]
  // - Assume aggregation level 2 while computing nof. candidates that can be fit in CORESET.
  // - CORESET CCEs are divided by 2 to provide equal PDCCH resources to DL and UL.
  unsigned max_nof_candidates = (ss_info->coreset->get_nof_cces() / 2) / to_nof_cces(aggregation_level::n2);

  // > Subtract already scheduled PDCCHs.
  unsigned pdcchs_in_grid = is_dl ? cell_alloc[0].result.dl.dl_pdcchs.size() : cell_alloc[0].result.dl.ul_pdcchs.size();
  max_nof_candidates -= std::min(pdcchs_in_grid, max_nof_candidates);

  // > Ensure fairness in PDCCH allocation between DL and UL.
  // [Implementation-defined] To avoid running out of PDCCH candidates for UL allocation in multi-UE scenario and short
  // BW (e.g. TDD and 10Mhz BW), apply further limits on nof. UEs to be scheduled per slot.
  ues_to_alloc = std::min(max_nof_candidates, ues_to_alloc);
  if (ues_to_alloc == 0) {
    return std::make_pair(0, 0);
  }

  const crb_interval& bwp_crb_limits = is_dl ? ss_info->dl_crb_lims : ss_info->ul_crb_lims;
  unsigned            max_nof_rbs    = std::min(bwp_crb_limits.length(), slice.remaining_rbs());
  // Count how many RBs are still available for allocation.
  max_nof_rbs = std::min(max_nof_rbs, (unsigned)(~used_crbs).count());
  if (max_nof_rbs == 0) {
    return std::make_pair(0, 0);
  }

  static constexpr unsigned MIN_RB_PER_GRANT = 4;
  return std::make_pair(max_nof_rbs, std::max(max_nof_rbs / ues_to_alloc, MIN_RB_PER_GRANT));
}

unsigned intra_slice_scheduler::schedule_dl_retx_candidates(dl_ran_slice_candidate& slice,
                                                            unsigned                max_ue_grants_to_alloc)
{
  const slice_ue_repository& slice_ues     = slice.get_slice_ues();
  dl_harq_pending_retx_list  pending_harqs = cell_harqs.pending_dl_retxs();

  unsigned alloc_count = 0;
  for (auto it = pending_harqs.begin(); it != pending_harqs.end();) {
    // Note: During retx alloc, the pending HARQ list will mutate. So, we prefetch the next node.
    auto prev_it = it++;
    auto h       = *prev_it;

    if (h.get_grant_params().slice_id != slice.id()) {
      // HARQ ReTx is for another slice.
      continue;
    }
    const slice_ue& u = slice_ues[h.ue_index()];
    if (not can_allocate_pdsch(u, u.get_cc())) {
      continue;
    }

    // Perform DL grant allocation, including PDCCH, PDSCH and UCI.
    auto result = ue_alloc.allocate_dl_grant(ue_retx_dl_grant_request{u, pdsch_slot, h, used_dl_crbs});
    if (not result.has_value() and result.error() == alloc_status::skip_slot) {
      // Received signal to stop allocations in the slot.
      break;
    }

    if (result.has_value()) {
      crb_interval alloc_crbs = result.value();
      used_dl_crbs.fill(alloc_crbs.start(), alloc_crbs.stop());
      slice.store_grant(alloc_crbs.length());
      if (++alloc_count >= max_ue_grants_to_alloc or slice.remaining_rbs() == 0) {
        // Maximum number of allocations reached.
        break;
      }
    }

    dl_attempts_count++;
    if (dl_attempts_count >= expert_cfg.max_pdcch_alloc_attempts_per_slot) {
      // Maximum number of attempts per slot reached.
      break;
    }
  }

  return alloc_count;
}

unsigned intra_slice_scheduler::schedule_ul_retx_candidates(ul_ran_slice_candidate& slice,
                                                            unsigned                max_ue_grants_to_alloc)
{
  const slice_ue_repository& slice_ues     = slice.get_slice_ues();
  ul_harq_pending_retx_list  pending_harqs = cell_harqs.pending_ul_retxs();

  unsigned alloc_count = 0;
  for (auto it = pending_harqs.begin(); it != pending_harqs.end();) {
    // Note: During retx alloc, the pending HARQ list will mutate. So, we prefetch the next node.
    auto prev_it = it++;
    auto h       = *prev_it;

    if (h.get_grant_params().slice_id != slice.id()) {
      continue;
    }
    const slice_ue& u = slice_ues[h.ue_index()];

    if (not can_allocate_pusch(u, u.get_cc())) {
      continue;
    }

    // Allocate PDCCH and PUSCH.
    auto result = ue_alloc.allocate_ul_grant(ue_retx_ul_grant_request{u, pusch_slot, h, used_ul_crbs});

    if (not result.has_value() and result.error() == alloc_status::skip_slot) {
      // Received signal to stop allocations in the slot.
      break;
    }

    if (result.has_value()) {
      crb_interval alloc_crbs = result.value();
      used_ul_crbs.fill(alloc_crbs.start(), alloc_crbs.stop());
      slice.store_grant(alloc_crbs.length());
      if (++alloc_count >= max_ue_grants_to_alloc or slice.remaining_rbs() == 0) {
        // Maximum number of allocations reached.
        break;
      }
    }

    ul_attempts_count++;
    if (ul_attempts_count >= expert_cfg.max_pdcch_alloc_attempts_per_slot) {
      // Maximum number of attempts per slot reached.
      break;
    }
  }

  return alloc_count;
}

void intra_slice_scheduler::prepare_newtx_dl_candidates(const dl_ran_slice_candidate& slice,
                                                        scheduler_policy&             dl_policy)
{
  const slice_ue_repository& slice_ues = slice.get_slice_ues();

  // Build list of UE candidates for newTx.
  newtx_candidates.clear();
  for (const slice_ue& u : slice_ues) {
    auto ue_candidate = create_newtx_dl_candidate(u);
    if (ue_candidate.has_value()) {
      newtx_candidates.push_back(ue_candidate.value());
    }
  }
  if (newtx_candidates.empty()) {
    return;
  }

  // Compute priorities using the provided policy.
  dl_policy.compute_ue_dl_priorities(pdcch_slot, pdsch_slot, newtx_candidates);

  // Sort candidates by priority in descending order.
  std::sort(newtx_candidates.begin(), newtx_candidates.end(), [](const auto& a, const auto& b) {
    return a.priority > b.priority;
  });

  // Remove candidates with forbid priority.
  auto rit = std::find_if(newtx_candidates.rbegin(), newtx_candidates.rend(), [](const auto& cand) {
    return cand.priority != forbid_sched_priority;
  });
  newtx_candidates.erase(rit.base(), newtx_candidates.end());
}

void intra_slice_scheduler::prepare_newtx_ul_candidates(const ul_ran_slice_candidate& slice,
                                                        scheduler_policy&             ul_policy)
{
  const slice_ue_repository& slice_ues = slice.get_slice_ues();

  // Build list of UE candidates.
  newtx_candidates.clear();
  for (const slice_ue& u : slice_ues) {
    auto ue_candidate = create_newtx_ul_candidate(u);
    if (ue_candidate.has_value()) {
      newtx_candidates.push_back(ue_candidate.value());
    }
  }
  if (newtx_candidates.empty()) {
    return;
  }

  // Compute priorities using the provided policy.
  ul_policy.compute_ue_ul_priorities(pdcch_slot, pusch_slot, newtx_candidates);

  // Sort candidates by priority in descending order.
  std::sort(newtx_candidates.begin(), newtx_candidates.end(), [](const auto& a, const auto& b) {
    return a.priority > b.priority;
  });

  // Remove candidates with forbid priority.
  auto rit = std::find_if(newtx_candidates.rbegin(), newtx_candidates.rend(), [](const auto& cand) {
    return cand.priority != forbid_sched_priority;
  });
  newtx_candidates.erase(rit.base(), newtx_candidates.end());
}

unsigned intra_slice_scheduler::schedule_dl_newtx_candidates(dl_ran_slice_candidate& slice,
                                                             scheduler_policy&       dl_policy,
                                                             unsigned                max_ue_grants_to_alloc)
{
  // Prepare candidate list.
  prepare_newtx_dl_candidates(slice, dl_policy);
  if (newtx_candidates.empty()) {
    return 0;
  }

  // Recompute max number of UE grants that can be scheduled in this slot and the number of RBs per grant.
  auto [rbs_to_alloc, max_rbs_per_grant] =
      get_max_grants_and_rb_grant_size(newtx_candidates, cell_alloc, slice, used_dl_crbs, max_ue_grants_to_alloc);
  if (max_rbs_per_grant == 0) {
    return 0;
  }

  // Stage 1: Pre-select UEs with the highest priority and reserve control-plane space for their DL grants.
  unsigned rb_count = 0;
  for (const auto& ue_candidate : newtx_candidates) {
    // Create DL grant builder.
    auto result =
        ue_alloc.allocate_dl_grant(ue_newtx_dl_grant_request{*ue_candidate.ue, pdsch_slot, ue_candidate.pending_bytes});

    if (result.has_value()) {
      // Allocation was successful. Move grant builder to list of pending newTx grants.
      auto& grant_builder = result.value();
      rb_count += std::min(grant_builder.context().expected_nof_rbs, max_rbs_per_grant);
      pending_dl_newtxs.push_back(std::move(grant_builder));
      if (rb_count >= rbs_to_alloc) {
        // Enough UEs have been allocated to ensure that the grid is filled. Move to stage 2.
        break;
      }
      if (pending_dl_newtxs.size() >= max_ue_grants_to_alloc) {
        // Maximum number of allocations reached. Move to stage 2.
        break;
      }
    } else if (result.error() == alloc_status::skip_slot) {
      // Received signal to stop allocations in the slot. Move to stage 2.
      break;
    }

    dl_attempts_count++;
    if (dl_attempts_count >= expert_cfg.max_pdcch_alloc_attempts_per_slot) {
      // Maximum number of attempts per slot reached.
      break;
    }
  }

  if (pending_dl_newtxs.empty()) {
    return 0;
  }

  // Stage 2: Derive CRBs, MCS, and number of layers for each grant.
  max_rbs_per_grant = rbs_to_alloc / pending_dl_newtxs.size();
  rb_count          = 0;
  int rbs_missing   = 0;
  for (unsigned alloc_count = 0, nof_grants = pending_dl_newtxs.size(); alloc_count != nof_grants; ++alloc_count) {
    auto& grant_builder = pending_dl_newtxs[alloc_count];
    // Determine the max grant size in RBs for this grant.
    int max_grant_size;
    if (alloc_count == nof_grants - 1) {
      // For the last UE, we also account for the remainder.
      max_grant_size = rbs_to_alloc - rb_count;
    } else {
      max_grant_size = max_rbs_per_grant + rbs_missing;
    }
    srsran_assert(max_grant_size > 0, "Invalid grant size.");

    // Derive recommended parameters for the DL newTx grant.
    crb_interval alloc_crbs = grant_builder.recommended_crbs(used_dl_crbs, max_grant_size);
    if (alloc_crbs.empty()) {
      logger.error("ue={} c-rnti={}: Failed to allocate PDSCH CRBs",
                   fmt::underlying(grant_builder.ue().ue_index()),
                   grant_builder.ue().crnti());
      continue;
    }

    // Save CRBs, MCS and RI.
    grant_builder.set_pdsch_params(alloc_crbs);

    // Fill used CRBs.
    unsigned nof_rbs_alloc = alloc_crbs.length();
    used_dl_crbs.fill(alloc_crbs.start(), alloc_crbs.stop());

    // Update slice state.
    slice.store_grant(nof_rbs_alloc);
    rb_count += nof_rbs_alloc;
    rbs_missing = (max_grant_size - nof_rbs_alloc);
  }

  // Clear grant builders.
  unsigned alloc_count = pending_dl_newtxs.size();
  pending_dl_newtxs.clear();

  // Update policy with final allocation results.
  const auto& pdschs = cell_alloc[pdsch_slot].result.dl.ue_grants;
  dl_policy.save_dl_newtx_grants(span<const dl_msg_alloc>(pdschs.end() - alloc_count, pdschs.end()));

  return alloc_count;
}

unsigned intra_slice_scheduler::schedule_ul_newtx_candidates(ul_ran_slice_candidate& slice,
                                                             scheduler_policy&       ul_policy,
                                                             unsigned                max_ue_grants_to_alloc)
{
  // Prepare candidate list.
  prepare_newtx_ul_candidates(slice, ul_policy);
  if (newtx_candidates.empty()) {
    return 0;
  }

  // Recompute max number of UE grants that can be scheduled in this slot and the number of RBs per grant.
  auto [rbs_to_alloc, expected_rbs_per_grant] =
      get_max_grants_and_rb_grant_size(newtx_candidates, cell_alloc, slice, used_ul_crbs, max_ue_grants_to_alloc);
  if (expected_rbs_per_grant == 0) {
    return 0;
  }

  // Stage 1: Pre-select UEs with the highest priority and reserve control-plane space for their UL grants.
  unsigned rb_count = 0;
  for (const auto& ue_candidate : newtx_candidates) {
    // Create UL grant builder.
    auto result =
        ue_alloc.allocate_ul_grant(ue_newtx_ul_grant_request{*ue_candidate.ue, pusch_slot, ue_candidate.pending_bytes});

    if (result.has_value()) {
      // Allocation was successful. Move grant builder to list of pending newTx grants.
      auto& grant_builder = result.value();
      rb_count += std::min(grant_builder.context().expected_nof_rbs, expected_rbs_per_grant);
      pending_ul_newtxs.push_back(std::move(grant_builder));
      if (rb_count >= rbs_to_alloc) {
        // Enough UEs have been allocated to ensure that the grid is filled. Move to stage 2.
        break;
      }
      if (pending_ul_newtxs.size() >= max_ue_grants_to_alloc) {
        // Maximum number of allocations reached. Move to stage 2.
        break;
      }
    } else if (result.error() == alloc_status::skip_slot) {
      // Received signal to stop allocations in the slot. Move to stage 2.
      break;
    }

    ul_attempts_count++;
    if (ul_attempts_count >= expert_cfg.max_pdcch_alloc_attempts_per_slot) {
      // Maximum number of attempts per slot reached.
      break;
    }
  }

  if (pending_ul_newtxs.empty()) {
    return 0;
  }

  // Stage 2: Derive CRBs, MCS, and number of layers for each grant.
  expected_rbs_per_grant = rbs_to_alloc / pending_ul_newtxs.size();
  rb_count               = 0;
  int rbs_missing        = 0;
  for (unsigned alloc_count = 0, nof_grants = pending_ul_newtxs.size(); alloc_count != nof_grants; ++alloc_count) {
    auto& grant_builder = pending_ul_newtxs[alloc_count];
    // Determine the max grant size in RBs for this grant.
    int max_grant_size;
    if (alloc_count == nof_grants - 1) {
      // For the last UE, we also account for the remaining RBs.
      max_grant_size = rbs_to_alloc - rb_count;
    } else {
      max_grant_size = expected_rbs_per_grant + rbs_missing;
    }
    srsran_assert(max_grant_size > 0, "Invalid grant size.");

    // Derive recommended parameters for the DL newTx grant.
    crb_interval alloc_crbs = grant_builder.recommended_crbs(used_ul_crbs, max_grant_size);
    if (alloc_crbs.empty()) {
      logger.error("ue={} c-rnti={}: Failed to allocate PUSCH CRBs",
                   fmt::underlying(grant_builder.ue().ue_index()),
                   grant_builder.ue().crnti());
      continue;
    }

    // Save CRBs, MCS and RI.
    grant_builder.set_pusch_params(alloc_crbs);

    // Fill used CRBs.
    unsigned nof_rbs_alloc = alloc_crbs.length();
    used_ul_crbs.fill(alloc_crbs.start(), alloc_crbs.stop());

    // Update slice state.
    slice.store_grant(nof_rbs_alloc);
    rb_count += nof_rbs_alloc;
    rbs_missing = (max_grant_size - nof_rbs_alloc);
  }

  // Clear grant builders.
  unsigned alloc_count = pending_ul_newtxs.size();
  pending_ul_newtxs.clear();

  // Update policy with allocation results.
  const auto& puschs = cell_alloc[pusch_slot].result.ul.puschs;
  ul_policy.save_ul_newtx_grants(span<const ul_sched_info>(puschs.end() - alloc_count, puschs.end()));

  return alloc_count;
}

bool intra_slice_scheduler::can_allocate_pdsch(const slice_ue& u, const ue_cell& ue_cc) const
{
  // Check if PDCCH/PDSCH is possible for this slot (e.g. not in UL slot or measGap)
  srsran_assert(not ue_cc.is_in_fallback_mode(), "Slice UE cannot be in fallback mode");
  if (not ue_cc.is_pdsch_enabled(pdcch_slot, pdsch_slot)) {
    return false;
  }
  return true;
}

bool intra_slice_scheduler::can_allocate_pusch(const slice_ue& u, const ue_cell& ue_cc) const
{
  // Check if PDCCH/PUSCH are possible for the provided slots (e.g. not in UL slot or measGap)
  if (not ue_cc.is_pusch_enabled(pdcch_slot, pusch_slot)) {
    return false;
  }

  return true;
}

std::optional<ue_newtx_candidate> intra_slice_scheduler::create_newtx_dl_candidate(const slice_ue& u) const
{
  const ue_cell& ue_cc = u.get_cc();
  srsran_assert(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(), "Invalid slice UE state");

  // Check if the UE has pending data to transmit.
  unsigned pending_bytes = u.pending_dl_newtx_bytes();
  if (pending_bytes == 0) {
    return std::nullopt;
  }

  if (not can_allocate_pdsch(u, ue_cc)) {
    return std::nullopt;
  }

  if (not ue_cc.harqs.has_empty_dl_harqs()) {
    // No available HARQs.
    if (not ue_cc.harqs.find_pending_dl_retx().has_value()) {
      // All HARQs are waiting for their respective HARQ-ACK. This may be a symptom of a long RTT for the PDSCH
      // and HARQ-ACK.
      logger.warning(
          "ue={} rnti={} PDSCH allocation skipped. Cause: All the HARQs are allocated and waiting for their "
          "respective HARQ-ACK. Check if any HARQ-ACK went missing in the lower layers or is arriving too late to "
          "the scheduler.",
          fmt::underlying(ue_cc.ue_index),
          ue_cc.rnti());
    }
    return std::nullopt;
  }

  return ue_newtx_candidate{&u, &ue_cc, pending_bytes, forbid_sched_priority};
}

std::optional<ue_newtx_candidate> intra_slice_scheduler::create_newtx_ul_candidate(const slice_ue& u) const
{
  const ue_cell& ue_cc = u.get_cc();
  srsran_assert(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(), "Invalid slice UE state");

  // Check if the UE has pending data to transmit.
  unsigned pending_bytes = u.pending_ul_newtx_bytes();
  if (pending_bytes == 0) {
    return std::nullopt;
  }

  if (not can_allocate_pusch(u, ue_cc)) {
    return std::nullopt;
  }

  if (not ue_cc.harqs.has_empty_ul_harqs()) {
    // No available HARQs.
    if (not ue_cc.harqs.find_pending_ul_retx().has_value()) {
      // All HARQs are waiting for their respective CRC. This may be a symptom of a slow PUSCH processing chain.
      logger.warning("ue={} rnti={} PUSCH allocation skipped. Cause: All the UE HARQs are busy waiting for "
                     "their respective CRC result. Check if any CRC PDU went missing in the lower layers or is "
                     "arriving too late to the scheduler.",
                     fmt::underlying(ue_cc.ue_index),
                     ue_cc.rnti());
    }
    return std::nullopt;
  }

  return ue_newtx_candidate{&u, &ue_cc, pending_bytes, forbid_sched_priority};
}

unsigned intra_slice_scheduler::max_pdschs_to_alloc(const dl_ran_slice_candidate& slice)
{
  // We cannot allocate more than the number of UEs available.
  int pdschs_to_alloc = slice.get_slice_ues().size();

  // Determine how many UE DL PDUs can be allocated in this slot.
  auto& pdsch_res = cell_alloc[pdsch_slot].result;
  pdschs_to_alloc = std::min(pdschs_to_alloc, static_cast<int>(MAX_UE_PDUS_PER_SLOT - pdsch_res.dl.ue_grants.size()));
  if (pdschs_to_alloc <= 0) {
    return 0;
  }

  // Determine how many PDCCHs can be allocated in this slot.
  auto& pdcch_res = cell_alloc[pdcch_slot].result;
  pdschs_to_alloc = std::min({pdschs_to_alloc,
                              static_cast<int>(MAX_DL_PDCCH_PDUS_PER_SLOT - pdcch_res.dl.dl_pdcchs.size()),
                              static_cast<int>(expert_cfg.max_pdcch_alloc_attempts_per_slot - dl_attempts_count)});
  if (pdschs_to_alloc <= 0) {
    return 0;
  }

  // Determine how many PDSCHs can be allocated in this slot.
  int allocated_pdschs = pdsch_res.dl.ue_grants.size() + pdsch_res.dl.bc.sibs.size() + pdsch_res.dl.rar_grants.size() +
                         pdsch_res.dl.paging_grants.size();
  pdschs_to_alloc = std::min(pdschs_to_alloc, static_cast<int>(max_pdschs_per_slot) - allocated_pdschs);
  if (pdschs_to_alloc <= 0) {
    return 0;
  }

  // Assume at least one RB per UE.
  pdschs_to_alloc = std::min(pdschs_to_alloc, (int)slice.remaining_rbs());

  return std::max(pdschs_to_alloc, 0);
}

unsigned intra_slice_scheduler::max_puschs_to_alloc(const ul_ran_slice_candidate& slice)
{
  // We cannot allocate more than the number of UEs available.
  int puschs_to_alloc = slice.get_slice_ues().size();

  // The max PUSCHs-per-slot limit cannot be exceeded.
  auto& pusch_res = cell_alloc[pusch_slot].result;
  // Note: We use signed integer to avoid unsigned overflow.
  const int max_puschs = std::min(static_cast<unsigned>(MAX_PUSCH_PDUS_PER_SLOT), expert_cfg.max_puschs_per_slot);
  puschs_to_alloc      = max_puschs - (int)pusch_res.ul.puschs.size();
  if (puschs_to_alloc <= 0) {
    return 0;
  }

  // Assume at least one RB per UE.
  puschs_to_alloc = std::min(puschs_to_alloc, (int)slice.remaining_rbs());
  if (puschs_to_alloc <= 0) {
    return 0;
  }

  // Determine how many PDCCHs can be allocated in this slot.
  auto& pdcch_res = cell_alloc[pdcch_slot].result;
  puschs_to_alloc = std::min({puschs_to_alloc,
                              static_cast<int>(MAX_UL_PDCCH_PDUS_PER_SLOT - pdcch_res.dl.ul_pdcchs.size()),
                              static_cast<int>(expert_cfg.max_pdcch_alloc_attempts_per_slot - ul_attempts_count)});
  return std::max(puschs_to_alloc, 0);
}

void intra_slice_scheduler::update_used_dl_crbs()
{
  // (Implementation-defined) We use the common PDSCH TD resources as a reference for the computation of RBs
  // unavailable for PDSCH. This assumes that these resources are not colliding with PDCCH.
  const auto&              init_dl_bwp      = cell_alloc.cfg.dl_cfg_common.init_dl_bwp;
  const ofdm_symbol_range& symbols_to_check = init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].symbols;
  used_dl_crbs = cell_alloc[pdsch_slot].dl_res_grid.used_crbs(init_dl_bwp.generic_params, symbols_to_check);
}

void intra_slice_scheduler::update_used_ul_crbs()
{
  // (Implementation-defined) We use the common PUSCH TD resources as a reference for the computation of RBs unavailable
  // for PDSCH. This assumes that these resources are not colliding with SRS.
  const auto&              init_ul_bwp      = cell_alloc.cfg.ul_cfg_common.init_ul_bwp;
  const ofdm_symbol_range& symbols_to_check = init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].symbols;
  used_ul_crbs = cell_alloc[pusch_slot].ul_res_grid.used_crbs(init_ul_bwp.generic_params, symbols_to_check);
}
