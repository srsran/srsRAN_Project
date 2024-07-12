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

#include "ue_fallback_scheduler.h"
#include "../support/csi_rs_helpers.h"
#include "../support/dci_builder.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdsch/pdsch_resource_allocation.h"
#include "../support/prbs_calculator.h"
#include "../support/pusch/pusch_td_resource_indices.h"
#include "srsran/mac/mac_pdu_format.h"
#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

ue_fallback_scheduler::ue_fallback_scheduler(const scheduler_ue_expert_config& expert_cfg_,
                                             const cell_configuration&         cell_cfg_,
                                             pdcch_resource_allocator&         pdcch_sch_,
                                             pucch_allocator&                  pucch_alloc_,
                                             ue_repository&                    ues_) :
  expert_cfg(expert_cfg_),
  cell_cfg(cell_cfg_),
  pdcch_sch(pdcch_sch_),
  pucch_alloc(pucch_alloc_),
  ues(ues_),
  initial_active_dl_bwp(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params),
  ss_cfg(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common
             .search_spaces[cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id]),
  cs_cfg(cell_cfg.get_common_coreset(ss_cfg.get_coreset_id())),
  logger(srslog::fetch_basic_logger("SCHED"))
{
  ongoing_ues_ack_retxs.reserve(MAX_NOF_DU_UES);
  // NOTE 1: We use a std::vector instead of a std::array because we can later on initialize the vector with the minimum
  // value of k1, passed through the expert config.
  // NOTE 2: Although the TS 38.213, Section 9.2.3 specifies that the k1 possible values are {1, ..., 8}, some UEs do
  // not handle well the value k1=8, which can result in collision between PRACH and PUCCH.
  const unsigned max_k1 = 7U;
  srsran_sanity_check(expert_cfg.min_k1 <= max_k1, "Invalid min_k1 value");
  for (unsigned k1_value = expert_cfg.min_k1; k1_value <= max_k1; ++k1_value) {
    dci_1_0_k1_values.push_back(k1_value);
  }
  slots_with_no_pdxch_space.fill(false);
}

void ue_fallback_scheduler::run_slot(cell_resource_allocator& res_alloc)
{
  // Update the HARQ processes of UE with ongoing transmissions to check which ones still need to be acked or
  // retransmitted.
  slot_point sched_ref_slot = res_alloc[0].slot;

  slot_indication(sched_ref_slot);

  // Reset the scheduling counters; this takes into account retxs and new txs.
  sched_attempts_cnt = 0;

  if (ues.empty()) {
    return;
  }

  // Although the scheduler can schedule on future slots (for DL), we run the scheduler only in DL enabled slots.
  if (not cell_cfg.is_dl_enabled(sched_ref_slot)) {
    return;
  }

  // Schedule DL retransmissions before new any other DL or UL transmissions.
  bool stop_dl_scheduling = not schedule_dl_retx(res_alloc);

  // Schedule UL (retx and new TX) before DL new TX, as the DL can take advantage from scheduling on next slots.
  schedule_ul_new_tx_and_retx(res_alloc);

  if (stop_dl_scheduling) {
    return;
  }

  // Schedule ConRes CE.
  if (not schedule_dl_conres_new_tx(res_alloc)) {
    return;
  }

  // Schedule SRB0 messages before SRB1, as we prioritize SRB0 over SRB1.
  if (not schedule_dl_new_tx_srb0(res_alloc)) {
    return;
  }

  // Keep SRB1 with lower priority than SRB0.
  schedule_dl_new_tx_srb1(res_alloc);
}

void ue_fallback_scheduler::handle_dl_buffer_state_indication_srb(du_ue_index_t ue_index,
                                                                  bool          is_srb0,
                                                                  slot_point    sl,
                                                                  unsigned      srb_buffer_bytes)
{
  if (not ues.contains(ue_index)) {
    logger.error("ue_index={} not found in the scheduler", ue_index);
    return;
  }

  auto ue_it = std::find_if(
      pending_dl_ues_new_tx.begin(), pending_dl_ues_new_tx.end(), [ue_index, is_srb0](const fallback_ue& ue) {
        return ue.ue_index == ue_index and (not ue.is_srb0.has_value() or ue.is_srb0.value() == is_srb0);
      });

  ue& u = ues[ue_index];

  if (ue_it != pending_dl_ues_new_tx.end()) {
    // This case can happen when ConRes indication is received first and then receive Msg4 indication from upper layers.
    if (not ue_it->is_srb0.has_value()) {
      ue_it->is_srb0                   = is_srb0;
      ue_it->pending_srb1_buffer_bytes = srb_buffer_bytes;
      return;
    }
    // Remove the UE from the pending UE list when the Buffer Status Update is 0.
    if (not u.has_pending_dl_newtx_bytes(is_srb0 ? LCID_SRB0 : LCID_SRB1)) {
      pending_dl_ues_new_tx.erase(ue_it);
    }
    // This case can happen only for SRB1; note that for SRB0 there is no segmentation and, when the UE already exists
    // in the fallback scheduler, the RLC buffer state update can only be 0.
    // For SRB1, due to the segmentation, we need to update the internal SRB1 buffer state.
    else if (not is_srb0) {
      update_srb1_buffer_after_rlc_bsu(ue_index, sl, srb_buffer_bytes);
    }
    return;
  }

  // The UE doesn't exist in the internal fallback scheduler list, add it.
  if (u.has_pending_dl_newtx_bytes(is_srb0 ? LCID_SRB0 : LCID_SRB1)) {
    pending_dl_ues_new_tx.push_back({ue_index, is_srb0, u.is_conres_ce_pending(), srb_buffer_bytes});
  }
}

void ue_fallback_scheduler::handle_conres_indication(du_ue_index_t ue_index)
{
  if (not ues.contains(ue_index)) {
    logger.error("ue_index={} not found in the scheduler", ue_index);
    return;
  }

  auto ue_it = std::find_if(pending_dl_ues_new_tx.begin(),
                            pending_dl_ues_new_tx.end(),
                            [ue_index](const fallback_ue& ue) { return ue.ue_index == ue_index; });

  // Entry for this UE already exists.
  if (ue_it != pending_dl_ues_new_tx.end()) {
    ue_it->is_conres_pending = true;
    return;
  }

  pending_dl_ues_new_tx.push_back({ue_index, std::nullopt, true, 0});
}

void ue_fallback_scheduler::handle_ul_bsr_indication(du_ue_index_t ue_index, const ul_bsr_indication_message& bsr_ind)
{
  if (not ues.contains(ue_index)) {
    logger.error("ue_index={} not found in the scheduler", ue_index);
    return;
  }

  const auto* srb_lcg_report =
      std::find_if(bsr_ind.reported_lcgs.begin(), bsr_ind.reported_lcgs.end(), [](const auto& lcg_report) {
        // We assume the LCG ID for SRB is 0.
        const lcg_id_t srb_lcg_id = uint_to_lcg_id(0U);
        return lcg_report.lcg_id == srb_lcg_id;
      });
  const unsigned bsr_bytes = srb_lcg_report != bsr_ind.reported_lcgs.end() ? srb_lcg_report->nof_bytes : 0U;

  auto ue_it = std::find(pending_ul_ues.begin(), pending_ul_ues.end(), ue_index);

  // For UL UEs, we don't keep an internal estimate of the UL traffic, but we rely on the UL logical channel manager for
  // that task. Thus, compared to the DL case, the only operation that is needed for the UL is the addition of a new UE
  // to the pending UL UEs list.
  if (ue_it == pending_ul_ues.end() and bsr_bytes != 0) {
    pending_ul_ues.emplace_back(ue_index);
  }
}

void ue_fallback_scheduler::handle_sr_indication(du_ue_index_t ue_index)
{
  if (not ues.contains(ue_index)) {
    logger.error("ue_index={} not found in the scheduler", ue_index);
    return;
  }

  auto ue_it = std::find(pending_ul_ues.begin(), pending_ul_ues.end(), ue_index);

  if (ue_it == pending_ul_ues.end()) {
    pending_ul_ues.emplace_back(ue_index);
  }
}

bool ue_fallback_scheduler::schedule_dl_retx(cell_resource_allocator& res_alloc)
{
  for (auto& next_ue_harq_retx : ongoing_ues_ack_retxs) {
    auto& u    = *ues.find(next_ue_harq_retx.ue_index);
    auto* h_dl = next_ue_harq_retx.h_dl;

    if (h_dl->has_pending_retx()) {
      std::optional<most_recent_tx_slots> most_recent_tx_ack = get_most_recent_slot_tx(u.ue_index);
      dl_sched_outcome outcome = schedule_dl_srb(res_alloc, u, h_dl, most_recent_tx_ack, next_ue_harq_retx.is_srb0);
      // This is the case of the scheduler reaching the maximum number of sched attempts.
      if (outcome == dl_sched_outcome::stop_dl_scheduling) {
        return false;
      }
    }
  }
  return true;
}

void ue_fallback_scheduler::schedule_ul_new_tx_and_retx(cell_resource_allocator& res_alloc)
{
  // Processes all the UL UE at once, including UE with new transmissions and UE with retransmissions.
  for (auto next_ue = pending_ul_ues.begin(); next_ue != pending_ul_ues.end();) {
    // next_ue must be in the ues list, else it would have been removed by the \ref slot_indication() function.
    auto&            u         = ues[*next_ue];
    ul_harq_process* h_ul_retx = u.get_pcell().harqs.find_pending_ul_retx();
    if (h_ul_retx == nullptr and not u.pending_ul_newtx_bytes()) {
      ++next_ue;
      continue;
    }
    ul_srb_sched_outcome outcome = schedule_ul_ue(res_alloc, u, h_ul_retx);
    if (outcome == ul_srb_sched_outcome::stop_ul_scheduling) {
      // If there is no PDCCH space, then stop the scheduling for all UL UEs.
      return;
    }
    ++next_ue;
  }
}

bool ue_fallback_scheduler::schedule_dl_conres_new_tx(cell_resource_allocator& res_alloc)
{
  auto next_ue = pending_dl_ues_new_tx.begin();
  while (next_ue != pending_dl_ues_new_tx.end()) {
    // The UE might have been deleted in the meantime, check if still exists.
    if (not ues.contains(next_ue->ue_index)) {
      next_ue = pending_dl_ues_new_tx.erase(next_ue);
      continue;
    }

    // Check whether UE has any pending ConRes CE to be scheduled.
    if (not next_ue->is_conres_pending) {
      ++next_ue;
      continue;
    }

    // Check whether Msg4 over SRB has been received or not.
    // NOTE: UE with both pending ConRes + Msg4 is handled by \c schedule_dl_new_tx_srb0 and \c schedule_dl_new_tx_srb1.
    if (next_ue->is_srb0.has_value()) {
      ++next_ue;
      continue;
    }

    auto& u = ues[next_ue->ue_index];
    if (not u.is_conres_ce_pending()) {
      ++next_ue;
      continue;
    }

    std::optional<most_recent_tx_slots> most_recent_tx_ack = get_most_recent_slot_tx(u.ue_index);
    dl_sched_outcome outcome = schedule_dl_srb(res_alloc, u, nullptr, most_recent_tx_ack, next_ue->is_srb0);
    if (outcome == dl_sched_outcome::success) {
      next_ue = pending_dl_ues_new_tx.erase(next_ue);
    } else if (outcome == dl_sched_outcome::next_ue) {
      ++next_ue;
    } else {
      // This is the case the DL fallback scheduler has reached the maximum number of scheduling attempts and the fnc
      // returns \ref stop_dl_scheduling.
      return false;
    }
  }
  return true;
}

