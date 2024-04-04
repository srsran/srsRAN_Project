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
#include "srsran/mac/mac_pdu_format.h"
#include "srsran/ran/sch/tbs_calculator.h"

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
  dci_1_0_k1_values = {4, 5, 6, 7};
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

  // Although the scheduler can schedule on future slots, we run the scheduler only in DL enabled slots.
  if (not cell_cfg.is_dl_enabled(sched_ref_slot)) {
    return;
  }

  for (auto& next_ue_harq_retx : ongoing_ues_ack_retxs) {
    auto& u    = *ues.find(next_ue_harq_retx.ue_index);
    auto* h_dl = next_ue_harq_retx.h_dl;

    if (h_dl->has_pending_retx()) {
      optional<most_recent_tx_slots> most_recent_tx_ack = get_most_recent_slot_tx(u.ue_index);
      sched_outcome outcome = schedule_srb(res_alloc, u, next_ue_harq_retx.is_srb0, h_dl, most_recent_tx_ack);
      // This is the case of the scheduler reaching the maximum number of sched attempts.
      if (outcome == sched_outcome::exit_scheduler) {
        return;
      }
    }
  }

  // Schedule SRB0 messages before SRB1, as we prioritize SRB0 over SRB1.
  for (auto next_ue = pending_ues.begin(); next_ue != pending_ues.end();) {
    if (not next_ue->is_srb0) {
      ++next_ue;
      continue;
    }

    // The UE might have been deleted in the meantime, check if still exists.
    if (not ues.contains(next_ue->ue_index)) {
      next_ue = pending_ues.erase(next_ue);
      continue;
    }

    auto&                          u                  = ues[next_ue->ue_index];
    optional<most_recent_tx_slots> most_recent_tx_ack = get_most_recent_slot_tx(u.ue_index);
    if (not u.has_pending_dl_newtx_bytes(LCID_SRB0)) {
      ++next_ue;
      continue;
    }

    sched_outcome outcome = schedule_srb(res_alloc, u, true, nullptr, most_recent_tx_ack);
    if (outcome == sched_outcome::success) {
      next_ue = pending_ues.erase(next_ue);
    } else if (outcome == sched_outcome::next_ue) {
      ++next_ue;
    } else {
      return;
    }
  }

  // Keep SRB1 with lower priority than SRB0.
  for (auto next_ue = pending_ues.begin(); next_ue != pending_ues.end();) {
    if (next_ue->is_srb0) {
      ++next_ue;
      continue;
    }

    // The UE might have been deleted in the meantime, check if still exists.
    if (not ues.contains(next_ue->ue_index)) {
      next_ue = pending_ues.erase(next_ue);
      continue;
    }

    auto&                          u                  = ues[next_ue->ue_index];
    optional<most_recent_tx_slots> most_recent_tx_ack = get_most_recent_slot_tx(u.ue_index);
    // NOTE: Since SRB1 data can be segmented, it could happen that not all the SRB1 bytes are scheduled at once. The
    // scheduler will attempt to allocate those remaining bytes in the following slots. The policy we adopt in this
    // scheduler is to schedule first all possible grants to a given UE (to speed up the re-establishment and
    // re-configuration). Only after the SRB1 buffer of that UE is emptied, we move on to the next UE.
    if (not has_pending_bytes_for_srb1(u.ue_index)) {
      ++next_ue;
      continue;
    }

    sched_outcome outcome = schedule_srb(res_alloc, u, false, nullptr, most_recent_tx_ack);
    if (outcome == sched_outcome::success) {
      // Move to the next UE ONLY IF the UE has no more pending bytes for SRB1. This is to give priority to the same UE,
      // if there are still some SRB1 bytes left in the buffer. At the next iteration, the scheduler will try again with
      // the same scheduler, but starting from the next available slot.
      if (not has_pending_bytes_for_srb1(u.ue_index)) {
        ++next_ue;
      }

    } else if (outcome == sched_outcome::next_ue) {
      ++next_ue;
    } else {
      return;
    }
  }
}

