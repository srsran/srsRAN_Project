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
#include "../logging/scheduler_metrics_handler.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/support/math/mod_math_utils.h"

using namespace srsran;

/// \brief Helper function to determine the expected number of PDSCHs that can be allocated per slot in a manner that
/// ensures fair distribution of PDSCHs across slots.
///
/// To ensure fair distribution of PDSCHs across slots, we need to consider the number of UL slots available and how
/// many PUCCHs can be scheduled in each of the UL slots.
static unsigned compute_expected_pdschs_per_slot(const cell_configuration& cell_cfg)
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

// class intra_slice_scheduler::slice_ue_group_scheduler

void intra_slice_scheduler::slice_ue_group_scheduler::fill_ue_dl_candidate_group(
    std::vector<ue_newtx_candidate>& candidates,
    const dl_ran_slice_candidate&    slice)
{
  fill_ue_candidate_group(candidates, slice.get_slot_tx(), true, slice.get_slice_ues());
}

void intra_slice_scheduler::slice_ue_group_scheduler::fill_ue_ul_candidate_group(
    std::vector<ue_newtx_candidate>& candidates,
    const ul_ran_slice_candidate&    slice)
{
  fill_ue_candidate_group(candidates, slice.get_slot_tx(), false, slice.get_slice_ues());
}

void intra_slice_scheduler::slice_ue_group_scheduler::fill_ue_candidate_group(
    std::vector<ue_newtx_candidate>& candidates,
    slot_point                       sl_tx,
    bool                             is_dl,
    const slice_ue_repository&       slice_ues)
{
  candidates.clear();

  const unsigned nof_ues = slice_ues.size();
  if (nof_ues > parent->expert_cfg.pre_policy_rr_ue_group_size) {
    if (not last_pdcch_tx.valid() or last_pdcch_tx.sfn() != parent->pdcch_slot.sfn()) {
      // Update group_offset whenever we enter a new SFN.
      // The new group offset is chosen as the UE index after the last UE index considered in the previous group.
      // > Compute max ue index + 1, which will be used to perform the wrap-around of the group offset.
      const unsigned ue_idx_mod = (--slice_ues.end())->ue_index() + 1;
      // > Given that DL and UL UE groups are independent, we choose the minimum of the two last UE indexes, to ensure
      // that no UE is left unconsidered in any direction.
      group_offset    = (group_offset + std::min(max_dl_ue_count, max_ul_ue_count)) % ue_idx_mod;
      max_dl_ue_count = 0;
      max_ul_ue_count = 0;
    }
  } else {
    // The number of UEs is less than the group size, so we consider all UEs from the beginning.
    group_offset = 0;
  }
  last_pdcch_tx = parent->pdcch_slot;

  // Build list of candidates, starting from the UE group offset.
  unsigned       count                = 0;
  unsigned       last_candidate_count = 0;
  const unsigned group_size           = std::min(nof_ues, parent->expert_cfg.pre_policy_rr_ue_group_size);
  auto           start_ue_it          = slice_ues.lower_bound(to_du_ue_index(group_offset));
  for (auto ue_it = start_ue_it; count != nof_ues; ++ue_it) {
    ++count;
    if (ue_it == slice_ues.end()) {
      // wrap-around.
      ue_it = slice_ues.begin();
    }

    std::optional<ue_newtx_candidate> ue_candidate =
        is_dl ? parent->create_newtx_dl_candidate(*ue_it) : parent->create_newtx_ul_candidate(*ue_it);
    if (ue_candidate.has_value()) {
      candidates.push_back(ue_candidate.value());
      last_candidate_count = count;
      if (candidates.size() >= group_size) {
        break;
      }
    }
  }

  // Update the next UE group offset, if the last UE index went beyond next group offset.
  auto& max_count = is_dl ? max_dl_ue_count : max_ul_ue_count;
  max_count       = std::max(max_count, last_candidate_count);
}

// class intra_slice_scheduler