bool ue_fallback_scheduler::schedule_dl_new_tx_srb0(cell_resource_allocator& res_alloc)
{
  for (auto next_ue = pending_dl_ues_new_tx.begin(); next_ue != pending_dl_ues_new_tx.end();) {
    // The UE might have been deleted in the meantime, check if still exists.
    if (not ues.contains(next_ue->ue_index)) {
      next_ue = pending_dl_ues_new_tx.erase(next_ue);
      continue;
    }

    // Check whether UE has any pending SRB0 data to be scheduled.
    if (not next_ue->is_srb0.has_value() or not next_ue->is_srb0.value()) {
      ++next_ue;
      continue;
    }

    auto&                               u                  = ues[next_ue->ue_index];
    std::optional<most_recent_tx_slots> most_recent_tx_ack = get_most_recent_slot_tx(u.ue_index);
    if (not u.has_pending_dl_newtx_bytes(LCID_SRB0)) {
      ++next_ue;
      continue;
    }

    dl_sched_outcome outcome = schedule_dl_srb(res_alloc, u, nullptr, most_recent_tx_ack, next_ue->is_srb0);
    if (outcome == dl_sched_outcome::success) {
      next_ue = pending_dl_ues_new_tx.erase(next_ue);
    } else if (outcome == dl_sched_outcome::next_ue) {
      ++next_ue;
    } else if (outcome == dl_sched_outcome::srb_pending) {
      next_ue->is_conres_pending = false;
      ++next_ue;
    } else {
      // This is the case the DL fallback scheduler has reached the maximum number of scheduling attempts and the fnc
      // returns \ref stop_dl_scheduling.
      return false;
    }
  }
  return true;
}

void ue_fallback_scheduler::schedule_dl_new_tx_srb1(cell_resource_allocator& res_alloc)
{
  for (auto next_ue = pending_dl_ues_new_tx.begin(); next_ue != pending_dl_ues_new_tx.end();) {
    // The UE might have been deleted in the meantime, check if still exists.
    if (not ues.contains(next_ue->ue_index)) {
      next_ue = pending_dl_ues_new_tx.erase(next_ue);
      continue;
    }

    // Check whether UE has any pending SRB1 data to be scheduled.
    if (not next_ue->is_srb0.has_value() or next_ue->is_srb0.value()) {
      ++next_ue;
      continue;
    }

    auto&                               u                  = ues[next_ue->ue_index];
    std::optional<most_recent_tx_slots> most_recent_tx_ack = get_most_recent_slot_tx(u.ue_index);
    // NOTE: Since SRB1 data can be segmented, it could happen that not all the SRB1 bytes are scheduled at once. The
    // scheduler will attempt to allocate those remaining bytes in the following slots. The policy we adopt in this
    // scheduler is to schedule first all possible grants to a given UE (to speed up the re-establishment and
    // re-configuration). Only after the SRB1 buffer of that UE is emptied, we move on to the next UE.
    if (not has_pending_bytes_for_srb1(u.ue_index)) {
      ++next_ue;
      continue;
    }

    dl_sched_outcome outcome = schedule_dl_srb(res_alloc, u, nullptr, most_recent_tx_ack, next_ue->is_srb0);
    if (outcome == dl_sched_outcome::success) {
      next_ue->is_conres_pending = false;
      // Move to the next UE ONLY IF the UE has no more pending bytes for SRB1. This is to give priority to the same UE,
      // if there are still some SRB1 bytes left in the buffer. At the next iteration, the scheduler will try again with
      // the same scheduler, but starting from the next available slot.
      if (not has_pending_bytes_for_srb1(u.ue_index)) {
        ++next_ue;
      }
    } else if (outcome == dl_sched_outcome::next_ue) {
      ++next_ue;
    } else {
      // This is the case the DL fallback scheduler has reached the maximum number of scheduling attempts and the fnc
      // returns \ref stop_dl_scheduling.
      return;
    }
  }
}

static slot_point get_next_srb_slot(const cell_configuration& cell_cfg, slot_point sl_tx)
{
  slot_point next_candidate_slot = sl_tx + 1U;

  // In FDD, we advance by 1 slot.
  if (not cell_cfg.is_tdd()) {
    return next_candidate_slot;
  }

  // In TDD, advance the slot until it's a DL slot.
  while (not cell_cfg.is_dl_enabled(next_candidate_slot)) {
    next_candidate_slot = next_candidate_slot + 1U;
  }
  return next_candidate_slot;
}