void ue_fallback_scheduler::handle_dl_buffer_state_indication_srb(du_ue_index_t ue_index,
                                                                  bool          is_srb0,
                                                                  slot_point    sl,
                                                                  unsigned      srb_buffer_bytes)
{
  auto ue_it = std::find_if(pending_ues.begin(), pending_ues.end(), [ue_index, is_srb0](const srb_ue& ue) {
    return ue.ue_index == ue_index and ue.is_srb0 == is_srb0;
  });

  srsran_sanity_check(ues.contains(ue_index), "UE doesn't exist in the scheduler");
  ue& u = ues[ue_index];

  if (ue_it != pending_ues.end()) {
    // Remove the UE from the pending UE list when the Buffer Status Update is 0.
    if (not u.has_pending_dl_newtx_bytes(is_srb0 ? LCID_SRB0 : LCID_SRB1)) {
      pending_ues.erase(ue_it);
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
    pending_ues.push_back({ue_index, is_srb0, srb_buffer_bytes});
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

ue_fallback_scheduler::sched_outcome
ue_fallback_scheduler::schedule_srb(cell_resource_allocator&       res_alloc,
                                    ue&                            u,
                                    bool                           is_srb0,
                                    dl_harq_process*               h_dl_retx,
                                    optional<most_recent_tx_slots> most_recent_tx_ack_slots)
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
    return sched_outcome::next_ue;
  }

  // \ref starting_slot is the slot from which the SRB0 starts scheduling this given UE. Assuming the UE was assigned
  // a PDSCH grant for SRB1 that was fragmented, we want to avoid allocating the second part of SRB1 in a PDSCH that
  // is scheduled for an earlier slot than the PDSCH of the first part of the SRB1. NOTE: The \c most_recent_tx_slot
  // is not necessarily more recent than sched_ref_slot; hence we need to check that
  // most_recent_tx_ack_slots.value().most_recent_tx_slot > sched_ref_slot.
  slot_point starting_slot =
      most_recent_tx_ack_slots.has_value() and most_recent_tx_ack_slots.value().most_recent_tx_slot > sched_ref_slot
          ? most_recent_tx_ack_slots.value().most_recent_tx_slot
          : sched_ref_slot;

  for (slot_point next_slot = starting_slot; next_slot <= sched_ref_slot + max_dl_slots_ahead_sched;
       next_slot            = get_next_srb_slot(cell_cfg, next_slot)) {
    if (sched_attempts_cnt >= max_sched_attempts) {
      return sched_outcome::exit_scheduler;
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
    optional<unsigned> time_res_idx = get_pdsch_time_res_idx(bwp_cfg_common.pdsch_common, pdsch_alloc.slot, h_dl_retx);
    if (not time_res_idx.has_value()) {
      continue;
    }

    const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(time_res_idx.value());
    srsran_sanity_check(pdsch_td_cfg.k0 == 0, "Fallback scheduler only only supports k0=0");

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
                   is_srb0 ? "SRB0" : "SRB1");
      slots_with_no_pdxch_space[next_slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = true;
      continue;
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

    sched_srb_results sched_res =
        is_srb0 ? schedule_srb0(
                      u, res_alloc, time_res_idx.value(), offset_to_sched_ref_slot, most_recent_ack_slot, h_dl_retx)
                : schedule_srb1(u,
                                sched_ref_slot,
                                res_alloc,
                                time_res_idx.value(),
                                offset_to_sched_ref_slot,
                                most_recent_ack_slot,
                                h_dl_retx);

    const bool alloc_successful = sched_res.h_dl != nullptr;
    if (alloc_successful) {
      if (not is_retx) {
        // The srb1_payload_bytes is meaningful only for SRB1.
        store_harq_tx(u.ue_index, sched_res.h_dl, is_srb0, sched_res.nof_srb1_scheduled_bytes);
      }
      return sched_outcome::success;
    }

    ++sched_attempts_cnt;
  }

  // No resource found in UE's carriers and Search spaces.
  slot_point pdcch_slot = res_alloc[0].slot;
  logger.debug("rnti={}: Skipped {} allocation in slots:[{},{}). Cause: no PDCCH/PDSCH/PUCCH resources available",
               u.crnti,
               is_srb0 ? "SRB0" : "SRB1",
               pdcch_slot,
               pdcch_slot + max_dl_slots_ahead_sched + 1);
  return sched_outcome::next_ue;
}

ue_fallback_scheduler::sched_srb_results ue_fallback_scheduler::schedule_srb0(ue&                      u,
                                                                              cell_resource_allocator& res_alloc,
                                                                              unsigned                 pdsch_time_res,
                                                                              unsigned                 slot_offset,
                                                                              slot_point       most_recent_ack_slot,
                                                                              dl_harq_process* h_dl_retx)
{
  ue_cell&                                     ue_pcell     = u.get_pcell();
  const subcarrier_spacing                     scs          = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(pdsch_time_res);

  pdsch_config_params pdsch_cfg = get_pdsch_config_f1_0_tc_rnti(cell_cfg, pdsch_td_cfg);

  const bool is_retx = h_dl_retx != nullptr;

  // Search for empty HARQ.
  dl_harq_process* h_dl = is_retx ? h_dl_retx : ue_pcell.harqs.find_empty_dl_harq();
  if (h_dl == nullptr) {
    logger.warning("rnti={}: UE must have empty HARQs during SRB0 PDU allocation", u.crnti);
    return {};
  }

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
    // Fetch the pending bytes in the SRB0 buffer + pending MAC Contention Resolution CEs bytes.
    const unsigned pending_bytes = u.pending_dl_srb0_or_srb1_newtx_bytes(true);

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
      if (unused_crbs.length() >= prbs_tbs.nof_prbs) {
        break;
      }
      ++mcs_idx;
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
  optional<unsigned> pucch_res_indicator;
  for (const auto k1_candidate : dci_1_0_k1_values) {
    // Skip k1 values that would result in a PUCCH transmission in a slot that is older than the most recent ACK slot.
    if (pdsch_alloc.slot + k1_candidate <= most_recent_ack_slot) {
      continue;
    }
    pucch_res_indicator = pucch_alloc.alloc_common_pucch_harq_ack_ue(
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

  const bool              is_srb0  = true;
  dci_dl_rnti_config_type dci_type = dci_dl_rnti_config_type::tc_rnti_f1_0;
  fill_srb_grant(u,
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
  return sched_srb_results{.h_dl = h_dl};
}

ue_fallback_scheduler::sched_srb_results ue_fallback_scheduler::schedule_srb1(ue&                      u,
                                                                              slot_point               sched_ref_slot,
                                                                              cell_resource_allocator& res_alloc,
                                                                              unsigned                 pdsch_time_res,
                                                                              unsigned                 slot_offset,
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
    optional<sch_mcs_tbs> mcs_tbs = compute_dl_mcs_tbs(pdsch_cfg, mcs_prbs_estimate.mcs, final_nof_prbs, contains_dc);

    // If there is not MCS-TBS info, it means no MCS exists such that the effective code rate is <= 0.95.
    if (not mcs_tbs.has_value()) {
      logger.warning(
          "ue={} rnti={}: Failed to allocate PDSCH. Cause: no MCS such that code rate <= 0.95.", u.ue_index, u.crnti);
      return {};
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
  optional<unsigned> pucch_res_indicator;
  for (const auto k1_candidate : dci_1_0_k1_values) {
    // Skip the k1 values that would result in a PUCCH allocation that would overlap with the most recent ACK slot.
    if (pdsch_alloc.slot + k1_candidate <= most_recent_ack_slot) {
      continue;
    }
    pucch_res_indicator = pucch_alloc.alloc_common_pucch_harq_ack_ue(
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

  unsigned nof_srb1_scheduled_bytes = fill_srb_grant(u,
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

unsigned ue_fallback_scheduler::fill_srb_grant(ue&                        u,
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
                                               bool                       is_srb0)
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
  srsran_sanity_check(is_srb0 ? dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0 : true, "Invalid DCI type for SRB0");
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
        u.build_dl_fallback_transport_block_info(
            msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes, is_srb0);
        if (not is_srb0) {
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
      // This is only for SRB1.
    case dci_dl_rnti_config_type::c_rnti_f1_0: {
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
        u.build_dl_transport_block_info(
            msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes, lcid_t::LCID_SRB1);
        if (not is_srb0) {
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
      srsran_assert(false, "Invalid DCI type for SRB1");
    }
  }

  // Save in HARQ the parameters set for this PDCCH and PDSCH PDUs.
  h_dl.save_alloc_params(pdcch.dci.type, msg.pdsch_cfg);

  return srb1_bytes_allocated;
}

unsigned ue_fallback_scheduler::has_pending_bytes_for_srb1(du_ue_index_t ue_idx) const
{
  return get_srb1_pending_tot_bytes(ue_idx) > 0U;
}

const pdsch_time_domain_resource_allocation& ue_fallback_scheduler::get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const
{
  return cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdsch_time_res_idx];
}

optional<unsigned> ue_fallback_scheduler::get_pdsch_time_res_idx(const pdsch_config_common& pdsch_cfg,
                                                                 slot_point                 sl_tx,
                                                                 const dl_harq_process*     h_dl_retx) const
{
  optional<unsigned> candidate_pdsch_time_res_idx;
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

optional<ue_fallback_scheduler::most_recent_tx_slots>
ue_fallback_scheduler::get_most_recent_slot_tx(du_ue_index_t ue_idx) const
{
  optional<ue_fallback_scheduler::most_recent_tx_slots> most_recent_tx_ack_slot;
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

void ue_fallback_scheduler::store_harq_tx(du_ue_index_t    ue_index,
                                          dl_harq_process* h_dl,
                                          bool             is_srb0,
                                          unsigned         srb_payload_bytes)
{
  srsran_sanity_check(ongoing_ues_ack_retxs.end() ==
                          std::find_if(ongoing_ues_ack_retxs.begin(),
                                       ongoing_ues_ack_retxs.end(),
                                       [ue_index, h_dl](const ack_and_retx_tracker& tracker) {
                                         return tracker.match_ue_harq(ue_index, h_dl);
                                       }),
                      "This UE and HARQ process were already in the list");

  ongoing_ues_ack_retxs.emplace_back(ue_index, h_dl, is_srb0, ues, srb_payload_bytes);
}

unsigned ue_fallback_scheduler::get_srb1_pending_tot_bytes(du_ue_index_t ue_idx) const
{
  auto it = std::find_if(
      pending_ues.begin(), pending_ues.end(), [ue_idx](const srb_ue& ue) { return ue.ue_index == ue_idx; });
  if (it == pending_ues.end()) {
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
  auto ue_it = std::find_if(
      pending_ues.begin(), pending_ues.end(), [ue_idx](const srb_ue& ue) { return ue.ue_index == ue_idx; });
  if (ue_it == pending_ues.end() or not ues.contains(ue_idx)) {
    return;
  }

  ue_it->pending_srb1_buffer_bytes > allocated_bytes ? ue_it->pending_srb1_buffer_bytes -= allocated_bytes
                                                     : ue_it->pending_srb1_buffer_bytes = 0U;
}

void ue_fallback_scheduler::update_srb1_buffer_after_rlc_bsu(du_ue_index_t ue_idx,
                                                             slot_point    sl,
                                                             unsigned      buffer_status_report)
{
  // Retrieve the UE from the list of UEs that need to be scheduled.
  auto ue_it = std::find_if(
      pending_ues.begin(), pending_ues.end(), [ue_idx](const srb_ue& ue) { return ue.ue_index == ue_idx; });
  if (ue_it == pending_ues.end() or not ues.contains(ue_idx)) {
    return;
  }

  unsigned srb1_buffer_bytes = buffer_status_report;

  // Remove the LCID-1 bytes that are already scheduled for future new transmissions, but yet to be transmitted, from
  // the RLC buffer state update received from upper layers.
  for (auto& ack_tracker : ongoing_ues_ack_retxs) {
    if (ack_tracker.ue_index != ue_idx or ack_tracker.is_srb0) {
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

  // Remove any UE that is no longer in fallback mode; this can happen in case of overallocation, or when the GNB
  // detects a false NACK from PUCCH. Existing the fallback mode is an indication that the fallback transmission was
  // completed successfully.
  for (auto ue_it = pending_ues.begin(); ue_it != pending_ues.end();) {
    if (not ues.contains(ue_it->ue_index)) {
      ue_it = pending_ues.erase(ue_it);
      continue;
    }
    // For SRB1, due to segmentation and to pre-allocation, the scheduler might not be able to estimate precisely when
    // the UE has received the SRB1 full buffer; we assume the UE has received the full SRB1 buffer data if UE exits
    // fallback mode. This is not needed for SRB0, which doesn't allow segmentation.
    // NOTE: There is a drawback to this, which is false positive detection of CSI or SRs. This can stop the scheduling
    // even before it has completed the SRB1 allocation.
    if ((not ue_it->is_srb0) and (not ues[ue_it->ue_index].get_pcell().is_in_fallback_mode())) {
      ue_it = pending_ues.erase(ue_it);
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
    // NOTE: There is a drawback to this, which is false positive detection of CSI or SRs. This can stop the scheduling
    // even before it has completed the SRB1 allocation.
    if ((not it_ue_harq->is_srb0) and (not ues[it_ue_harq->ue_index].get_pcell().is_in_fallback_mode())) {
      const unsigned tb_index = 0U;
      it_ue_harq->h_dl->cancel_harq_retxs(tb_index);
      it_ue_harq = ongoing_ues_ack_retxs.erase(it_ue_harq);
      continue;
    }

    dl_harq_process& h_dl = *it_ue_harq->h_dl;
    if (h_dl.empty()) {
      update_srb1_buffer_after_rlc_bsu(it_ue_harq->ue_index, sl, it_ue_harq->srb1_payload_bytes);
      it_ue_harq = ongoing_ues_ack_retxs.erase(it_ue_harq);
      continue;
    }
    ++it_ue_harq;
  }

  // Remove UE when the SRB1 buffer status is empty and when there are no HARQ processes scheduled for future
  // transmissions.
  for (auto ue_it = pending_ues.begin(); ue_it != pending_ues.end();) {
    auto& ue = *ue_it;
    if (ue.is_srb0) {
      ++ue_it;
      continue;
    }

    const bool remove_ue = ue.pending_srb1_buffer_bytes == 0 and
                           ongoing_ues_ack_retxs.end() ==
                               std::find_if(ongoing_ues_ack_retxs.begin(),
                                            ongoing_ues_ack_retxs.end(),
                                            [ue_idx = ue.ue_index, sl](const ack_and_retx_tracker& tracker) {
                                              return tracker.ue_index == ue_idx and tracker.h_dl->is_waiting_ack() and
                                                     tracker.h_dl->tb(0).nof_retxs == 0 and
                                                     tracker.h_dl->slot_tx() >= sl;
                                            });

    remove_ue ? ue_it = pending_ues.erase(ue_it) : ++ue_it;
  }
}