intra_slice_scheduler::intra_slice_scheduler(const scheduler_ue_expert_config& expert_cfg_,
                                             ue_repository&                    ues,
                                             pdcch_resource_allocator&         pdcch_alloc,
                                             uci_allocator&                    uci_alloc_,
                                             cell_resource_allocator&          cell_alloc_,
                                             cell_metrics_handler&             cell_metrics_,
                                             cell_harq_manager&                cell_harqs_,
                                             srslog::basic_logger&             logger_) :
  expert_cfg(expert_cfg_),
  cell_alloc(cell_alloc_),
  cell_metrics(cell_metrics_),
  cell_harqs(cell_harqs_),
  uci_alloc(uci_alloc_),
  logger(logger_),
  expected_pdschs_per_slot(compute_expected_pdschs_per_slot(cell_alloc.cfg)),
  ue_alloc(expert_cfg, ues, pdcch_alloc, uci_alloc, cell_alloc_, logger_)
{
  // Pre-reserve memory for UE candidates.
  newtx_candidates.reserve(MAX_NOF_DU_UES);
  pending_dl_newtxs.reserve(MAX_UE_PDUS_PER_SLOT);
  pending_ul_newtxs.reserve(MAX_UE_PDUS_PER_SLOT);

  // Pre-reserve memory for slice context.
  const unsigned max_expected_ran_slices = 8;
  slice_ctxt_list.reserve(max_expected_ran_slices);
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
  if (slice.get_slice_ues().empty()) {
    return;
  }

  if (slice.get_slot_tx() != pdsch_slot) {
    pdsch_slot = slice.get_slot_tx();
    update_used_dl_vrbs(slice);
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
  if (slice.get_slice_ues().empty()) {
    return;
  }

  if (slice.get_slot_tx() != pusch_slot) {
    pusch_slot = slice.get_slot_tx();
    update_used_ul_vrbs(slice);
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
                                                                      const vrb_bitmap&              used_vrbs,
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
  ues_to_alloc =
      std::min({ues_to_alloc, std::max(static_cast<unsigned>(ue_candidates.size()) / 4U, 1U), MAX_UE_GRANT_PER_SLOT});

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
  max_nof_rbs = std::min(max_nof_rbs, static_cast<unsigned>((~used_vrbs).count()));
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
    auto result =
        ue_alloc.allocate_dl_grant(ue_retx_dl_grant_request{u, pdsch_slot, h, used_dl_vrbs, enable_pdsch_interleaving});
    if (not result.has_value() and result.error() == dl_alloc_failure_cause::skip_slot) {
      // Received signal to stop allocations in the slot.
      break;
    }

    if (result.has_value()) {
      vrb_interval alloc_vrbs = result.value();
      used_dl_vrbs.fill(alloc_vrbs.start(), alloc_vrbs.stop());
      slice.store_grant(alloc_vrbs.length());
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
    auto result = ue_alloc.allocate_ul_grant(ue_retx_ul_grant_request{u, pusch_slot, h, used_ul_vrbs});
    if (not result.has_value() and result.error() == alloc_status::skip_slot) {
      // Received signal to stop allocations in the slot.
      break;
    }

    if (result.has_value()) {
      vrb_interval alloc_vrbs = result.value();
      used_ul_vrbs.fill(alloc_vrbs.start(), alloc_vrbs.stop());
      slice.store_grant(alloc_vrbs.length());
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
  // Build list of UE candidates for newTx.
  if (not slice_ctxt_list.contains(slice.id())) {
    slice_ctxt_list.emplace(slice.id(), *this);
  }
  auto& slice_sched = slice_ctxt_list[slice.id()];
  slice_sched.fill_ue_dl_candidate_group(newtx_candidates, slice);
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
  // Build list of UE candidates for newTx.
  if (not slice_ctxt_list.contains(slice.id())) {
    slice_ctxt_list.emplace(slice.id(), *this);
  }
  auto& slice_sched = slice_ctxt_list[slice.id()];
  slice_sched.fill_ue_ul_candidate_group(newtx_candidates, slice);
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
  auto [rbs_to_alloc, max_rbs_per_grant] = get_max_grants_and_rb_grant_size(
      newtx_candidates, cell_alloc, slice, used_dl_vrbs, std::min(max_ue_grants_to_alloc, expected_pdschs_per_slot));
  if (max_rbs_per_grant == 0) {
    return 0;
  }

  // Stage 1: Pre-select UEs with the highest priority and reserve control-plane space for their DL grants.
  unsigned rb_count                   = 0;
  bool     pucch_grant_limit_exceeded = false;
  for (const auto& ue_candidate : newtx_candidates) {
    if (pucch_grant_limit_exceeded) {
      // The PUCCH is likely saturated and there is no space for new PUCCHs.
      // As an heuristic, we only allocate DL grants to UEs which already have a PUCCH or a PUSCH in a future slot that
      // can likely accommodate more HARQ-ACK bits.
      if (not(ue_candidate.ue_cc->harqs.last_ack_slot().valid() and
              ue_candidate.ue_cc->harqs.last_ack_slot() > pdsch_slot) and
          not(ue_candidate.ue_cc->harqs.last_pusch_slot().valid() and
              ue_candidate.ue_cc->harqs.last_pusch_slot() > pdsch_slot)) {
        continue;
      }
    }

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
    } else if (result.error() == dl_alloc_failure_cause::skip_slot) {
      // Received signal to stop allocations in the slot. Move to stage 2.
      break;
    } else if (result.error() == dl_alloc_failure_cause::uci_alloc_failed) {
      // The scheduler likely ran out of PUCCH resources. Prioritize UEs which have a UCI already pending.
      pucch_grant_limit_exceeded = true;
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
    vrb_interval alloc_vrbs = grant_builder.recommended_vrbs(used_dl_vrbs, max_grant_size);
    if (alloc_vrbs.empty()) {
      logger.warning("ue={} c-rnti={}: Failed to allocate RBs for PDSCH grant at slot={}",
                     fmt::underlying(grant_builder.ue().ue_index()),
                     grant_builder.ue().crnti(),
                     slice.get_slot_tx());
      // We let the grant be empty. It will be skipped in the post-processing scheduling step.
      grant_builder.set_pdsch_params({}, {}, enable_pdsch_interleaving);
      continue;
    }

    // Compute the corresponding CRBs.
    static constexpr search_space_id      ue_ded_ss_id = to_search_space_id(2);
    const auto&                           ss_info      = grant_builder.ue().get_cc().cfg().search_space(ue_ded_ss_id);
    std::pair<crb_interval, crb_interval> alloc_crbs;
    if (enable_pdsch_interleaving) {
      const auto prbs = ss_info.interleaved_mapping.value().vrb_to_prb(alloc_vrbs);
      alloc_crbs      = {prb_to_crb(ss_info.dl_crb_lims, prbs.first), prb_to_crb(ss_info.dl_crb_lims, prbs.second)};
    } else {
      alloc_crbs = {prb_to_crb(ss_info.dl_crb_lims, alloc_vrbs.convert_to<prb_interval>()), {}};
    }

    // Save CRBs, MCS and RI.
    grant_builder.set_pdsch_params(alloc_vrbs, alloc_crbs, enable_pdsch_interleaving);

    // Fill used VRBs.
    const unsigned nof_rbs_alloc = alloc_vrbs.length();
    used_dl_vrbs.fill(alloc_vrbs.start(), alloc_vrbs.stop());

    // Update slice state.
    slice.store_grant(nof_rbs_alloc);
    rb_count += nof_rbs_alloc;
    rbs_missing = (max_grant_size - nof_rbs_alloc);
  }

  // Clear grant builders.
  const unsigned alloc_count = pending_dl_newtxs.size();
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
      get_max_grants_and_rb_grant_size(newtx_candidates, cell_alloc, slice, used_ul_vrbs, max_ue_grants_to_alloc);
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
    vrb_interval alloc_vrbs = grant_builder.recommended_vrbs(used_ul_vrbs, max_grant_size);
    if (alloc_vrbs.empty()) {
      logger.error("ue={} c-rnti={}: Failed to allocate RBs for PUSCH grant at slot={}",
                   fmt::underlying(grant_builder.ue().ue_index()),
                   grant_builder.ue().crnti(),
                   slice.get_slot_tx());
      // We let the grant be empty. It will be skipped in the post-processing scheduling step.
      grant_builder.set_pusch_params({});
      continue;
    }

    // Save CRBs, MCS and RI.
    grant_builder.set_pusch_params(alloc_vrbs);

    // Fill used CRBs.
    unsigned nof_rbs_alloc = alloc_vrbs.length();
    used_ul_vrbs.fill(alloc_vrbs.start(), alloc_vrbs.stop());

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
  return ue_cc.is_pdsch_enabled(pdcch_slot, pdsch_slot);
}

bool intra_slice_scheduler::can_allocate_pusch(const slice_ue& u, const ue_cell& ue_cc) const
{
  // Check if PDCCH/PUSCH are possible for the provided slots (e.g. not in UL slot or measGap)
  return ue_cc.is_pusch_enabled(pdcch_slot, pusch_slot);
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
      logger.info(
          "ue={} rnti={} PDSCH allocation skipped. Cause: All the HARQs are allocated and waiting for their "
          "respective HARQ-ACK. Check if any HARQ-ACK went missing in the lower layers or is arriving too late to "
          "the scheduler.",
          fmt::underlying(ue_cc.ue_index),
          ue_cc.rnti());
      cell_metrics.handle_late_dl_harqs();
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
      logger.info("ue={} rnti={} PUSCH allocation skipped. Cause: All the UE HARQs are busy waiting for "
                  "their respective CRC result. Check if any CRC PDU went missing in the lower layers or is "
                  "arriving too late to the scheduler.",
                  fmt::underlying(ue_cc.ue_index),
                  ue_cc.rnti());
      cell_metrics.handle_late_ul_harqs();
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
  const auto& pdsch_res = cell_alloc[pdsch_slot].result;
  pdschs_to_alloc = std::min(pdschs_to_alloc, static_cast<int>(MAX_UE_PDUS_PER_SLOT - pdsch_res.dl.ue_grants.size()));
  if (pdschs_to_alloc <= 0) {
    return 0;
  }

  // Determine how many PDCCHs can be allocated in this slot.
  const auto& pdcch_res  = cell_alloc[pdcch_slot].result;
  const int   max_pdcchs = std::min(static_cast<int>(MAX_DL_PDCCH_PDUS_PER_SLOT - pdcch_res.dl.dl_pdcchs.size()),
                                  static_cast<int>(expert_cfg.max_pdcch_alloc_attempts_per_slot - dl_attempts_count));
  pdschs_to_alloc        = std::min(pdschs_to_alloc, max_pdcchs);
  if (pdschs_to_alloc <= 0) {
    return 0;
  }

  // Determine how many PDSCHs can be allocated in this slot.
  const int max_pdschs =
      std::min(static_cast<int>(MAX_PDSCH_PDUS_PER_SLOT), static_cast<int>(expert_cfg.max_pdschs_per_slot));
  int allocated_pdschs = pdsch_res.dl.ue_grants.size() + pdsch_res.dl.bc.sibs.size() + pdsch_res.dl.rar_grants.size() +
                         pdsch_res.dl.paging_grants.size();
  pdschs_to_alloc = std::min(pdschs_to_alloc, max_pdschs - allocated_pdschs);
  if (pdschs_to_alloc <= 0) {
    return 0;
  }

  // Assume at least one RB per UE.
  pdschs_to_alloc = std::min(pdschs_to_alloc, static_cast<int>(slice.remaining_rbs()));

  return std::max(pdschs_to_alloc, 0);
}

unsigned intra_slice_scheduler::max_puschs_to_alloc(const ul_ran_slice_candidate& slice)
{
  if (not cell_alloc.cfg.is_ul_enabled(slice.get_slot_tx())) {
    return 0;
  }

  // We cannot allocate more than the number of UEs available.
  int puschs_to_alloc = slice.get_slice_ues().size();

  // Determine how many PUSCHs can be allocated in this slot.
  const auto& pusch_res = cell_alloc[pusch_slot].result;
  // The max PUSCHs-per-slot limit cannot be exceeded.
  // Note: We use signed integer to avoid unsigned overflow.
  const int max_puschs = std::min(static_cast<unsigned>(MAX_PUSCH_PDUS_PER_SLOT), expert_cfg.max_puschs_per_slot);
  puschs_to_alloc      = std::min(puschs_to_alloc, max_puschs - static_cast<int>(pusch_res.ul.puschs.size()));
  if (puschs_to_alloc <= 0) {
    return 0;
  }

  // The max UL grants-per-slot limit cannot be exceeded.
  puschs_to_alloc = std::min(
      puschs_to_alloc,
      static_cast<int>(expert_cfg.max_ul_grants_per_slot - pusch_res.ul.puschs.size() - pusch_res.ul.pucchs.size()));
  if (puschs_to_alloc <= 0) {
    return 0;
  }

  // Determine how many PDCCHs can be allocated in this slot.
  const auto& pdcch_res  = cell_alloc[pdcch_slot].result;
  const auto  max_pdcchs = std::min(static_cast<int>(MAX_UL_PDCCH_PDUS_PER_SLOT - pdcch_res.ul.puschs.size()),
                                   static_cast<int>(expert_cfg.max_pdcch_alloc_attempts_per_slot - ul_attempts_count));
  puschs_to_alloc        = std::min(puschs_to_alloc, max_pdcchs);
  if (puschs_to_alloc <= 0) {
    return 0;
  }

  // Assume at least one RB per UE.
  puschs_to_alloc = std::min(puschs_to_alloc, static_cast<int>(slice.remaining_rbs()));

  return std::max(puschs_to_alloc, 0);
}

void intra_slice_scheduler::update_used_dl_vrbs(const dl_ran_slice_candidate& slice)
{
  // The mapping from CRBs to VRBs is different for UEs configured with a common SearchSpace that uses DCI 1_0.
  // [Implementation defined] We assume all the UEs of this cell are configured the same way in that regard.
  const slice_ue_repository&       slice_ues    = slice.get_slice_ues();
  static constexpr search_space_id ue_ded_ss_id = to_search_space_id(2);
  const search_space_info&         ss_info      = slice_ues.begin()->get_cc().cfg().search_space(ue_ded_ss_id);

  // [Implementation defined] We use the common PDSCH TD resources as a reference for the computation of RBs
  // unavailable for PDSCH. This assumes that these resources are not colliding with PDCCH.
  const auto&              init_dl_bwp      = cell_alloc.cfg.dl_cfg_common.init_dl_bwp;
  const ofdm_symbol_range& symbols_to_check = init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].symbols;

  enable_pdsch_interleaving = ss_info.interleaved_mapping.has_value();
  if (enable_pdsch_interleaving) {
    for (const auto& pdsch : cell_alloc[pdsch_slot].result.dl.ue_grants) {
      // [Implementation defined] Disable interleaving if there are non-interleaved PDSCH grants already allocated on
      // that slot, to avoid wasting resources.
      if (pdsch.pdsch_cfg.vrb_prb_mapping == vrb_to_prb::mapping_type::non_interleaved) {
        enable_pdsch_interleaving = false;
        break;
      }
    }
  }

  prb_bitmap used_prbs = cell_alloc[pdsch_slot].dl_res_grid.used_prbs(
      init_dl_bwp.generic_params.scs, ss_info.dl_crb_lims, symbols_to_check);
  // Mark as used the PRBs that fall outside the CRB limits.
  const auto prb_lims = crb_to_prb(ss_info.dl_crb_lims,
                                   slice_ues.begin()->get_cc().cfg().cell_cfg_common.expert_cfg.ue.pdsch_crb_limits &
                                       ss_info.dl_crb_lims);
  if (not prb_lims.empty()) {
    used_prbs.fill(0, prb_lims.start());
    used_prbs.fill(prb_lims.stop(), used_prbs.size());
  }

  // Perform inverse VRB-to-PRB mapping when interleaving is enabled for this slice/BWP.
  if (enable_pdsch_interleaving) {
    const auto& interleaved_mapping = ss_info.interleaved_mapping.value();
    used_dl_vrbs                    = interleaved_mapping.prb_to_vrb(used_prbs);
  } else {
    used_dl_vrbs = used_prbs.convert_to<vrb_bitmap>();
  }
}

void intra_slice_scheduler::update_used_ul_vrbs(const ul_ran_slice_candidate& slice)
{
  // The mapping from CRBs to VRBs is different for UEs configured with a common SearchSpace that uses DCI 1_0.
  // [Implementation defined] We assume all the UEs of this cell are configured the same way in that regard.
  const slice_ue_repository&       slice_ues   = slice.get_slice_ues();
  static constexpr search_space_id ue_ded_ss   = to_search_space_id(2);
  const search_space_info&         ss_info     = slice_ues.begin()->get_cc().cfg().search_space(ue_ded_ss);
  const auto&                      ul_crb_lims = ss_info.ul_crb_lims;

  // (Implementation-defined) We use the common PUSCH TD resources as a reference for the computation of RBs unavailable
  // for PDSCH. This assumes that these resources are not colliding with SRS.
  const auto&              init_ul_bwp      = cell_alloc.cfg.ul_cfg_common.init_ul_bwp;
  const ofdm_symbol_range& symbols_to_check = init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].symbols;

  used_ul_vrbs = cell_alloc[pusch_slot]
                     .ul_res_grid.used_prbs(init_ul_bwp.generic_params.scs, ul_crb_lims, symbols_to_check)
                     .convert_to<vrb_bitmap>();
}