ue_fallback_scheduler::dl_sched_outcome
ue_fallback_scheduler::schedule_dl_srb(cell_resource_allocator&            res_alloc,
                                       ue&                                 u,
                                       dl_harq_process*                    h_dl_retx,
                                       std::optional<most_recent_tx_slots> most_recent_tx_ack_slots,
                                       std::optional<bool>                 is_srb0)
{
  const auto& bwp_cfg_common = cell_cfg.dl_cfg_common.init_dl_bwp;
  // Search valid PDSCH time domain resource.

  const bool is_retx = h_dl_retx != nullptr;

  // \ref sched_ref_slot is the slot that we take as reference for the scheduler, which is processed when calling the
  // slot_indication().
  // NOTE: we guarantee that \ref sched_ref_slot is a DL slot in the caller.
  // TODO: Make this compatible with k0 > 0.
  slot_point sched_ref_slot = res_alloc[0].slot;

  // This is to prevent the edge case of the scheduler trying to allocate an SRB PDSCH in the farthest possible slot
  // in the future when, in the same slot, there is already an SRB PDSCH allocated. This can happen, for example, if
  // there is a retransmission (previously) allocated at slot sched_ref_slot + max_dl_slots_ahead_sched, and then the
  // scheduler attempt to allocate a new TX on the same slot.
  if (most_recent_tx_ack_slots.has_value() and
      sched_ref_slot + max_dl_slots_ahead_sched <= most_recent_tx_ack_slots.value().most_recent_tx_slot) {
    return dl_sched_outcome::next_ue;
  }

  // \ref starting_slot is the slot from which the SRB0 starts scheduling this given UE. Assuming the UE was assigned
  // a PDSCH grant for SRB1 that was fragmented, we want to avoid allocating the second part of SRB1 in a PDSCH that
  // is scheduled for an earlier slot than the PDSCH of the first part of the SRB1.
  // NOTE: The \c most_recent_tx_slot is not necessarily more recent than sched_ref_slot; hence we need to check that
  // most_recent_tx_ack_slots.value().most_recent_tx_slot >= sched_ref_slot.
  slot_point starting_slot =
      most_recent_tx_ack_slots.has_value() and most_recent_tx_ack_slots.value().most_recent_tx_slot > sched_ref_slot
          ? most_recent_tx_ack_slots.value().most_recent_tx_slot
          : sched_ref_slot;

  for (slot_point next_slot = starting_slot; next_slot <= sched_ref_slot + max_dl_slots_ahead_sched;
       next_slot            = get_next_srb_slot(cell_cfg, next_slot)) {
    if (sched_attempts_cnt >= max_dl_sched_attempts) {
      return dl_sched_outcome::stop_dl_scheduling;
    }

    if (slots_with_no_pdxch_space[next_slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE]) {
      logger.debug("rnti={}: Skipping slot {}. Cause: no PDSCH/PDCCH space available", u.crnti, next_slot);
      continue;
    }

    auto                                offset_to_sched_ref_slot = static_cast<unsigned>(next_slot - sched_ref_slot);
    const cell_slot_resource_allocator& pdcch_alloc              = res_alloc[offset_to_sched_ref_slot];
    const cell_slot_resource_allocator& pdsch_alloc              = res_alloc[offset_to_sched_ref_slot];

    if ((not cell_cfg.is_dl_enabled(pdsch_alloc.slot)) or (not cell_cfg.is_dl_enabled(pdsch_alloc.slot))) {
      continue;
    }

    // Instead of looping through all pdsch_time_res_idx values, pick the one with the largest number of symbols that
    // fits within the current DL slots.
    std::optional<unsigned> time_res_idx =
        get_pdsch_time_res_idx(bwp_cfg_common.pdsch_common, pdsch_alloc.slot, h_dl_retx);
    if (not time_res_idx.has_value()) {
      continue;
    }

    const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(time_res_idx.value());
    srsran_sanity_check(pdsch_td_cfg.k0 == 0, "Fallback scheduler only supports k0=0");

    // We do not support multiplexing of PDSCH for SRB0 and SRB1 when in fallback with CSI-RS.
    const bool is_csi_rs_slot = next_slot == sched_ref_slot ? not pdsch_alloc.result.dl.csi_rs.empty()
                                                            : csi_helper::is_csi_rs_slot(cell_cfg, pdsch_alloc.slot);
    if (is_csi_rs_slot) {
      continue;
    }

    // Verify there is space in PDSCH and PDCCH result lists for new allocations.
    if (pdcch_alloc.result.dl.dl_pdcchs.full() or pdsch_alloc.result.dl.ue_grants.full()) {
      logger.debug("rnti={}: Failed to allocate PDSCH for {}. Cause: No space available in scheduler output list",
                   u.crnti,
                   not is_srb0.has_value() ? "ConRes CE"
                   : *is_srb0              ? "SRB0 PDU"
                                           : "SRB1 PDU");
      slots_with_no_pdxch_space[next_slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = true;
      continue;
    }

    // Check whether PDSCH for the UE already exists or not.
    const auto* ue_pdsch_exists_it =
        std::find_if(pdsch_alloc.result.dl.ue_grants.begin(),
                     pdsch_alloc.result.dl.ue_grants.end(),
                     [rnti = u.crnti](const dl_msg_alloc& pdsch) { return pdsch.pdsch_cfg.rnti == rnti; });
    if (ue_pdsch_exists_it != pdsch_alloc.result.dl.ue_grants.end()) {
      return dl_sched_outcome::next_ue;
    }

    // As it is not possible to schedule a PDSCH whose related PUCCH falls in a slot that is the same as or older than
    // the most recent already scheduled ACK slot (for the same UE), whenever we detect this is the case we skip the
    // allocation in advance.
    slot_point most_recent_ack_slot = pdsch_alloc.slot;
    if (most_recent_tx_ack_slots.has_value()) {
      if (pdsch_alloc.slot + dci_1_0_k1_values.back() <= most_recent_tx_ack_slots.value().most_recent_ack_slot) {
        continue;
      }
      most_recent_ack_slot = most_recent_tx_ack_slots.value().most_recent_ack_slot;
    }

    sched_srb_results sched_res;
    if (not is_srb0.has_value()) {
      sched_res = schedule_dl_conres_ce(
          u, res_alloc, time_res_idx.value(), offset_to_sched_ref_slot, most_recent_ack_slot, h_dl_retx);
    } else {
      if (is_srb0.value()) {
        sched_res = schedule_dl_srb0(
            u, res_alloc, time_res_idx.value(), offset_to_sched_ref_slot, most_recent_ack_slot, h_dl_retx);
      } else {
        sched_res = schedule_dl_srb1(u,
                                     sched_ref_slot,
                                     res_alloc,
                                     time_res_idx.value(),
                                     offset_to_sched_ref_slot,
                                     most_recent_ack_slot,
                                     h_dl_retx);
      }
    }

    const bool alloc_successful = sched_res.h_dl != nullptr;
    if (alloc_successful) {
      if (not is_retx) {
        // The srb1_payload_bytes is meaningful only for SRB1.
        store_harq_tx(u.ue_index, sched_res.h_dl, sched_res.nof_srb1_scheduled_bytes, is_srb0);
      }
      if (sched_res.is_srb_data_pending) {
        return dl_sched_outcome::srb_pending;
      }
      return dl_sched_outcome::success;
    }

    ++sched_attempts_cnt;
  }

  // No resource found in UE's carriers and Search spaces.
  slot_point pdcch_slot = res_alloc[0].slot;
  logger.debug("rnti={}: Skipped {} allocation in slots:[{},{}). Cause: no PDCCH/PDSCH/PUCCH resources available",
               u.crnti,
               not is_srb0.has_value() ? "ConRes CE"
               : *is_srb0              ? "SRB0 PDU"
                                       : "SRB1 PDU",
               pdcch_slot,
               pdcch_slot + max_dl_slots_ahead_sched + 1);
  return dl_sched_outcome::next_ue;
}

ue_fallback_scheduler::sched_srb_results
ue_fallback_scheduler::schedule_dl_conres_ce(ue&                      u,
                                             cell_resource_allocator& res_alloc,
                                             unsigned                 pdsch_time_res,
                                             unsigned                 slot_offset,
                                             slot_point               most_recent_ack_slot,
                                             dl_harq_process*         h_dl_retx)
{
  ue_cell&                                     ue_pcell     = u.get_pcell();
  const subcarrier_spacing                     scs          = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(pdsch_time_res);

  const bool is_retx = h_dl_retx != nullptr;

  // Search for empty HARQ.
  dl_harq_process* h_dl = is_retx ? h_dl_retx : ue_pcell.harqs.find_empty_dl_harq();
  if (h_dl == nullptr) {
    logger.warning("rnti={}: UE must have empty HARQs during ConRes CE allocation", u.crnti);
    return {};
  }

  dci_dl_rnti_config_type dci_type =
      is_retx ? h_dl_retx->last_alloc_params().dci_cfg_type : dci_dl_rnti_config_type::tc_rnti_f1_0;
  srsran_assert(dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0,
                "Only DCI 1_0 with TC-RNTI is supported for ConRes CE scheduling");

  pdsch_config_params pdsch_cfg = get_pdsch_config_f1_0_tc_rnti(cell_cfg, pdsch_td_cfg);

  cell_slot_resource_allocator& pdsch_alloc = res_alloc[slot_offset + pdsch_td_cfg.k0];
  auto cset0_crbs_lim = pdsch_helper::get_ra_crb_limits_common(cell_cfg.dl_cfg_common.init_dl_bwp, ss_cfg.get_id());
  prb_bitmap used_crbs =
      pdsch_alloc.dl_res_grid.used_crbs(initial_active_dl_bwp.scs, cset0_crbs_lim, pdsch_cfg.symbols);

  sch_prbs_tbs  prbs_tbs{};
  sch_mcs_index mcs_idx = 0;
  if (is_retx) {
    // Use the same MCS, nof PRBs and TBS as the last allocation.
    mcs_idx            = h_dl->last_alloc_params().tb[0]->mcs;
    prbs_tbs.nof_prbs  = h_dl->last_alloc_params().rbs.type1().length();
    prbs_tbs.tbs_bytes = h_dl->last_alloc_params().tb[0].value().tbs_bytes;
  } else {
    // Fetch the pending MAC Contention Resolution CEs bytes.
    const unsigned pending_bytes = u.pending_conres_ce_bytes();

    crb_interval unused_crbs =
        rb_helper::find_next_empty_interval(used_crbs, cset0_crbs_lim.start(), cset0_crbs_lim.stop());

    if (unused_crbs.empty()) {
      logger.debug(
          "rnti={}: Postponed ConRes CE scheduling for slot {}. Cause: No space in PDSCH.", u.crnti, pdsch_alloc.slot);
      // If there is no free PRBs left on this slot for this UE, then this slot should be avoided by the other UEs
      // too.
      slots_with_no_pdxch_space[pdsch_alloc.slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = true;
      return {};
    }

    // Try to find least MCS to fit ConRes CE.
    while (mcs_idx <= expert_cfg.max_msg4_mcs) {
      // As per TS 38.214, clause 5.1.3.1, if the PDSCH is scheduled by a PDCCH with CRC scrambled by TC-RNTI, the UE
      // use "Table 5.1.3.1-1: MCS index table 1" for MCS mapping. This is not stated explicitly, but can be inferred
      // from the sentence "... the UE shall use I_MCS and Table 5.1.3.1-1 to determine the modulation order (Qm) and
      // Target code rate (R) used in the physical downlink shared channel.".

      // At this point, xOverhead is not configured yet. As per TS 38.214, Clause 5.1.3.2, xOverhead is assumed to be
      // 0.
      const sch_mcs_description mcs_config = pdsch_mcs_get_config(pdsch_mcs_table::qam64, mcs_idx);
      prbs_tbs                             = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                         static_cast<unsigned>(pdsch_cfg.symbols.length()),
                                                         calculate_nof_dmrs_per_rb(pdsch_cfg.dmrs),
                                                         pdsch_cfg.nof_oh_prb,
                                                         mcs_config,
                                                         pdsch_cfg.nof_layers});
      if (unused_crbs.length() >= prbs_tbs.nof_prbs) {
        break;
      }
      ++mcs_idx;
    }

    if (prbs_tbs.tbs_bytes < pending_bytes) {
      logger.debug(
          "rnti={}: ConRes CE size ({}) exceeds TBS calculated ({})", pending_bytes, prbs_tbs.tbs_bytes, u.crnti);
      return {};
    }

    if (mcs_idx > expert_cfg.max_msg4_mcs) {
      logger.debug(
          "rnti={}: Postponing ConRes CE allocation. Cause: MCS index chosen ({}) for ConRes CE exceeds maximum"
          " allowed MCS index ({})",
          u.crnti,
          mcs_idx,
          expert_cfg.max_msg4_mcs);
      return {};
    }
  }

  crb_interval ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, prbs_tbs.nof_prbs, 0);
  if (ue_grant_crbs.length() < prbs_tbs.nof_prbs) {
    logger.debug("rnti={}: Postponed ConRes CE scheduling. Cause: Not enough PRBs ({} < {})",
                 u.crnti,
                 ue_grant_crbs.length(),
                 prbs_tbs.nof_prbs);
    return {};
  }

  // Allocate PDCCH resources.
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[slot_offset];
  pdcch_dl_information*         pdcch =
      pdcch_sch.alloc_dl_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.get_id(), aggregation_level::n4);
  if (pdcch == nullptr) {
    logger.debug(
        "rnti={}: Postponed ConRes CE scheduling for slot={}. Cause: No space in PDCCH.", u.crnti, pdsch_alloc.slot);
    // If there is no PDCCH space on this slot for this UE, then this slot should be avoided by the other UEs too.
    slots_with_no_pdxch_space[pdsch_alloc.slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = true;
    return {};
  }

  // Allocate PUCCH resources.
  unsigned k1 = dci_1_0_k1_values.front();
  // Minimum k1 value supported is 4.
  std::optional<unsigned> pucch_res_indicator;
  for (const auto k1_candidate : dci_1_0_k1_values) {
    // Skip k1 values that would result in a PUCCH transmission in a slot that is older than the most recent ACK slot.
    if (pdsch_alloc.slot + k1_candidate <= most_recent_ack_slot) {
      continue;
    }
    pucch_res_indicator =
        is_retx ? pucch_alloc.alloc_common_and_ded_harq_res(
                      res_alloc, u.crnti, u.get_pcell().cfg(), slot_offset + pdsch_td_cfg.k0, k1_candidate, *pdcch)
                : pucch_alloc.alloc_common_pucch_harq_ack_ue(
                      res_alloc, u.crnti, slot_offset + pdsch_td_cfg.k0, k1_candidate, *pdcch);
    if (pucch_res_indicator.has_value()) {
      k1 = k1_candidate;
      break;
    }
  }
  if (not pucch_res_indicator.has_value()) {
    logger.debug("rnti={}: Failed to allocate PDSCH for ConRes CE for slot={}. Cause: No space in PUCCH",
                 u.crnti,
                 pdsch_alloc.slot);
    pdcch_sch.cancel_last_pdcch(pdcch_alloc);
    return {};
  }

  // Mark resources as occupied in the Resource grid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, ue_grant_crbs});

  fill_dl_srb_grant(u,
                    pdsch_alloc.slot,
                    *h_dl,
                    *pdcch,
                    dci_type,
                    pdsch_alloc.result.dl.ue_grants.emplace_back(),
                    pucch_res_indicator.value(),
                    pdsch_time_res,
                    k1,
                    mcs_idx,
                    ue_grant_crbs,
                    pdsch_cfg,
                    prbs_tbs.tbs_bytes,
                    is_retx,
                    std::nullopt);

  // No need to pass the nof SRB scheduled bytes.
  return sched_srb_results{.h_dl = h_dl};
}

