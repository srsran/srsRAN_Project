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

#include "intra_slice_scheduler.h"

using namespace srsran;

intra_slice_scheduler::intra_slice_scheduler(const scheduler_ue_expert_config& expert_cfg_,
                                             ue_repository&                    ues,
                                             pdcch_resource_allocator&         pdcch_alloc,
                                             uci_allocator&                    uci_alloc,
                                             cell_resource_allocator&          cell_alloc_,
                                             cell_harq_manager&                cell_harqs_,
                                             srslog::basic_logger&             logger_) :
  expert_cfg(expert_cfg_),
  cell_alloc(cell_alloc_),
  cell_harqs(cell_harqs_),
  logger(logger_),
  ue_alloc(expert_cfg, ues, pdcch_alloc, uci_alloc, cell_alloc_, logger_)
{
  dl_newtx_candidates.reserve(MAX_NOF_DU_UES);
  ul_newtx_candidates.reserve(MAX_NOF_DU_UES);
}

void intra_slice_scheduler::slot_indication(slot_point sl_tx)
{
  last_sl_tx        = sl_tx;
  dl_attempts_count = 0;
  ul_attempts_count = 0;
}

void intra_slice_scheduler::post_process_results()
{
  ue_alloc.post_process_results();
}

void intra_slice_scheduler::dl_sched(slot_point pdcch_slot, dl_ran_slice_candidate slice, scheduler_policy& policy)
{
  srsran_sanity_check(slice.remaining_rbs() > 0, "Invalid slice slice");

  // Determine max number of UE grants that can be scheduled in this slot.
  unsigned pdschs_to_alloc = max_pdschs_to_alloc(pdcch_slot, slice);
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

void intra_slice_scheduler::ul_sched(slot_point pdcch_slot, ul_ran_slice_candidate slice, scheduler_policy& ul_policy)
{
  srsran_sanity_check(slice.remaining_rbs() > 0, "Invalid slice slice");

  // Determine max number of UE grants that can be scheduled in this slot.
  unsigned puschs_to_alloc = max_puschs_to_alloc(pdcch_slot, slice);
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
                                                                      unsigned max_ue_grants_to_alloc)
{
  constexpr bool is_dl = std::is_same_v<dl_ran_slice_candidate, SliceCandidate>;
  static_assert(is_dl or std::is_same_v<ul_ran_slice_candidate, SliceCandidate>, "Invalid slice candidate");

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
  unsigned                  expected_grants       = std::min(
      max_ue_grants_to_alloc, std::min(std::max((unsigned)ue_candidates.size() / 4U, 1U), MAX_UE_GRANT_PER_SLOT));

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
  expected_grants = std::min(max_nof_candidates, expected_grants);
  if (expected_grants == 0) {
    return std::make_pair(0, 0);
  }

  const crb_interval& bwp_crb_limits = is_dl ? ss_info->dl_crb_lims : ss_info->ul_crb_lims;
  unsigned            max_nof_rbs    = std::min(bwp_crb_limits.length(), slice.remaining_rbs());

  static constexpr unsigned MIN_RB_PER_GRANT = 4;
  return std::make_pair(max_nof_rbs, std::max(divide_ceil(max_nof_rbs, expected_grants), MIN_RB_PER_GRANT));
}

unsigned intra_slice_scheduler::schedule_dl_retx_candidates(const dl_ran_slice_candidate& slice,
                                                            unsigned                      max_ue_grants_to_alloc)
{
  const slice_ue_repository& slice_ues     = slice.get_slice_ues();
  slot_point                 pdcch_slot    = cell_alloc.slot_tx();
  slot_point                 pdsch_slot    = slice.get_slot_tx();
  dl_harq_pending_retx_list  pending_harqs = cell_harqs.pending_dl_retxs();

  unsigned alloc_count = 0;
  for (auto it = pending_harqs.begin(); it != pending_harqs.end();) {
    // Note: During retx alloc, the pending HARQ list will mutate. So, we prefetch the next node.
    auto prev_it = it++;
    auto h       = *prev_it;

    if (h.get_grant_params().slice_id != slice.id()) {
      continue;
    }
    const slice_ue& u = slice_ues[h.ue_index()];
    if (not can_allocate_pdsch(pdcch_slot, pdsch_slot, u, u.get_cc())) {
      continue;
    }

    // Perform allocation of PDCCH, PDSCH and UCI.
    dl_alloc_result result = ue_alloc.allocate_retx_dl_grant(ue_dl_retx_grant_request{pdsch_slot, u, h});

    if (result.status == alloc_status::skip_slot) {
      // Received signal to stop allocations in the slot.
      break;
    }

    if (result.status == alloc_status::success) {
      if (++alloc_count >= max_ue_grants_to_alloc) {
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

unsigned intra_slice_scheduler::schedule_ul_retx_candidates(const ul_ran_slice_candidate& slice,
                                                            unsigned                      max_ue_grants_to_alloc)
{
  const slice_ue_repository& slice_ues     = slice.get_slice_ues();
  slot_point                 pdcch_slot    = cell_alloc.slot_tx();
  slot_point                 pusch_slot    = slice.get_slot_tx();
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

    if (not can_allocate_pusch(pdcch_slot, pusch_slot, u, u.get_cc())) {
      continue;
    }

    // Allocate PDCCH and PUSCH.
    ul_alloc_result result = ue_alloc.allocate_retx_ul_grant(ue_ul_retx_grant_request{pusch_slot, u, h});

    if (result.status == alloc_status::skip_slot) {
      // Received signal to stop allocations in the slot.
      break;
    }

    if (result.status == alloc_status::success) {
      if (++alloc_count >= max_ue_grants_to_alloc) {
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
  const slice_ue_repository& slice_ues  = slice.get_slice_ues();
  slot_point                 pdcch_slot = cell_alloc.slot_tx();
  slot_point                 pdsch_slot = slice.get_slot_tx();

  // Build list of UE candidates for newTx.
  dl_newtx_candidates.clear();
  for (const slice_ue& u : slice_ues) {
    auto ue_candidate = create_newtx_dl_candidate(pdcch_slot, pdsch_slot, u);
    if (ue_candidate.has_value()) {
      dl_newtx_candidates.push_back(ue_candidate.value());
    }
  }
  if (dl_newtx_candidates.empty()) {
    return;
  }

  // Compute priorities using the provided policy.
  dl_policy.compute_ue_dl_priorities(pdcch_slot, pdsch_slot, dl_newtx_candidates);

  // Sort candidates by priority in descending order.
  std::sort(dl_newtx_candidates.begin(), dl_newtx_candidates.end(), [](const auto& a, const auto& b) {
    return a.priority > b.priority;
  });

  // Remove candidates with forbid priority.
  auto rit = std::find_if(dl_newtx_candidates.rbegin(), dl_newtx_candidates.rend(), [](const auto& cand) {
    return cand.priority != forbid_sched_priority;
  });
  dl_newtx_candidates.erase(rit.base(), dl_newtx_candidates.end());
}

void intra_slice_scheduler::prepare_newtx_ul_candidates(const ul_ran_slice_candidate& slice,
                                                        scheduler_policy&             ul_policy)
{
  const slice_ue_repository& slice_ues  = slice.get_slice_ues();
  slot_point                 pdcch_slot = cell_alloc.slot_tx();
  slot_point                 pusch_slot = slice.get_slot_tx();

  // Build list of UE candidates.
  ul_newtx_candidates.clear();
  for (const slice_ue& u : slice_ues) {
    auto ue_candidate = create_newtx_ul_candidate(pdcch_slot, pusch_slot, u);
    if (ue_candidate.has_value()) {
      ul_newtx_candidates.push_back(ue_candidate.value());
    }
  }
  if (ul_newtx_candidates.empty()) {
    return;
  }

  // Compute priorities using the provided policy.
  ul_policy.compute_ue_ul_priorities(pdcch_slot, pusch_slot, ul_newtx_candidates);

  // Sort candidates by priority in descending order.
  std::sort(ul_newtx_candidates.begin(), ul_newtx_candidates.end(), [](const auto& a, const auto& b) {
    return a.priority > b.priority;
  });

  // Remove candidates with forbid priority.
  auto rit = std::find_if(ul_newtx_candidates.rbegin(), ul_newtx_candidates.rend(), [](const auto& cand) {
    return cand.priority != forbid_sched_priority;
  });
  ul_newtx_candidates.erase(rit.base(), ul_newtx_candidates.end());
}

unsigned intra_slice_scheduler::schedule_dl_newtx_candidates(dl_ran_slice_candidate& slice,
                                                             scheduler_policy&       dl_policy,
                                                             unsigned                max_ue_grants_to_alloc)
{
  // Prepare candidate list.
  prepare_newtx_dl_candidates(slice, dl_policy);
  if (dl_newtx_candidates.empty()) {
    return 0;
  }

  // Recompute max number of UE grants that can be scheduled in this slot and the number of RBs per grant.
  slot_point pdsch_slot = slice.get_slot_tx();
  auto [rbs_to_alloc, max_rbs_per_grant] =
      get_max_grants_and_rb_grant_size(dl_newtx_candidates, cell_alloc, slice, max_ue_grants_to_alloc);
  if (max_rbs_per_grant == 0) {
    return 0;
  }

  // Allocate DL grants.
  unsigned alloc_count           = 0;
  int      rbs_missing           = 0;
  unsigned expected_ues_to_alloc = divide_ceil(rbs_to_alloc, max_rbs_per_grant);
  for (const auto& ue_candidate : dl_newtx_candidates) {
    // Determine the max grant size in RBs.
    unsigned max_grant_size = 0;
    unsigned rem_rbs        = rbs_to_alloc - (alloc_count * max_rbs_per_grant - rbs_missing);
    if (rem_rbs < max_rbs_per_grant * 2 or alloc_count + 1 >= expected_ues_to_alloc) {
      // If we are in the last allocation of the slot, fill remaining RBs.
      max_grant_size = rem_rbs;
    } else {
      // Account the RBs that were left to be allocated earlier that changes on each allocation.
      max_grant_size = std::max((int)max_rbs_per_grant + rbs_missing, 0);
      max_grant_size = std::min(max_grant_size, rem_rbs);
    }
    if (max_grant_size == 0) {
      break;
    }

    // Allocate DL grant.
    dl_alloc_result result = ue_alloc.allocate_newtx_dl_grant(
        ue_dl_newtx_grant_request{pdsch_slot, *ue_candidate.ue, ue_candidate.pending_bytes, max_rbs_per_grant});

    if (result.status == alloc_status::skip_slot) {
      // Received signal to stop allocations in the slot.
      break;
    }

    if (result.status == alloc_status::success) {
      slice.store_grant(result.alloc_nof_rbs);
      if (++alloc_count >= max_ue_grants_to_alloc) {
        // Maximum number of allocations reached.
        break;
      }
      // Check if the grant was too small and we need to compensate in the next grants.
      rbs_missing += (max_rbs_per_grant - result.alloc_nof_rbs);
    }

    dl_attempts_count++;
    if (dl_attempts_count >= expert_cfg.max_pdcch_alloc_attempts_per_slot) {
      // Maximum number of attempts per slot reached.
      break;
    }
  }

  // Update policy with allocation results.
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
  if (ul_newtx_candidates.empty()) {
    return 0;
  }

  // Recompute max number of UE grants that can be scheduled in this slot and the number of RBs per grant.
  slot_point pusch_slot = slice.get_slot_tx();
  auto [rbs_to_alloc, max_rbs_per_grant] =
      get_max_grants_and_rb_grant_size(ul_newtx_candidates, cell_alloc, slice, max_ue_grants_to_alloc);
  if (max_rbs_per_grant == 0) {
    return 0;
  }

  // Allocate UL grants.
  unsigned alloc_count           = 0;
  int      rbs_missing           = 0;
  unsigned expected_ues_to_alloc = divide_ceil(rbs_to_alloc, max_rbs_per_grant);
  for (const auto& ue_candidate : ul_newtx_candidates) {
    // Determine the max grant size in RBs.
    unsigned max_grant_size = 0;
    unsigned rem_rbs        = rbs_to_alloc - (alloc_count * max_rbs_per_grant - rbs_missing);
    if (rem_rbs < max_rbs_per_grant * 2 or alloc_count + 1 >= expected_ues_to_alloc) {
      // This is the last UE to allocate. Fill the remaining RBs.
      max_grant_size = rem_rbs;
    } else {
      // Account the RBs that were left to be allocated earlier that changes on each allocation.
      max_grant_size = std::max((int)max_rbs_per_grant + rbs_missing, 0);
      max_grant_size = std::min(max_grant_size, rem_rbs);
    }
    if (max_grant_size == 0) {
      break;
    }

    // Allocate PDCCH and PUSCH for a new Tx.
    ul_alloc_result result = ue_alloc.allocate_newtx_ul_grant(
        ue_ul_newtx_grant_request{pusch_slot, *ue_candidate.ue, ue_candidate.pending_bytes, max_rbs_per_grant});

    if (result.status == alloc_status::skip_slot) {
      // Received signal to stop allocations in the slot.
      break;
    }

    if (result.status == alloc_status::success) {
      slice.store_grant(result.alloc_nof_rbs);
      if (++alloc_count >= max_ue_grants_to_alloc) {
        // Maximum number of allocations reached.
        break;
      }
      // Check if the grant was too small and we need to compensate in the next grants.
      rbs_missing += (max_rbs_per_grant - result.alloc_nof_rbs);
    }

    if (++ul_attempts_count >= expert_cfg.max_pdcch_alloc_attempts_per_slot) {
      // Maximum number of attempts per slot reached.
      break;
    }
  }

  // Update policy with allocation results.
  const auto& puschs = cell_alloc[pusch_slot].result.ul.puschs;
  ul_policy.save_ul_newtx_grants(span<const ul_sched_info>(puschs.end() - alloc_count, puschs.end()));

  return alloc_count;
}

bool intra_slice_scheduler::can_allocate_pdsch(slot_point      pdcch_slot,
                                               slot_point      pdsch_slot,
                                               const slice_ue& u,
                                               const ue_cell&  ue_cc) const
{
  // Check if PDCCH is active for this slot (e.g. not in UL slot or measGap)
  if (not ue_cc.is_pdcch_enabled(pdcch_slot)) {
    return false;
  }

  // Check if PDSCH is active for the PDSCH slot.
  // Note: no need to do this check if PDSCH slot == PDCCH slot.
  if (pdcch_slot != pdsch_slot and not ue_cc.is_pdsch_enabled(pdsch_slot)) {
    return false;
  }

  // Check if no PDSCH grant is already allocated for this UE in this slot.
  const auto& sched_pdschs = cell_alloc[pdsch_slot].result.dl.ue_grants;
  if (std::any_of(sched_pdschs.begin(), sched_pdschs.end(), [&u](const auto& grant) {
        return grant.pdsch_cfg.rnti == u.crnti();
      })) {
    // UE already has a PDSCH grant in this slot. (e.g. a ReTx has took place earlier)
    return false;
  }

  return true;
}

bool intra_slice_scheduler::can_allocate_pusch(slot_point      pdcch_slot,
                                               slot_point      pusch_slot,
                                               const slice_ue& u,
                                               const ue_cell&  ue_cc) const
{
  // Check if PDCCH is active for this slot (e.g. not in UL slot or measGap)
  if (not ue_cc.is_pdcch_enabled(pdcch_slot)) {
    return false;
  }

  // Check if UL is active for the PUSCH slot.
  if (not ue_cc.is_ul_enabled(pusch_slot)) {
    return false;
  }

  // Check if no PUSCH grant is already allocated for this UE in this slot.
  const auto& sched_puschs = cell_alloc[pusch_slot].result.ul.puschs;
  if (std::any_of(sched_puschs.begin(), sched_puschs.end(), [&u](const auto& grant) {
        return grant.pusch_cfg.rnti == u.crnti();
      })) {
    // UE already has a PDSCH grant in this slot. (e.g. a ReTx has took place earlier)
    return false;
  }

  return true;
}

std::optional<ue_newtx_candidate>
intra_slice_scheduler::create_newtx_dl_candidate(slot_point pdcch_slot, slot_point pdsch_slot, const slice_ue& u) const
{
  const ue_cell& ue_cc = u.get_cc();
  srsran_assert(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(), "Invalid slice UE state");

  // Check if the UE has pending data to transmit.
  unsigned pending_bytes = u.pending_dl_newtx_bytes();
  if (pending_bytes == 0) {
    return std::nullopt;
  }

  if (not can_allocate_pdsch(pdcch_slot, pdsch_slot, u, ue_cc)) {
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

std::optional<ue_newtx_candidate>
intra_slice_scheduler::create_newtx_ul_candidate(slot_point pdcch_slot, slot_point pusch_slot, const slice_ue& u) const
{
  const ue_cell& ue_cc = u.get_cc();
  srsran_assert(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(), "Invalid slice UE state");

  // Check if the UE has pending data to transmit.
  unsigned pending_bytes = u.pending_ul_newtx_bytes();
  if (pending_bytes == 0) {
    return std::nullopt;
  }

  if (not can_allocate_pusch(pdcch_slot, pusch_slot, u, ue_cc)) {
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

unsigned intra_slice_scheduler::max_pdschs_to_alloc(slot_point pdcch_slot, const dl_ran_slice_candidate& slice)
{
  // We cannot allocate more than the number of UEs available.
  int pdschs_to_alloc = slice.get_slice_ues().size();

  // Determine how many UE DL PDUs can be allocated in this slot.
  slot_point pdsch_slot = slice.get_slot_tx();
  auto&      pdsch_res  = cell_alloc[pdsch_slot].result;
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
  const int max_pdschs = std::min(
      static_cast<int>(MAX_UE_PDUS_PER_SLOT + MAX_RAR_PDUS_PER_SLOT + MAX_PAGING_PDUS_PER_SLOT + MAX_SI_PDUS_PER_SLOT),
      static_cast<int>(expert_cfg.max_pdschs_per_slot));
  int allocated_pdschs = pdsch_res.dl.ue_grants.size() + pdsch_res.dl.bc.sibs.size() + pdsch_res.dl.rar_grants.size() +
                         pdsch_res.dl.paging_grants.size();
  pdschs_to_alloc = std::min(pdschs_to_alloc, max_pdschs - allocated_pdschs);
  if (pdschs_to_alloc <= 0) {
    return 0;
  }

  // Assume at least one RB per UE.
  pdschs_to_alloc = std::min(pdschs_to_alloc, (int)slice.remaining_rbs());

  return std::max(pdschs_to_alloc, 0);
}

unsigned intra_slice_scheduler::max_puschs_to_alloc(slot_point pdcch_slot, const ul_ran_slice_candidate& slice)
{
  // We cannot allocate more than the number of UEs available.
  int puschs_to_alloc = slice.get_slice_ues().size();

  // The max PUSCHs-per-slot limit cannot be exceeded.
  slot_point pusch_slot = slice.get_slot_tx();
  auto&      pusch_res  = cell_alloc[pusch_slot].result;
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