ue_fallback_scheduler::sched_srb_results ue_fallback_scheduler::schedule_dl_srb0(ue&                      u,
                                                                                 cell_resource_allocator& res_alloc,
                                                                                 unsigned         pdsch_time_res,
                                                                                 unsigned         slot_offset,
                                                                                 slot_point       most_recent_ack_slot,
                                                                                 dl_harq_process* h_dl_retx)
{
  ue_cell&                                     ue_pcell     = u.get_pcell();
  const subcarrier_spacing                     scs          = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(pdsch_time_res);

  const bool is_retx = h_dl_retx != nullptr;

  ue_fallback_scheduler::sched_srb_results result{};

  // Search for empty HARQ.
  dl_harq_process* h_dl = is_retx ? h_dl_retx : ue_pcell.harqs.find_empty_dl_harq();
  if (h_dl == nullptr) {
    logger.warning("rnti={}: UE must have empty HARQs during SRB0 PDU allocation", u.crnti);
    return {};
  }

  dci_dl_rnti_config_type dci_type = dci_dl_rnti_config_type::c_rnti_f1_0;
  if (is_retx) {
    dci_type = h_dl->last_alloc_params().dci_cfg_type;
  } else if (u.is_conres_ce_pending()) {
    dci_type = dci_dl_rnti_config_type::tc_rnti_f1_0;
  }
  srsran_assert(dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0 or dci_type == dci_dl_rnti_config_type::c_rnti_f1_0,
                "Only DCI 1_0 with TC-RNTI or C-RNTI is supported for SRB0 scheduling");

  pdsch_config_params pdsch_cfg = dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0
                                      ? get_pdsch_config_f1_0_tc_rnti(cell_cfg, pdsch_td_cfg)
                                      : get_pdsch_config_f1_0_c_rnti(cell_cfg, nullptr, pdsch_td_cfg);

  // For DCI 1-0 scrambled with TC-RNTI, as per TS 38.213, Section 7.3.1.2.1, we should consider the size of CORESET#0
  // as the size for the BWP.
  // For DCI 1-0 scrambled with C-RNTI, if the DCI is monitored in a common search space and CORESET#0 is configured
  // for the cell, as per TS 38.213, Section 7.3.1.0, we should consider the size of CORESET#0  as the size for the
  // BWP.
  cell_slot_resource_allocator& pdsch_alloc = res_alloc[slot_offset + pdsch_td_cfg.k0];
  auto cset0_crbs_lim = pdsch_helper::get_ra_crb_limits_common(cell_cfg.dl_cfg_common.init_dl_bwp, ss_cfg.get_id());
  prb_bitmap used_crbs =
      pdsch_alloc.dl_res_grid.used_crbs(initial_active_dl_bwp.scs, cset0_crbs_lim, pdsch_cfg.symbols);

  sch_prbs_tbs  prbs_tbs{};
  sch_mcs_index mcs_idx = 0;
  if (is_retx) {
    // Use the same MCS, nof PRBs and TBS as the last allocation.
    mcs_idx            = h_dl->last_alloc_params().tb[0]->mcs;
    prbs_tbs.nof_prbs  = h_dl->last_alloc_params().rbs.type1().length();
    prbs_tbs.tbs_bytes = h_dl->last_alloc_params().tb[0].value().tbs_bytes;
  } else {
    // Required PRBs and TBS information for scheduling only ConRes CE in case PDSCH does not have enough space to
    // accommodate ConRes CE + SRB0.
    sch_prbs_tbs only_conres_prbs_tbs{};
    // MCS index to use for scheduling only ConRes CE in case PDSCH does not have enough space to accommodate ConRes CE
    // + SRB0.
    std::optional<sch_mcs_index> only_conres_mcs_idx;

    // Fetch the pending bytes.
    const unsigned only_conres_pending_bytes = u.pending_conres_ce_bytes();
    unsigned       pending_bytes             = u.pending_dl_newtx_bytes(LCID_SRB0) + only_conres_pending_bytes;

    crb_interval unused_crbs =
        rb_helper::find_next_empty_interval(used_crbs, cset0_crbs_lim.start(), cset0_crbs_lim.stop());

    if (unused_crbs.empty()) {
      logger.debug(
          "rnti={}: Postponed SRB0 PDU scheduling for slot {}. Cause: No space in PDSCH.", u.crnti, pdsch_alloc.slot);
      // If there is no free PRBs left on this slot for this UE, then this slot should be avoided by the other UEs
      // too.
      slots_with_no_pdxch_space[pdsch_alloc.slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = true;
      return {};
    }

    // Try to find least MCS to fit SRB0 message.
    while (mcs_idx <= expert_cfg.max_msg4_mcs) {
      // As per TS 38.214, clause 5.1.3.1, if the PDSCH is scheduled by a PDCCH with CRC scrambled by TC-RNTI, the UE
      // use "Table 5.1.3.1-1: MCS index table 1" for MCS mapping. This is not stated explicitly, but can be inferred
      // from the sentence "... the UE shall use I_MCS and Table 5.1.3.1-1 to determine the modulation order (Qm) and
      // Target code rate (R) used in the physical downlink shared channel.".

      // At this point, xOverhead is not configured yet. As per TS 38.214, Clause 5.1.3.2, xOverhead is assumed to be
      // 0.
      const sch_mcs_description mcs_config = pdsch_mcs_get_config(pdsch_mcs_table::qam64, mcs_idx);
      prbs_tbs                             = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                         static_cast<unsigned>(pdsch_cfg.symbols.length()),
                                                         calculate_nof_dmrs_per_rb(pdsch_cfg.dmrs),
                                                         pdsch_cfg.nof_oh_prb,
                                                         mcs_config,
                                                         pdsch_cfg.nof_layers});
      // If MCS index to use to schedule only ConRes CE is not set compute required PRBs and TBS information.
      // NOTE: The \c only_conres_prbs_tbs and \c only_conres_mcs_idx is used in case PDSCH does not have enough space
      // to accommodate ConRes CE + SRB0.
      if (u.is_conres_ce_pending() and not only_conres_mcs_idx.has_value()) {
        only_conres_prbs_tbs =
            get_nof_prbs(prbs_calculator_sch_config{only_conres_pending_bytes,
                                                    static_cast<unsigned>(pdsch_cfg.symbols.length()),
                                                    calculate_nof_dmrs_per_rb(pdsch_cfg.dmrs),
                                                    pdsch_cfg.nof_oh_prb,
                                                    mcs_config,
                                                    pdsch_cfg.nof_layers});
        if (unused_crbs.length() >= only_conres_prbs_tbs.nof_prbs) {
          only_conres_mcs_idx = mcs_idx;
        }
      }

      if (unused_crbs.length() >= prbs_tbs.nof_prbs) {
        break;
      }
      ++mcs_idx;
    }

    crb_interval ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, prbs_tbs.nof_prbs, 0);

    // Check whether SRB0 can be scheduled along with ConRes CE or not.
    if (u.is_conres_ce_pending() and (prbs_tbs.tbs_bytes < pending_bytes or mcs_idx > expert_cfg.max_msg4_mcs or
                                      ue_grant_crbs.length() < prbs_tbs.nof_prbs)) {
      // Schedule only ConRes CE.
      result.is_srb_data_pending = true;
      prbs_tbs                   = only_conres_prbs_tbs;
      mcs_idx                    = only_conres_mcs_idx.value();
      pending_bytes              = only_conres_pending_bytes;
    }

    if (prbs_tbs.tbs_bytes < pending_bytes) {
      logger.debug(
          "rnti={}: SRB0 PDU size ({}) exceeds TBS calculated ({})", pending_bytes, prbs_tbs.tbs_bytes, u.crnti);
      return {};
    }

    if (mcs_idx > expert_cfg.max_msg4_mcs) {
      logger.debug("rnti={}: Postponing SRB0 PDU allocation. Cause: MCS index chosen ({}) for SRB0 exceeds maximum"
                   " allowed MCS index ({})",
                   u.crnti,
                   mcs_idx,
                   expert_cfg.max_msg4_mcs);
      return {};
    }
  }

  crb_interval ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, prbs_tbs.nof_prbs, 0);
  if (ue_grant_crbs.length() < prbs_tbs.nof_prbs) {
    logger.debug("rnti={}: Postponed SRB0 PDU scheduling. Cause: Not enough PRBs ({} < {})",
                 u.crnti,
                 ue_grant_crbs.length(),
                 prbs_tbs.nof_prbs);
    return {};
  }

  // Allocate PDCCH resources.
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[slot_offset];
  pdcch_dl_information*         pdcch =
      pdcch_sch.alloc_dl_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.get_id(), aggregation_level::n4);
  if (pdcch == nullptr) {
    logger.debug(
        "rnti={}: Postponed SRB0 PDU scheduling for slot={}. Cause: No space in PDCCH.", u.crnti, pdsch_alloc.slot);
    // If there is no PDCCH space on this slot for this UE, then this slot should be avoided by the other UEs too.
    slots_with_no_pdxch_space[pdsch_alloc.slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = true;
    return {};
  }

  // Allocate PUCCH resources.
  unsigned k1 = dci_1_0_k1_values.front();
  // Minimum k1 value supported is 4.
  std::optional<unsigned> pucch_res_indicator;
  for (const auto k1_candidate : dci_1_0_k1_values) {
    // Skip k1 values that would result in a PUCCH transmission in a slot that is older than the most recent ACK slot.
    if (pdsch_alloc.slot + k1_candidate <= most_recent_ack_slot) {
      continue;
    }
    pucch_res_indicator =
        is_retx ? pucch_alloc.alloc_common_and_ded_harq_res(
                      res_alloc, u.crnti, u.get_pcell().cfg(), slot_offset + pdsch_td_cfg.k0, k1_candidate, *pdcch)
                : pucch_alloc.alloc_common_pucch_harq_ack_ue(
                      res_alloc, u.crnti, slot_offset + pdsch_td_cfg.k0, k1_candidate, *pdcch);
    if (pucch_res_indicator.has_value()) {
      k1 = k1_candidate;
      break;
    }
  }
  if (not pucch_res_indicator.has_value()) {
    logger.debug(
        "rnti={}: Failed to allocate PDSCH for SRB0 for slot={}. Cause: No space in PUCCH", u.crnti, pdsch_alloc.slot);
    pdcch_sch.cancel_last_pdcch(pdcch_alloc);
    return {};
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, ue_grant_crbs});

  std::optional<bool> is_srb0;
  if (not result.is_srb_data_pending) {
    is_srb0 = true;
  }
  fill_dl_srb_grant(u,
                    pdsch_alloc.slot,
                    *h_dl,
                    *pdcch,
                    dci_type,
                    pdsch_alloc.result.dl.ue_grants.emplace_back(),
                    pucch_res_indicator.value(),
                    pdsch_time_res,
                    k1,
                    mcs_idx,
                    ue_grant_crbs,
                    pdsch_cfg,
                    prbs_tbs.tbs_bytes,
                    is_retx,
                    is_srb0);

  // No need to pass the nof SRB scheduled bytes with SRB0.
  result.h_dl = h_dl;
  return result;
}

ue_fallback_scheduler::sched_srb_results ue_fallback_scheduler::schedule_dl_srb1(ue&        u,
                                                                                 slot_point sched_ref_slot,
                                                                                 cell_resource_allocator& res_alloc,
                                                                                 unsigned         pdsch_time_res,
                                                                                 unsigned         slot_offset,
                                                                                 slot_point       most_recent_ack_slot,
                                                                                 dl_harq_process* h_dl_retx)
{
  ue_cell&                                     ue_pcell     = u.get_pcell();
  const subcarrier_spacing                     scs          = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(pdsch_time_res);

  const bool is_retx = h_dl_retx != nullptr;

  // Search for empty HARQ.
  dl_harq_process* h_dl = is_retx ? h_dl_retx : ue_pcell.harqs.find_empty_dl_harq();
  if (h_dl == nullptr) {
    return {};
  }

  dci_dl_rnti_config_type dci_type = dci_dl_rnti_config_type::c_rnti_f1_0;
  if (is_retx) {
    dci_type = h_dl->last_alloc_params().dci_cfg_type;
  } else if (u.is_conres_ce_pending()) {
    dci_type = dci_dl_rnti_config_type::tc_rnti_f1_0;
  }
  srsran_assert(dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0 or dci_type == dci_dl_rnti_config_type::c_rnti_f1_0,
                "Only DCI 1_0 with TC-RNTI or C-RNTI is supported for SRB1 scheduling");

  pdsch_config_params pdsch_cfg = dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0
                                      ? get_pdsch_config_f1_0_tc_rnti(cell_cfg, pdsch_td_cfg)
                                      : get_pdsch_config_f1_0_c_rnti(cell_cfg, nullptr, pdsch_td_cfg);

  // For DCI 1-0 scrambled with TC-RNTI, as per TS 38.213, Section 7.3.1.2.1, we should consider the size of CORESET#0
  // as the size for the BWP.
  // For DCI 1-0 scrambled with C-RNTI, if the DCI is monitored in a common search space and CORESET#0 is configured
  // for the cell, as per TS 38.213, Section 7.3.1.0, we should consider the size of CORESET#0  as the size for the
  // BWP.
  cell_slot_resource_allocator& pdsch_alloc = res_alloc[slot_offset + pdsch_td_cfg.k0];
  prb_bitmap                    used_crbs   = pdsch_alloc.dl_res_grid.used_crbs(
      initial_active_dl_bwp.scs,
      pdsch_helper::get_ra_crb_limits_common(cell_cfg.dl_cfg_common.init_dl_bwp, ss_cfg.get_id()),
      pdsch_cfg.symbols);

  crb_interval ue_grant_crbs;
  sch_mcs_tbs  final_mcs_tbs;

  // Check if there is space on the PDSCH, and if not, skip this slot.
  auto cset0_crbs_lim = pdsch_helper::get_ra_crb_limits_common(cell_cfg.dl_cfg_common.init_dl_bwp, ss_cfg.get_id());
  crb_interval unused_crbs =
      rb_helper::find_next_empty_interval(used_crbs, cset0_crbs_lim.start(), cset0_crbs_lim.stop());

  if (unused_crbs.empty()) {
    logger.debug(
        "rnti={}: Postponed SRB1 PDU scheduling for slot={}. Cause: No space in PDSCH.", u.crnti, pdsch_alloc.slot);
    // If there is no free PRBs left on this slot for this UE, then this slot should be avoided by the other UEs too.
    slots_with_no_pdxch_space[pdsch_alloc.slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = true;
    return {};
  }

  const bool is_srb0 = false;
  if (is_retx) {
    const unsigned final_nof_prbs = h_dl->last_alloc_params().rbs.type1().length();
    final_mcs_tbs.mcs             = h_dl->last_alloc_params().tb.front().value().mcs;
    final_mcs_tbs.tbs             = h_dl->last_alloc_params().tb.front().value().tbs_bytes;

    ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, final_nof_prbs, 0);
    if (ue_grant_crbs.empty() or ue_grant_crbs.length() < final_nof_prbs) {
      return {};
    }

  } else {
    // Find available symbol x RB resources.
    const unsigned pending_bytes = get_srb1_pending_tot_bytes(u.ue_index);

    // If there is no free PRBs left on this slot for this UE, then this slot should be avoided by the other UEs too.
    grant_prbs_mcs mcs_prbs_estimate = ue_pcell.required_dl_prbs(pdsch_td_cfg, pending_bytes, dci_type);

    if (mcs_prbs_estimate.n_prbs == 0) {
      return {};
    }

    // [Implementation-defined] In case of partial slots and nof. PRBs allocated equals to 1 probability of KO is
    // high due to code not being able to cope with interference. So the solution is to increase the PRB allocation
    // to greater than 1 PRB.
    const bool set_min_nof_prbs = not cell_cfg.is_fully_dl_enabled(pdsch_alloc.slot) and mcs_prbs_estimate.n_prbs == 1;
    const unsigned min_nof_prbs_partial_slots = 2U;
    if (set_min_nof_prbs) {
      mcs_prbs_estimate.n_prbs = min_nof_prbs_partial_slots;
    }

    ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, mcs_prbs_estimate.n_prbs, 0);
    if (ue_grant_crbs.empty() or (set_min_nof_prbs and ue_grant_crbs.length() < min_nof_prbs_partial_slots)) {
      return {};
    }

    const unsigned final_nof_prbs = ue_grant_crbs.length();
    // As \c txDirectCurrentLocation, in \c SCS-SpecificCarrier, TS 38.331, "If this field (\c
    // txDirectCurrentLocation) is absent for downlink within ServingCellConfigCommon and ServingCellConfigCommonSIB,
    // the UE assumes the default value of 3300 (i.e. "Outside the carrier")".
    bool contains_dc = false;
    if (cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list.back().tx_direct_current_location.has_value()) {
      contains_dc = dc_offset_helper::is_contained(
          cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list.back().tx_direct_current_location.value(),
          ue_grant_crbs);
    }
    std::optional<sch_mcs_tbs> mcs_tbs =
        compute_dl_mcs_tbs(pdsch_cfg, mcs_prbs_estimate.mcs, final_nof_prbs, contains_dc);

    // If there is not MCS-TBS info, it means no MCS exists such that the effective code rate is <= 0.95.
    if (not mcs_tbs.has_value()) {
      logger.warning(
          "ue={} rnti={}: Failed to allocate PDSCH. Cause: no MCS such that code rate <= 0.95.", u.ue_index, u.crnti);
      return {};
    }

    // If ConRes CE is pending then ensure that there is enough RBs/TBS such that ConRes CE is not segmented.
    if (u.is_conres_ce_pending()) {
      const unsigned only_conres_ce_pending_bytes = u.pending_conres_ce_bytes();
      grant_prbs_mcs only_conres_mcs_prbs_estimate =
          ue_pcell.required_dl_prbs(pdsch_td_cfg, only_conres_ce_pending_bytes, dci_dl_rnti_config_type::tc_rnti_f1_0);
      if ((mcs_prbs_estimate.n_prbs < only_conres_mcs_prbs_estimate.n_prbs) or
          (mcs_tbs->tbs < (only_conres_ce_pending_bytes + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE))) {
        logger.debug("ue={} rnti={}: Postponed SRB1 PDU scheduling for slot={}. Cause: Grant is too small to fit even "
                     "ConRes CE.",
                     u.ue_index,
                     u.crnti);
        return {};
      }
    }

    final_mcs_tbs = mcs_tbs.value();
  }

  // Allocate PDCCH resources.
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[slot_offset];
  pdcch_dl_information*         pdcch =
      pdcch_sch.alloc_dl_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.get_id(), aggregation_level::n4);
  if (pdcch == nullptr) {
    logger.debug(
        "rnti={}: Postponed SRB1 PDU scheduling for slot={}. Cause: No space in PDCCH.", u.crnti, pdcch_alloc.slot);
    slots_with_no_pdxch_space[pdcch_alloc.slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = true;
    return {};
  }

  // Allocate PUCCH resources.
  unsigned k1 = dci_1_0_k1_values.front();
  // Minimum k1 value supported is 4.
  std::optional<unsigned> pucch_res_indicator;
  const bool              allocate_common_and_ded_pucch = dci_type != dci_dl_rnti_config_type::tc_rnti_f1_0 or is_retx;
  for (const auto k1_candidate : dci_1_0_k1_values) {
    // Skip the k1 values that would result in a PUCCH allocation that would overlap with the most recent ACK slot.
    if (pdsch_alloc.slot + k1_candidate <= most_recent_ack_slot) {
      continue;
    }
    pucch_res_indicator =
        allocate_common_and_ded_pucch
            ? pucch_alloc.alloc_common_and_ded_harq_res(
                  res_alloc, u.crnti, u.get_pcell().cfg(), slot_offset + pdsch_td_cfg.k0, k1_candidate, *pdcch)
            : pucch_alloc.alloc_common_pucch_harq_ack_ue(
                  res_alloc, u.crnti, slot_offset + pdsch_td_cfg.k0, k1_candidate, *pdcch);
    if (pucch_res_indicator.has_value()) {
      k1 = k1_candidate;
      break;
    }
  }
  if (not pucch_res_indicator.has_value()) {
    logger.debug(
        "rnti={}: Failed to allocate PDSCH for SRB1 for slot={}. Cause: No space in PUCCH", u.crnti, pdsch_alloc.slot);
    pdcch_sch.cancel_last_pdcch(pdcch_alloc);
    return {};
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, ue_grant_crbs});

  unsigned nof_srb1_scheduled_bytes = fill_dl_srb_grant(u,
                                                        pdsch_alloc.slot,
                                                        *h_dl,
                                                        *pdcch,
                                                        dci_type,
                                                        pdsch_alloc.result.dl.ue_grants.emplace_back(),
                                                        pucch_res_indicator.value(),
                                                        pdsch_time_res,
                                                        k1,
                                                        final_mcs_tbs.mcs,
                                                        ue_grant_crbs,
                                                        pdsch_cfg,
                                                        final_mcs_tbs.tbs,
                                                        is_retx,
                                                        is_srb0);

  return sched_srb_results{.h_dl = h_dl, .nof_srb1_scheduled_bytes = nof_srb1_scheduled_bytes};
}

unsigned ue_fallback_scheduler::fill_dl_srb_grant(ue&                        u,
                                                  slot_point                 pdsch_slot,
                                                  dl_harq_process&           h_dl,
                                                  pdcch_dl_information&      pdcch,
                                                  dci_dl_rnti_config_type    dci_type,
                                                  dl_msg_alloc&              msg,
                                                  unsigned                   pucch_res_indicator,
                                                  unsigned                   pdsch_time_res,
                                                  unsigned                   k1,
                                                  sch_mcs_index              mcs_idx,
                                                  const crb_interval&        ue_grant_crbs,
                                                  const pdsch_config_params& pdsch_params,
                                                  unsigned                   tbs_bytes,
                                                  bool                       is_retx,
                                                  std::optional<bool>        is_srb0)
{
  // Allocate DL HARQ.
  // NOTE: We do not multiplex the SRB1 PUCCH with existing PUCCH HARQs, thus both DAI and HARQ-ACK bit index are 0.
  static constexpr uint8_t srb_dai = 0;
  if (not is_retx) {
    const bool is_fallback = true;
    h_dl.new_tx(pdsch_slot,
                k1,
                expert_cfg.max_nof_harq_retxs,
                srb_dai,
                u.get_pcell().channel_state_manager().get_wideband_cqi(),
                pdsch_params.nof_layers,
                is_fallback);
  } else {
    const unsigned harq_bit_idx = 0U;
    h_dl.new_retx(pdsch_slot, k1, harq_bit_idx);
  }

  // Fill DL PDCCH DCI.
  static const uint8_t msg4_rv = 0;
  switch (dci_type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0: {
      build_dci_f1_0_tc_rnti(pdcch.dci,
                             cell_cfg.dl_cfg_common.init_dl_bwp,
                             ue_grant_crbs,
                             pdsch_time_res,
                             k1,
                             pucch_res_indicator,
                             mcs_idx,
                             msg4_rv,
                             h_dl);
      break;
    }
    case dci_dl_rnti_config_type::c_rnti_f1_0: {
      build_dci_f1_0_c_rnti(pdcch.dci,
                            u.get_pcell().cfg().search_space(pdcch.ctx.context.ss_id),
                            cell_cfg.dl_cfg_common.init_dl_bwp,
                            ue_grant_crbs,
                            pdsch_time_res,
                            k1,
                            pucch_res_indicator,
                            srb_dai,
                            mcs_idx,
                            msg4_rv,
                            h_dl);
      break;
    }
    default: {
      srsran_assert(false, "Invalid DCI type for SRB1");
    }
  }

  // Fill PDSCH PDU.
  msg.context.ue_index    = u.ue_index;
  msg.context.k1          = k1;
  msg.context.ss_id       = pdcch.ctx.context.ss_id;
  msg.context.nof_retxs   = h_dl.tb(0).nof_retxs;
  msg.context.olla_offset = 0;

  unsigned srb1_bytes_allocated = 0;
  switch (dci_type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0: {
      build_pdsch_f1_0_tc_rnti(msg.pdsch_cfg,
                               pdsch_params,
                               tbs_bytes,
                               u.crnti,
                               cell_cfg,
                               pdcch.dci.tc_rnti_f1_0,
                               ue_grant_crbs,
                               not is_retx);

      // Set MAC logical channels to schedule in this PDU.
      if (not is_retx) {
        u.build_dl_fallback_transport_block_info(msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes);
        if (is_srb0.has_value() and not is_srb0.value()) {
          auto* msg_lcid_it =
              std::find_if(msg.tb_list.back().lc_chs_to_sched.begin(),
                           msg.tb_list.back().lc_chs_to_sched.end(),
                           [](const dl_msg_lc_info& lc_info) { return lc_info.lcid == lcid_t::LCID_SRB1; });
          srb1_bytes_allocated = msg_lcid_it != msg.tb_list.back().lc_chs_to_sched.end() ? msg_lcid_it->sched_bytes : 0;
          // Update the internal state of the SRB1 buffer for LCID-1 bytes left to transmit (exclude overhead).
          update_srb1_buffer_state_after_alloc(u.ue_index, srb1_bytes_allocated);
        }
      }
      break;
    }
    case dci_dl_rnti_config_type::c_rnti_f1_0: {
      srsran_assert(is_srb0.has_value(),
                    "Invalid DCI type={} used for scheduling ConRes CE only",
                    dci_dl_rnti_config_rnti_type(dci_type));
      build_pdsch_f1_0_c_rnti(msg.pdsch_cfg,
                              pdsch_params,
                              tbs_bytes,
                              u.crnti,
                              cell_cfg,
                              u.get_pcell().cfg().search_space(pdcch.ctx.context.ss_id),
                              pdcch.dci.c_rnti_f1_0,
                              ue_grant_crbs,
                              not is_retx);
      // Set MAC logical channels to schedule in this PDU.
      if (not is_retx) {
        u.build_dl_fallback_transport_block_info(msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes);
        if (is_srb0.has_value() and not is_srb0.value()) {
          auto* mcs_lcid_it =
              std::find_if(msg.tb_list.back().lc_chs_to_sched.begin(),
                           msg.tb_list.back().lc_chs_to_sched.end(),
                           [](const dl_msg_lc_info& lc_info) { return lc_info.lcid == lcid_t::LCID_SRB1; });
          srb1_bytes_allocated = mcs_lcid_it != msg.tb_list.back().lc_chs_to_sched.end() ? mcs_lcid_it->sched_bytes : 0;
          // Update the internal state of the SRB1 buffer for LCID-1 bytes left to transmit (exclude overhead).
          update_srb1_buffer_state_after_alloc(u.ue_index, srb1_bytes_allocated);
        }
      }
      break;
    }
    default: {
      srsran_assert(false, "Invalid DCI type for SRB0 or SRB1");
    }
  }

  // Save in HARQ the parameters set for this PDCCH and PDSCH PDUs.
  h_dl.save_alloc_params(dl_harq_sched_context{pdcch.dci.type}, msg.pdsch_cfg, true);

  return srb1_bytes_allocated;
}

ue_fallback_scheduler::ul_srb_sched_outcome
ue_fallback_scheduler::schedule_ul_ue(cell_resource_allocator& res_alloc, ue& u, ul_harq_process* h_ul_retx)
{
  // The caller ensures the slot is Ul enabled.
  const cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  slot_point                          pdcch_slot  = pdcch_alloc.slot;

  // Fetch applicable PUSCH Time Domain resource index list.
  static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS> pusch_td_res_index_list =
      get_pusch_td_resource_indices(cell_cfg, pdcch_slot);

  bool is_retx = h_ul_retx != nullptr;

  if (is_retx) {
    srsran_sanity_check(h_ul_retx->last_tx_params().dci_cfg_type == dci_ul_rnti_config_type::c_rnti_f0_0,
                        "Invalid DCI type for UL retransmission for fallback UE");
  }
  static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP> search_spaces =
      u.get_pcell().get_active_ul_search_spaces(pdcch_slot, dci_ul_rnti_config_type::c_rnti_f0_0);

  for (const auto* ss : search_spaces) {
    for (unsigned pusch_td_res_idx : pusch_td_res_index_list) {
      const pusch_time_domain_resource_allocation& pusch_td    = ss->pusch_time_domain_list[pusch_td_res_idx];
      cell_slot_resource_allocator&                pusch_alloc = res_alloc[pusch_td.k2 + cell_cfg.ntn_cs_koffset];
      const slot_point                             pusch_slot  = pusch_alloc.slot;

      if (not cell_cfg.is_ul_enabled(pusch_slot)) {
        continue;
      }

      const unsigned start_ul_symbols =
          NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.get_nof_ul_symbol_per_slot(pusch_slot);

      // If it is a retx, we need to ensure we use a time_domain_resource with the same number of symbols as used for
      // the first transmission.
      const bool sym_length_match_prev_grant_for_retx =
          is_retx ? pusch_td.symbols.length() == h_ul_retx->last_tx_params().nof_symbols : true;
      if (pusch_td.symbols.start() < start_ul_symbols or
          pusch_td.symbols.stop() > (start_ul_symbols + cell_cfg.get_nof_ul_symbol_per_slot(pusch_slot)) or
          !sym_length_match_prev_grant_for_retx) {
        // Try next PUSCH time domain resource value.
        continue;
      }

      if (pusch_alloc.result.ul.puschs.full() or pdcch_alloc.result.dl.ul_pdcchs.full()) {
        logger.warning(
            "ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: No space available in scheduler output list",
            u.ue_index,
            u.crnti,
            pusch_alloc.slot);
        continue;
      }

      // When checking the number of remaining grants for PUSCH, take into account that the PUCCH grants for this UE
      // will be removed when multiplexing the UCI on PUSCH.
      if (pusch_alloc.result.ul.puschs.size() >=
          expert_cfg.max_ul_grants_per_slot - static_cast<unsigned>(pusch_alloc.result.ul.pucchs.size())) {
        logger.info("ue={} rnti={}: Failed to allocate PUSCH. Cause: Max number of UL grants per slot {} was reached.",
                    u.ue_index,
                    u.crnti,
                    expert_cfg.max_puschs_per_slot);
        continue;
      }

      const bool existing_pusch =
          pusch_alloc.result.ul.puschs.end() !=
          std::find_if(pusch_alloc.result.ul.puschs.begin(),
                       pusch_alloc.result.ul.puschs.end(),
                       [rnti = u.crnti](const ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == rnti; });

      if (existing_pusch) {
        // Only one PUSCH per UE per slot.
        continue;
      }

      auto* existing_pucch = std::find_if(pusch_alloc.result.ul.pucchs.begin(),
                                          pusch_alloc.result.ul.pucchs.end(),
                                          [rnti = u.crnti](const pucch_info& pucch) { return pucch.crnti == rnti; });
      auto  existing_pucch_count =
          std::count_if(pusch_alloc.result.ul.pucchs.begin(),
                        pusch_alloc.result.ul.pucchs.end(),
                        [rnti = u.crnti](const pucch_info& pucch) { return pucch.crnti == rnti; });

      // [Implementation-defined]
      // Given that we don't support multiplexing of UCI on PUSCH at this point, removal of an existing PUCCH grant
      // requires careful consideration since we can have multiple PUCCH grants. Following are the possible options:
      //
      // - PUCCH common only (very unlikely, but a possibility)
      // - PUCCH common (1 HARQ bit) + 1 PUCCH F1 dedicated (1 HARQ bit)
      // - PUCCH common (1 HARQ bit) + 2 PUCCH F1 dedicated (1 HARQ bit, 1 HARQ bit + SR bit)
      // - PUCCH common (1 HARQ bit) + 1 PUCCH F2 dedicated (with CSI + HARQ bit)
      // - PUCCH F1 dedicated only (SR bit)
      // - PUCCH F2 dedicated only (CSI)
      //
      // We remove PUCCH grant only if there exists only ONE PUCCH grant, and it's a PUCCH F1 dedicated with only SR
      // bit.
      if (existing_pucch_count > 0) {
        if (existing_pucch_count == 1 and existing_pucch->format == pucch_format::FORMAT_1 and
            existing_pucch->format_1.sr_bits != sr_nof_bits::no_sr and
            existing_pucch->format_1.harq_ack_nof_bits == 0) {
          pusch_alloc.result.ul.pucchs.erase(existing_pucch);
        } else {
          // No PUSCH in slots with PUCCH.
          continue;
        }
      }

      ul_srb_sched_outcome outcome = schedule_ul_srb(u, res_alloc, pusch_td_res_idx, pusch_td, h_ul_retx);
      if (outcome != ul_srb_sched_outcome::next_slot) {
        return outcome;
      }
    }
  }
  return ul_srb_sched_outcome::next_ue;
}

ue_fallback_scheduler::ul_srb_sched_outcome
ue_fallback_scheduler::schedule_ul_srb(ue&                                          u,
                                       cell_resource_allocator&                     res_alloc,
                                       unsigned                                     pusch_time_res,
                                       const pusch_time_domain_resource_allocation& pusch_td,
                                       ul_harq_process*                             h_ul_retx)
{
  ue_cell&                      ue_pcell    = u.get_pcell();
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  cell_slot_resource_allocator& pusch_alloc = res_alloc[pusch_td.k2 + cell_cfg.ntn_cs_koffset];

  const crb_interval init_ul_bwp_crbs = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs;

  const prb_bitmap used_crbs = pusch_alloc.ul_res_grid.used_crbs(
      cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, init_ul_bwp_crbs, pusch_td.symbols);

  const bool is_retx = h_ul_retx != nullptr;

  // Search for empty HARQ.
  ul_harq_process* h_ul = is_retx ? h_ul_retx : ue_pcell.harqs.find_empty_ul_harq();
  if (h_ul == nullptr) {
    logger.debug("ue={} rnti={} PUSCH allocation skipped. Cause: no HARQ available", u.ue_index, u.crnti);
    return ul_srb_sched_outcome::next_ue;
  }

  if (used_crbs.all()) {
    logger.debug("ue={} rnti={} PUSCH allocation skipped. Cause: No more RBs available", u.ue_index, u.crnti);
    return ul_srb_sched_outcome::next_slot;
  }

  // In fallback, we do not multiplex any HARQ-ACK or CSI within the PUSCH.
  const unsigned      uci_bits_overallocation = 0U;
  const bool          is_csi_report_slot      = false;
  pusch_config_params pusch_cfg               = get_pusch_config_f0_0_c_rnti(
      cell_cfg, nullptr, cell_cfg.ul_cfg_common.init_ul_bwp, pusch_td, uci_bits_overallocation, is_csi_report_slot);

  crb_interval ue_grant_crbs;
  sch_mcs_tbs  final_mcs_tbs;
  if (is_retx) {
    const unsigned final_nof_prbs = h_ul->last_tx_params().rbs.type1().length();
    final_mcs_tbs.mcs             = h_ul->last_tx_params().mcs;
    final_mcs_tbs.tbs             = h_ul->last_tx_params().tbs_bytes;

    ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, final_nof_prbs, 0);
    if (ue_grant_crbs.empty() or ue_grant_crbs.length() < final_nof_prbs) {
      logger.debug("ue={} rnti={} PUSCH SRB allocation for re-tx skipped. Cause: available RBs {} < required RBs {}",
                   u.ue_index,
                   u.crnti,
                   ue_grant_crbs.length(),
                   final_nof_prbs);
      return ul_srb_sched_outcome::next_slot;
    }
  } else {
    pusch_mcs_table     fallback_mcs_table = pusch_mcs_table::qam64;
    sch_mcs_index       mcs                = ue_pcell.get_ul_mcs(fallback_mcs_table);
    sch_mcs_description ul_mcs_cfg         = pusch_mcs_get_config(fallback_mcs_table, mcs, false);

    unsigned pending_bytes = u.pending_ul_newtx_bytes();

    sch_prbs_tbs prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                                    pusch_td.symbols.length(),
                                                                    calculate_nof_dmrs_per_rb(pusch_cfg.dmrs),
                                                                    pusch_cfg.nof_oh_prb,
                                                                    ul_mcs_cfg,
                                                                    pusch_cfg.nof_layers});

    // Due to the pre-allocated UCI bits, MCS 0 and PRB 1 would not leave any space for the payload on the TBS, as
    // all the space would be taken by the UCI bits. As a result of this, the effective code rate would be 0 and the
    // allocation would fail and be postponed to the next slot.
    // [Implementation-defined] In our tests, we have seen that MCS 5 with 1 PRB can lead (depending on the
    // configuration) to a non-valid MCS-PRB allocation; therefore, we set 6 as minimum value for 1 PRB.
    // TODO: Remove this part and handle the problem with a loop that is general for any configuration.
    const sch_mcs_index min_mcs_for_1_prb  = static_cast<sch_mcs_index>(6U);
    const unsigned      min_allocable_prbs = 1U;
    if (mcs < min_mcs_for_1_prb and prbs_tbs.nof_prbs <= min_allocable_prbs) {
      ++prbs_tbs.nof_prbs;
    }

    ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, prbs_tbs.nof_prbs, 0);

    if (ue_grant_crbs.empty()) {
      logger.debug("ue={} rnti={} PUSCH allocation for SRB1 skipped. Cause: no PRBs available", u.ue_index, u.crnti);
      return ul_srb_sched_outcome::next_slot;
    }

    if (ue_grant_crbs.length() <= min_allocable_prbs and mcs < min_mcs_for_1_prb) {
      logger.debug("ue={} rnti={} PUSCH allocation for SRB1 skipped. Cause: the scheduler couldn't allocate the min. "
                   "number of PRBs={} for MCS={}",
                   u.ue_index,
                   u.crnti,
                   prbs_tbs.nof_prbs,
                   mcs.to_uint());
      return ul_srb_sched_outcome::next_slot;
    }

    bool contains_dc = dc_offset_helper::is_contained(
        cell_cfg.expert_cfg.ue.initial_ul_dc_offset, cell_cfg.nof_ul_prbs, ue_grant_crbs);

    std::optional<sch_mcs_tbs> mcs_tbs_info =
        compute_ul_mcs_tbs(pusch_cfg, nullptr, mcs, ue_grant_crbs.length(), contains_dc);

    // If there is not MCS-TBS info, it means no MCS exists such that the effective code rate is <= 0.95.
    if (not mcs_tbs_info.has_value()) {
      logger.warning("ue={} rnti={}: Failed to allocate PUSCH for SRB1. Cause: no MCS such that code rate <= 0.95",
                     u.ue_index,
                     u.crnti);
      return ul_srb_sched_outcome::next_slot;
    }
    final_mcs_tbs = mcs_tbs_info.value();
  }

  // Allocate PDCCH position.
  pdcch_ul_information* pdcch =
      pdcch_sch.alloc_ul_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.get_id(), aggregation_level::n4);
  if (pdcch == nullptr) {
    logger.info("ue={} rnti={}: Failed to allocate PUSCH. Cause: No space in PDCCH.", u.ue_index, u.crnti);
    return ul_srb_sched_outcome::stop_ul_scheduling;
  }

  // Mark resources as occupied in the ResourceGrid.
  pusch_alloc.ul_res_grid.fill(
      grant_info{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, pusch_td.symbols, ue_grant_crbs});

  fill_ul_srb_grant(u,
                    pdcch_alloc.slot,
                    *h_ul,
                    *pdcch,
                    pusch_alloc.result.ul.puschs.emplace_back(),
                    pusch_time_res,
                    pusch_td.k2,
                    final_mcs_tbs.mcs,
                    ue_grant_crbs,
                    pusch_cfg,
                    final_mcs_tbs.tbs,
                    is_retx);

  return ul_srb_sched_outcome::next_ue;
}

void ue_fallback_scheduler::fill_ul_srb_grant(ue&                        u,
                                              slot_point                 pdcch_slot,
                                              ul_harq_process&           h_ul,
                                              pdcch_ul_information&      pdcch,
                                              ul_sched_info&             msg,
                                              unsigned                   pusch_time_res,
                                              unsigned                   k2,
                                              sch_mcs_index              mcs_idx,
                                              const crb_interval&        ue_grant_crbs,
                                              const pusch_config_params& pusch_params,
                                              unsigned                   tbs_bytes,
                                              bool                       is_retx)
{
  if (is_retx) {
    // It is a retx.
    h_ul.new_retx(pdcch_slot + k2 + cell_cfg.ntn_cs_koffset);
  } else {
    // It is a new tx.
    h_ul.new_tx(pdcch_slot + k2 + cell_cfg.ntn_cs_koffset, expert_cfg.max_nof_harq_retxs);
  }

  uint8_t rv = u.get_pcell().get_pusch_rv(h_ul);
  build_dci_f0_0_c_rnti(pdcch.dci,
                        u.get_pcell().cfg().search_space(pdcch.ctx.context.ss_id),
                        cell_cfg.ul_cfg_common.init_ul_bwp,
                        ue_grant_crbs,
                        pusch_time_res,
                        mcs_idx,
                        rv,
                        h_ul);

  // Fill PDSCH PDU.
  msg.context.ue_index  = u.ue_index;
  msg.context.ss_id     = ss_cfg.get_id();
  msg.context.k2        = k2;
  msg.context.nof_retxs = h_ul.tb().nof_retxs;
  if (msg.context.nof_retxs == 0 and u.get_pcell().link_adaptation_controller().is_ul_olla_enabled()) {
    msg.context.olla_offset = u.get_pcell().link_adaptation_controller().ul_snr_offset_db();
  }
  build_pusch_f0_0_c_rnti(msg.pusch_cfg,
                          u.crnti,
                          pusch_params,
                          tbs_bytes,
                          cell_cfg,
                          cell_cfg.ul_cfg_common.init_ul_bwp,
                          pdcch.dci.c_rnti_f0_0,
                          ue_grant_crbs,
                          not is_retx);

  // Save set PDCCH and PUSCH PDU parameters in HARQ process.
  h_ul.save_alloc_params(ul_harq_sched_context{pdcch.dci.type}, msg.pusch_cfg);

  // In case there is a SR pending, reset it.
  u.reset_sr_indication();
}

unsigned ue_fallback_scheduler::has_pending_bytes_for_srb1(du_ue_index_t ue_idx) const
{
  return get_srb1_pending_tot_bytes(ue_idx) > 0U;
}

const pdsch_time_domain_resource_allocation& ue_fallback_scheduler::get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const
{
  return cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdsch_time_res_idx];
}

std::optional<unsigned> ue_fallback_scheduler::get_pdsch_time_res_idx(const pdsch_config_common& pdsch_cfg,
                                                                      slot_point                 sl_tx,
                                                                      const dl_harq_process*     h_dl_retx) const
{
  std::optional<unsigned> candidate_pdsch_time_res_idx;
  for (unsigned time_res_idx = 0; time_res_idx != pdsch_cfg.pdsch_td_alloc_list.size(); ++time_res_idx) {
    const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(time_res_idx);
    // Check whether PDSCH time domain resource does not overlap with CORESET.
    if (pdsch_td_cfg.symbols.start() < ss_cfg.get_first_symbol_index() + cs_cfg.duration) {
      continue;
    }

    // Check whether PDSCH time domain resource fits in DL symbols of the slot.
    if (pdsch_td_cfg.symbols.stop() > cell_cfg.get_nof_dl_symbol_per_slot(sl_tx)) {
      continue;
    }

    // For retransmissions, we want to make sure we use the same number of symbols as the original transmission.
    if (h_dl_retx != nullptr) {
      if (h_dl_retx->last_alloc_params().nof_symbols != pdsch_td_cfg.symbols.length()) {
        continue;
      }
      return candidate_pdsch_time_res_idx = time_res_idx;
    }
    // For new transmissions, we want to search for the PDSCH time domain resource with the largest number of symbols.
    if (candidate_pdsch_time_res_idx.has_value() and
        pdsch_td_cfg.symbols.length() < get_pdsch_td_cfg(candidate_pdsch_time_res_idx.value()).symbols.length()) {
      continue;
    }
    candidate_pdsch_time_res_idx = time_res_idx;
  }

  return candidate_pdsch_time_res_idx;
}

std::optional<ue_fallback_scheduler::most_recent_tx_slots>
ue_fallback_scheduler::get_most_recent_slot_tx(du_ue_index_t ue_idx) const
{
  std::optional<ue_fallback_scheduler::most_recent_tx_slots> most_recent_tx_ack_slot;
  for (const auto& ue_proc : ongoing_ues_ack_retxs) {
    if (ue_proc.ue_index == ue_idx) {
      slot_point h_dl_slot_tx  = ue_proc.h_dl->slot_tx();
      slot_point h_dl_slot_ack = ue_proc.h_dl->slot_ack();
      if (not most_recent_tx_ack_slot.has_value()) {
        most_recent_tx_ack_slot.emplace(
            most_recent_tx_slots{.most_recent_tx_slot = h_dl_slot_tx, .most_recent_ack_slot = h_dl_slot_ack});
        continue;
      }
      if (h_dl_slot_tx > most_recent_tx_ack_slot.value().most_recent_tx_slot) {
        most_recent_tx_ack_slot.value().most_recent_tx_slot = h_dl_slot_tx;
      }
      if (h_dl_slot_ack > most_recent_tx_ack_slot.value().most_recent_ack_slot) {
        most_recent_tx_ack_slot.value().most_recent_ack_slot = h_dl_slot_ack;
      }
    }
  }
  return most_recent_tx_ack_slot;
}

void ue_fallback_scheduler::store_harq_tx(du_ue_index_t       ue_index,
                                          dl_harq_process*    h_dl,
                                          unsigned            srb_payload_bytes,
                                          std::optional<bool> is_srb0)
{
  srsran_sanity_check(ongoing_ues_ack_retxs.end() ==
                          std::find_if(ongoing_ues_ack_retxs.begin(),
                                       ongoing_ues_ack_retxs.end(),
                                       [ue_index, h_dl](const ack_and_retx_tracker& tracker) {
                                         return tracker.match_ue_harq(ue_index, h_dl);
                                       }),
                      "This UE and HARQ process were already in the list");

  ongoing_ues_ack_retxs.emplace_back(ue_index, h_dl, ues, srb_payload_bytes, is_srb0);
}

unsigned ue_fallback_scheduler::get_srb1_pending_tot_bytes(du_ue_index_t ue_idx) const
{
  auto it = std::find_if(pending_dl_ues_new_tx.begin(), pending_dl_ues_new_tx.end(), [ue_idx](const fallback_ue& ue) {
    return ue.ue_index == ue_idx and ue.is_srb0.has_value() and not ue.is_srb0.value();
  });
  if (it == pending_dl_ues_new_tx.end()) {
    return 0U;
  }

  // Note: this function assumes the ues[ue_idx] exists, which is guaranteed by the caller.
  const unsigned mac_bytes = get_mac_sdu_required_bytes(it->pending_srb1_buffer_bytes);
  const unsigned ce_bytes  = ues[ue_idx].pending_ce_bytes();
  // Each RLC buffer state update includes extra 4 bytes compared to the number of bytes left to transmit; this is to
  // account for the segmentation overhead. Suppose that (i) we need to compute the PDSCH TBS to allocate the last
  // remaining SRB1 bytes and (ii) we expect an RLC buffer state update before the PDSCH will be transmitted. By
  // over-allocating 4 bytes to the MAC PDU, we ensure that the PDSCH will be able to fit all remaining bytes, including
  // the 4 extra bytes added by the latest RLC buffer state update.
  const unsigned overallocation_size = 4U;
  const unsigned overallocation      = mac_bytes != 0 ? overallocation_size : 0U;
  return mac_bytes + ce_bytes + overallocation;
}

void ue_fallback_scheduler::update_srb1_buffer_state_after_alloc(du_ue_index_t ue_idx, unsigned allocated_bytes)
{
  // Retrieve the UE from the list of UEs that need to be scheduled.
  auto ue_it =
      std::find_if(pending_dl_ues_new_tx.begin(), pending_dl_ues_new_tx.end(), [ue_idx](const fallback_ue& ue) {
        return ue.ue_index == ue_idx and ue.is_srb0.has_value() and not ue.is_srb0.value();
      });
  if (ue_it == pending_dl_ues_new_tx.end() or not ues.contains(ue_idx)) {
    return;
  }

  if (ue_it->pending_srb1_buffer_bytes > allocated_bytes) {
    ue_it->pending_srb1_buffer_bytes -= allocated_bytes;
  } else {
    ue_it->pending_srb1_buffer_bytes = 0U;
  }
}

void ue_fallback_scheduler::update_srb1_buffer_after_rlc_bsu(du_ue_index_t ue_idx,
                                                             slot_point    sl,
                                                             unsigned      buffer_status_report)
{
  // Retrieve the UE from the list of UEs that need to be scheduled.
  auto ue_it =
      std::find_if(pending_dl_ues_new_tx.begin(), pending_dl_ues_new_tx.end(), [ue_idx](const fallback_ue& ue) {
        return ue.ue_index == ue_idx and ue.is_srb0.has_value() and not ue.is_srb0.value();
      });
  if (ue_it == pending_dl_ues_new_tx.end() or not ues.contains(ue_idx)) {
    return;
  }

  unsigned srb1_buffer_bytes = buffer_status_report;

  // Remove the LCID-1 bytes that are already scheduled for future new transmissions, but yet to be transmitted, from
  // the RLC buffer state update received from upper layers.
  for (auto& ack_tracker : ongoing_ues_ack_retxs) {
    if (ack_tracker.ue_index != ue_idx or not ack_tracker.is_srb0.has_value() or
        (ack_tracker.is_srb0.has_value() and ack_tracker.is_srb0.value())) {
      continue;
    }
    if (ack_tracker.h_dl->is_waiting_ack() and ack_tracker.h_dl->slot_tx() >= sl and
        ack_tracker.h_dl->tb(0).nof_retxs == 0) {
      unsigned tx_ed_bytes = ack_tracker.srb1_payload_bytes;
      srb1_buffer_bytes    = srb1_buffer_bytes > tx_ed_bytes ? srb1_buffer_bytes - tx_ed_bytes : 0;
    }
  }

  ue_it->pending_srb1_buffer_bytes = srb1_buffer_bytes;
}

void ue_fallback_scheduler::slot_indication(slot_point sl)
{
  // If there is any skipped slot, reset \ref slots_with_no_pdxch_space for all the skipped slots.
  if (SRSRAN_LIKELY(last_sl_ind.valid())) {
    while (SRSRAN_UNLIKELY(last_sl_ind + 1 != sl)) {
      logger.info("UE fallback scheduler: Detected skipped slot={}.", last_sl_ind + 1);
      // Reset the flag that indicates that there are no resources for the slot that has passed.
      slots_with_no_pdxch_space[last_sl_ind.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = false;
      ++last_sl_ind;
    }
  }

  // Reset the flag that indicates that there are no resources for the slot that has passed.
  slots_with_no_pdxch_space[(sl - 1).to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = false;
  last_sl_ind                                                                     = sl;

  // Remove any DL UE that is no longer in fallback mode; this can happen in case of overallocation, or when the GNB
  // detects a false NACK from PUCCH. Existing the fallback mode is an indication that the fallback transmission was
  // completed successfully.
  for (auto ue_it = pending_dl_ues_new_tx.begin(); ue_it != pending_dl_ues_new_tx.end();) {
    if (not ues.contains(ue_it->ue_index)) {
      ue_it = pending_dl_ues_new_tx.erase(ue_it);
      continue;
    }
    // For SRB1, due to segmentation and to pre-allocation, the scheduler might not be able to estimate precisely when
    // the UE has received the SRB1 full buffer; we assume the UE has received the full SRB1 buffer data if UE exits
    // fallback mode. This is not needed for SRB0, which doesn't allow segmentation.
    if ((ue_it->is_srb0.has_value() and not ue_it->is_srb0.value()) and
        (not ues[ue_it->ue_index].get_pcell().is_in_fallback_mode())) {
      ue_it = pending_dl_ues_new_tx.erase(ue_it);
      continue;
    }
    ++ue_it;
  }

  // Only remove the {UE, HARQ-process} elements that have been retransmitted and positively acked. The rest of the
  // elements are potential candidate for retransmissions.
  for (auto it_ue_harq = ongoing_ues_ack_retxs.begin(); it_ue_harq != ongoing_ues_ack_retxs.end();) {
    if (not ues.contains(it_ue_harq->ue_index)) {
      it_ue_harq = ongoing_ues_ack_retxs.erase(it_ue_harq);
      continue;
    }

    // If the SRB1 UE is no longer in fallback mode, it's safe to cancel all ongoing HARQ processes.
    // For SRB1, due to segmentation and to pre-allocation, the scheduler might not be able to estimate precisely when
    // the UE has received the SRB1 full buffer; we assume the UE has received the full SRB1 buffer data if UE exits
    // fallback mode. This is not needed for SRB0, which doesn't allow segmentation.
    if ((it_ue_harq->is_srb0.has_value() and not it_ue_harq->is_srb0.value()) and
        (not ues[it_ue_harq->ue_index].get_pcell().is_in_fallback_mode())) {
      const unsigned tb_index = 0U;
      it_ue_harq->h_dl->cancel_harq_retxs(tb_index);
      it_ue_harq = ongoing_ues_ack_retxs.erase(it_ue_harq);
      continue;
    }

    dl_harq_process& h_dl = *it_ue_harq->h_dl;
    if (h_dl.empty()) {
      it_ue_harq = ongoing_ues_ack_retxs.erase(it_ue_harq);
      continue;
    }

    // If the HARQ process has the "fallback" flag set to false, it means that the HARQ process got reset by its
    // timeout, and in the meantime got reused by the non-fallback scheduler. In this case, it cannot be processed by
    // the fallback scheduler. NOTE: this very unlikely to happen, but not impossible under certain extreme conditions.
    if (not h_dl.last_alloc_params().is_fallback) {
      it_ue_harq = ongoing_ues_ack_retxs.erase(it_ue_harq);
      continue;
    }
    ++it_ue_harq;
  }

  // Remove UE when the SRB1 buffer status is empty and when there are no HARQ processes scheduled for future
  // transmissions.
  for (auto ue_it = pending_dl_ues_new_tx.begin(); ue_it != pending_dl_ues_new_tx.end();) {
    auto& ue = *ue_it;
    if (not ue.is_srb0.has_value() or ue.is_srb0.value()) {
      ++ue_it;
      continue;
    }

    // NOTE: the UEs that have pending RE-TXs are handled by the \ref ongoing_ues_ack_retxs and can be removed from
    // \ref pending_dl_ues_new_tx.
    const bool remove_ue = ue.pending_srb1_buffer_bytes == 0 and
                           ongoing_ues_ack_retxs.end() ==
                               std::find_if(ongoing_ues_ack_retxs.begin(),
                                            ongoing_ues_ack_retxs.end(),
                                            [ue_idx = ue.ue_index, sl](const ack_and_retx_tracker& tracker) {
                                              return tracker.ue_index == ue_idx and tracker.h_dl->is_waiting_ack() and
                                                     tracker.h_dl->tb(0).nof_retxs == 0 and
                                                     tracker.h_dl->slot_tx() >= sl;
                                            });

    if (remove_ue) {
      ue_it = pending_dl_ues_new_tx.erase(ue_it);
    } else {
      ++ue_it;
    }
  }

  // Remove UL UE if the UE has left fallback or if the UE has been deleted from the scheduler.
  for (auto ue_it = pending_ul_ues.begin(); ue_it != pending_ul_ues.end();) {
    if (not ues.contains(*ue_it)) {
      ue_it = pending_ul_ues.erase(ue_it);
      continue;
    }

    // Remove UE when it leaves the fallback.
    auto& ue = ues[*ue_it];
    if (not ue.get_pcell().is_in_fallback_mode()) {
      for (uint32_t h_id = 0; h_id != ue.get_pcell().harqs.nof_ul_harqs(); ++h_id) {
        ue.get_pcell().harqs.ul_harq(h_id).cancel_harq_retxs();
      }
      ue_it = pending_ul_ues.erase(ue_it);
      continue;
    }
    ++ue_it;
  }
}
