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

#include "ue_fallback_scheduler.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "../pucch_scheduling/pucch_allocator.h"
#include "../support/csi_rs_helpers.h"
#include "../support/dci_builder.h"
#include "../support/dmrs_helpers.h"
#include "../support/mcs_calculator.h"
#include "../support/pdsch/pdsch_resource_allocation.h"
#include "../support/prbs_calculator.h"
#include "../support/pusch/pusch_td_resource_indices.h"
#include "../uci_scheduling/uci_allocator.h"
#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/ran/transform_precoding/transform_precoding_helpers.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

ue_fallback_scheduler::ue_fallback_scheduler(const scheduler_ue_expert_config& expert_cfg_,
                                             const cell_configuration&         cell_cfg_,
                                             pdcch_resource_allocator&         pdcch_sch_,
                                             pucch_allocator&                  pucch_alloc_,
                                             uci_allocator&                    uci_alloc_,
                                             ue_repository&                    ues_) :
  expert_cfg(expert_cfg_),
  cell_cfg(cell_cfg_),
  pdcch_sch(pdcch_sch_),
  pucch_alloc(pucch_alloc_),
  uci_alloc(uci_alloc_),
  ues(ues_),
  initial_active_dl_bwp(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params),
  ss_cfg(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common
             .search_spaces[cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id]),
  cs_cfg(cell_cfg.get_common_coreset(ss_cfg.get_coreset_id())),
  pucch_crbs(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length()),
  logger(srslog::fetch_basic_logger("SCHED"))
{
  // Pre-reserve memory to avoid allocations in RT.
  pending_dl_ues_new_tx.reserve(MAX_NOF_DU_UES);
  ongoing_ues_ack_retxs.reserve(MAX_NOF_DU_UES);
  pending_ul_ues.reserve(MAX_NOF_DU_UES);

  // NOTE 1: We use a std::vector instead of a std::array because we can later on initialize the vector with the minimum
  // value of k1, passed through the expert config.
  // NOTE 2: Although the TS 38.213, Section 9.2.3 specifies that the k1 possible values are {1, ..., 8}, some UE
  // implementations that we work with do not handle well the value k1=8. Instead, in the particular context of RRC
  // Reestablishment, they PRACH instead of sending the PUCCH.
  const unsigned max_k1 = 7U;
  srsran_sanity_check(expert_cfg.min_k1 <= max_k1, "Invalid min_k1 value");
  for (unsigned k1_value = expert_cfg.min_k1; k1_value <= max_k1; ++k1_value) {
    dci_1_0_k1_values.push_back(k1_value);
  }
  slots_with_no_pdxch_space.fill(false);

  // Compute the PRBs that might be used for PUCCH transmissions, to avoid scheduling PUSCH over them.
  for (const auto& pucch_res : cell_cfg.pucch_guardbands) {
    // TODO: Convert PRBs to CRBs once multiple BWPs are supported.
    pucch_crbs.fill(pucch_res.prbs.start(), pucch_res.prbs.stop());
  }
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

  // Schedule DL new txs with the following priority: ConRes CE, SRB0 and SRB1.
  if (not schedule_dl_new_tx(res_alloc, dl_new_tx_alloc_type::conres_only)) {
    return;
  }
  if (not schedule_dl_new_tx(res_alloc, dl_new_tx_alloc_type::srb0)) {
    return;
  }
  schedule_dl_new_tx(res_alloc, dl_new_tx_alloc_type::srb1);
}

void ue_fallback_scheduler::stop()
{
  ongoing_ues_ack_retxs.clear();
  slots_with_no_pdxch_space = {};
  pending_dl_ues_new_tx.clear();
  pending_ul_ues.clear();
}

void ue_fallback_scheduler::handle_dl_buffer_state_indication(du_ue_index_t ue_index)
{
  if (not ues.contains(ue_index)) {
    logger.error("ue={}: DL Buffer Occupancy update discarded. UE is not found in the scheduler",
                 fmt::underlying(ue_index));
    return;
  }
  ue& u = ues[ue_index];
  if (not u.get_pcell().is_in_fallback_mode()) {
    logger.error("ue={}: DL Buffer Occupancy update discarded. UE is not in fallback mode", fmt::underlying(ue_index));
    return;
  }

  auto ue_it = std::find_if(pending_dl_ues_new_tx.begin(),
                            pending_dl_ues_new_tx.end(),
                            [ue_index](const fallback_ue& ue) { return ue.ue_index == ue_index; });

  if (ue_it != pending_dl_ues_new_tx.end()) {
    return;
  }

  // The UE doesn't exist in the internal fallback scheduler list, add it.
  pending_dl_ues_new_tx.push_back({ue_index});
}

void ue_fallback_scheduler::handle_conres_indication(du_ue_index_t ue_index)
{
  if (not ues.contains(ue_index)) {
    logger.error("ue={}: ConRes CE discarded. Cause: UE is not found in the scheduler", fmt::underlying(ue_index));
    return;
  }
  auto& u = ues[ue_index];
  if (not u.get_pcell().is_in_fallback_mode()) {
    // Note: In Test mode, the UE can skip fallback mode. However, since it was created via UL-CCCH, there is still an
    // attempt to schedule the ConRes CE. If we want to change this log to warning, we need to create test mode UEs
    // in a different manner.
    logger.info("ue={}: ConRes CE discarded. Cause: UE is not in fallback state", fmt::underlying(ue_index));
    return;
  }

  // Mark the start of the conRes timer.
  u.drx_controller().on_con_res_start();

  auto ue_it = std::find_if(pending_dl_ues_new_tx.begin(),
                            pending_dl_ues_new_tx.end(),
                            [ue_index](const fallback_ue& ue) { return ue.ue_index == ue_index; });

  // Entry for this UE already exists.
  if (ue_it != pending_dl_ues_new_tx.end()) {
    return;
  }

  pending_dl_ues_new_tx.push_back({ue_index});
}

void ue_fallback_scheduler::handle_ul_bsr_indication(du_ue_index_t ue_index, const ul_bsr_indication_message& bsr_ind)
{
  if (not ues.contains(ue_index) or not ues[ue_index].get_pcell().is_in_fallback_mode()) {
    logger.error("ue_index={} not found in the scheduler or not in fallback", fmt::underlying(ue_index));
    return;
  }

  const auto* srb_lcg_report =
      std::find_if(bsr_ind.reported_lcgs.begin(), bsr_ind.reported_lcgs.end(), [](const auto& lcg_report) {
        // We assume the LCG ID for SRB is 0.
        const lcg_id_t srb_lcg_id = uint_to_lcg_id(0U);
        return lcg_report.lcg_id == srb_lcg_id;
      });
  const unsigned bsr_bytes = srb_lcg_report != bsr_ind.reported_lcgs.end() ? srb_lcg_report->nof_bytes : 0U;
  if (bsr_bytes == 0) {
    return;
  }

  auto ue_it = std::find(pending_ul_ues.begin(), pending_ul_ues.end(), ue_index);

  // For UL UEs, we don't keep an internal estimate of the UL traffic, but we rely on the UL logical channel manager for
  // that task. Thus, compared to the DL case, the only operation that is needed for the UL is the addition of a new UE
  // to the pending UL UEs list.
  if (ue_it == pending_ul_ues.end()) {
    pending_ul_ues.emplace_back(ue_index);
  }
}

void ue_fallback_scheduler::handle_sr_indication(du_ue_index_t ue_index)
{
  if (not ues.contains(ue_index) or not ues[ue_index].get_pcell().is_in_fallback_mode()) {
    logger.error("ue_index={} not found in the scheduler or not in fallback", fmt::underlying(ue_index));
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
    auto&                   u    = ues[next_ue_harq_retx.ue_index];
    dl_harq_process_handle& h_dl = next_ue_harq_retx.h_dl;

    if (h_dl.has_pending_retx()) {
      dl_sched_outcome outcome = schedule_dl_srb(res_alloc, u, h_dl);
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
  // Processes all the UL UEs at once, including UEs with new transmissions and UEs with retransmissions.
  for (auto next_ue = pending_ul_ues.begin(); next_ue != pending_ul_ues.end();) {
    auto&                u       = ues[*next_ue];
    ul_srb_sched_outcome outcome = schedule_ul_ue(res_alloc, u);
    if (outcome == ul_srb_sched_outcome::stop_ul_scheduling) {
      // If there is no PDCCH space, then stop the scheduling for all UL UEs.
      return;
    }
    ++next_ue;
  }
}

ue_fallback_scheduler::dl_new_tx_alloc_type ue_fallback_scheduler::get_dl_new_tx_alloc_type(const ue& u)
{
  if (u.has_pending_dl_newtx_bytes(LCID_SRB0)) {
    return dl_new_tx_alloc_type::srb0;
  }
  if (u.has_pending_dl_newtx_bytes(LCID_SRB1)) {
    return dl_new_tx_alloc_type::srb1;
  }
  return u.is_conres_ce_pending() ? dl_new_tx_alloc_type::conres_only : dl_new_tx_alloc_type::error;
}

bool ue_fallback_scheduler::schedule_dl_new_tx(cell_resource_allocator& res_alloc,
                                               dl_new_tx_alloc_type     selected_alloc_type)
{
  for (auto next_ue = pending_dl_ues_new_tx.begin(); next_ue != pending_dl_ues_new_tx.end();) {
    // Determine if we should schedule ConRes, SRB0, SRB1 for the given UE.
    auto&      u          = ues[next_ue->ue_index];
    const auto alloc_type = get_dl_new_tx_alloc_type(u);
    if (alloc_type == dl_new_tx_alloc_type::error) {
      // The UE is not in a state for scheduling
      logger.error("ue={}: UE is an inconsistent state in the fallback scheduler. Pending bytes={}",
                   fmt::underlying(next_ue->ue_index),
                   u.pending_dl_newtx_bytes());
      next_ue = pending_dl_ues_new_tx.erase(next_ue);
      continue;
    }
    if (alloc_type != selected_alloc_type) {
      // This type of alloc is not being prioritized.
      ++next_ue;
      continue;
    }

    const bool srb0_or_srb1_only = alloc_type != dl_new_tx_alloc_type::conres_only and not u.is_conres_ce_pending();
    if (srb0_or_srb1_only and not u.get_pcell().is_conres_complete()) {
      // If the UE hasn't acked the ConRes, we cannot schedule the SRB0 or SRB1, as any MAC PDU received without ConRes
      // MAC CE would make the Contention Resolution fail, as per TS 38.331, Section 5.1.5.
      ++next_ue;
      continue;
    }

    // Make the scheduling attempt.
    dl_sched_outcome outcome = schedule_dl_srb(res_alloc, u, std::nullopt);
    if (outcome == dl_sched_outcome::next_ue) {
      // Allocation failed but we can attempt with another UE.
      ++next_ue;
      continue;
    }
    if (outcome == dl_sched_outcome::stop_dl_scheduling) {
      // This is the case the DL fallback scheduler has reached the maximum number of scheduling attempts and the fnc
      // returns \ref stop_dl_scheduling.
      return false;
    }

    // There was a successful allocation. This does not, however, mean that the pending data was completely flushed.
    // Move to the next UE ONLY IF the UE has no more pending bytes. This is to give priority to the same UE, if
    // there are still some bytes left in the buffer. At the next iteration, the scheduler will try
    // again with the same scheduler, but starting from the next available slot.
    if (not u.has_pending_dl_newtx_bytes()) {
      next_ue = pending_dl_ues_new_tx.erase(next_ue);
    }
  }

  return true;
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
ue_fallback_scheduler::schedule_dl_srb(cell_resource_allocator&              res_alloc,
                                       ue&                                   u,
                                       std::optional<dl_harq_process_handle> h_dl_retx)
{
  const auto& bwp_cfg_common = cell_cfg.dl_cfg_common.init_dl_bwp;
  // Search valid PDSCH time domain resource.

  const bool is_retx = h_dl_retx.has_value();

  // \ref sched_ref_slot is the slot that we take as reference for the scheduler, which is processed when calling the
  // slot_indication().
  // NOTE: we guarantee that \ref sched_ref_slot is a DL slot in the caller.
  // TODO: Make this compatible with k0 > 0.
  const slot_point sched_ref_slot = res_alloc[0].slot;

  // Retrieve the slot of the last PDSCH for this UE.
  const slot_point last_pdsch_slot = u.get_pcell().harqs.last_pdsch_slot();
  const slot_point last_slot_ack   = u.get_pcell().harqs.last_ack_slot();

  // \ref starting_slot is the slot from which the scheduler will search for PDSCH space for a given UE.
  // As per TS 38.214, clause 5.1, the PDSCHs need to follow the same order as PDCCHs for a given UE. Thus, we set the
  // starting_slot to be always higher than the slot of the last PDSCH for the same UE (if it exists).
  slot_point starting_slot =
      last_pdsch_slot.valid() ? std::max(get_next_srb_slot(cell_cfg, last_pdsch_slot), sched_ref_slot) : sched_ref_slot;

  for (slot_point next_slot = starting_slot; next_slot <= sched_ref_slot + max_dl_slots_ahead_sched;
       next_slot            = get_next_srb_slot(cell_cfg, next_slot)) {
    if (sched_attempts_cnt >= max_dl_sched_attempts) {
      return dl_sched_outcome::stop_dl_scheduling;
    }

    if (slots_with_no_pdxch_space[next_slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE]) {
      continue;
    }

    auto                                offset_to_sched_ref_slot = static_cast<unsigned>(next_slot - sched_ref_slot);
    const cell_slot_resource_allocator& pdcch_alloc              = res_alloc[offset_to_sched_ref_slot];
    const cell_slot_resource_allocator& pdsch_alloc              = res_alloc[offset_to_sched_ref_slot];

    if ((not cell_cfg.is_dl_enabled(pdcch_alloc.slot)) or (not cell_cfg.is_dl_enabled(pdsch_alloc.slot))) {
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
      logger.debug("rnti={}: Failed to allocate fallback PDSCH. Cause: No space available in scheduler output list",
                   u.crnti);
      slots_with_no_pdxch_space[next_slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = true;
      continue;
    }

    // As per TS 38.214, clause 5.1, it is not possible to schedule a PDSCH whose related PUCCH falls in a slot that
    // is the same as or older than the most recent already scheduled ACK slot (for the same UE). Whenever we detect
    // this is the case we skip the allocation in advance.
    slot_point most_recent_ack_slot = pdsch_alloc.slot;
    if (last_slot_ack.valid()) {
      if (pdsch_alloc.slot + dci_1_0_k1_values.back() <= last_slot_ack) {
        continue;
      }
      most_recent_ack_slot = last_slot_ack;
    }

    auto h_dl =
        alloc_grant(u, res_alloc, time_res_idx.value(), offset_to_sched_ref_slot, most_recent_ack_slot, h_dl_retx);

    const bool alloc_successful = h_dl.has_value();
    if (alloc_successful) {
      if (not is_retx) {
        store_harq_tx(u.ue_index, h_dl.value());
      }
      return dl_sched_outcome::success;
    }

    ++sched_attempts_cnt;
  }

  // No resource found in UE's carriers and Search spaces.
  logger.debug(
      "rnti={}: Skipped fallback PDSCH allocation in slots:[{},{}). Cause: no PDCCH/PDSCH/PUCCH resources available",
      u.crnti,
      starting_slot,
      sched_ref_slot + max_dl_slots_ahead_sched + 1);
  return dl_sched_outcome::next_ue;
}

// Helper to determine DCI type to use in PDSCH.
static dci_dl_rnti_config_type get_dci_type(const ue& u, const std::optional<dl_harq_process_handle>& h_dl)
{
  if (h_dl.has_value()) {
    return h_dl->get_grant_params().dci_cfg_type;
  }
  if (u.is_conres_ce_pending()) {
    return dci_dl_rnti_config_type::tc_rnti_f1_0;
  }
  return dci_dl_rnti_config_type::c_rnti_f1_0;
}

// Helper to allocate common (and optionally dedicated) PUCCH.
static std::optional<uci_allocation> allocate_ue_fallback_pucch(ue&                         u,
                                                                cell_resource_allocator&    res_alloc,
                                                                pucch_allocator&            pucch_alloc,
                                                                uci_allocator&              uci_alloc,
                                                                const pdcch_dl_information& pdcch_info,
                                                                span<const uint8_t>         k1_values,
                                                                slot_point                  pdsch_slot,
                                                                slot_point                  min_ack_slot,
                                                                bool                        common_alloc,
                                                                bool                        ded_alloc)
{
  srsran_assert(ded_alloc or common_alloc, "Invalid params passed to this function");
  const unsigned pdsch_delay = pdsch_slot - res_alloc.slot_tx();

  if (not common_alloc and ded_alloc) {
    // UE dedicated-only PUCCH allocation.
    std::optional<uci_allocation> uci =
        uci_alloc.alloc_uci_harq_ue(res_alloc, u.crnti, u.get_pcell().cfg(), pdsch_delay, k1_values);
    return uci;
  }

  std::optional<uint8_t> last_valid_k1;
  for (uint8_t k1_candidate : k1_values) {
    if (pdsch_slot + k1_candidate <= min_ack_slot) {
      // Skip k1 values that would result in a PUCCH transmission in a slot that is older than the most recent ACK slot.
      continue;
    }
    slot_point uci_slot = pdsch_slot + k1_candidate;
    if (not res_alloc.cfg.is_fully_ul_enabled(uci_slot)) {
      // If it is not UL-enabled slot.
      continue;
    }
    // There can be PUSCHs that are already allocated: to avoid potential collisions between PUCCH and PUSCH, skip this
    // allocation if any allocated PUSCHs.
    if (not res_alloc[k1_candidate].result.ul.puschs.empty()) {
      continue;
    }
    last_valid_k1 = k1_candidate;

    std::optional<unsigned> pucch_res_indicator;
    if (ded_alloc) {
      pucch_res_indicator = pucch_alloc.alloc_common_and_ded_harq_res(
          res_alloc, u.crnti, u.get_pcell().cfg(), pdsch_delay, k1_candidate, pdcch_info);
    } else {
      pucch_res_indicator =
          pucch_alloc.alloc_common_pucch_harq_ack_ue(res_alloc, u.crnti, pdsch_delay, k1_candidate, pdcch_info);
    }
    if (pucch_res_indicator.has_value()) {
      return uci_allocation{k1_candidate, 0, pucch_res_indicator};
    }
  }
  if (last_valid_k1.has_value()) {
    return uci_allocation{last_valid_k1.value(), 0, std::nullopt};
  }
  return std::nullopt;
}

std::optional<dl_harq_process_handle>
ue_fallback_scheduler::alloc_grant(ue&                                   u,
                                   cell_resource_allocator&              res_alloc,
                                   unsigned                              pdsch_time_res,
                                   unsigned                              slot_offset,
                                   slot_point                            most_recent_ack_slot,
                                   std::optional<dl_harq_process_handle> h_dl_retx)
{
  ue_cell&                                     ue_pcell     = u.get_pcell();
  const subcarrier_spacing                     scs          = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(pdsch_time_res);
  const bool                                   is_retx      = h_dl_retx.has_value();

  // Search for empty HARQ in case of newTx.
  if (not is_retx and not ue_pcell.harqs.has_empty_dl_harqs()) {
    logger.warning("rnti={}: UE must have empty HARQs during fallback newtx allocation", u.crnti);
    return {};
  }

  const dci_dl_rnti_config_type dci_type  = get_dci_type(u, h_dl_retx);
  const pdsch_config_params     pdsch_cfg = dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0
                                                ? sched_helper::get_pdsch_config_f1_0_tc_rnti(cell_cfg, pdsch_td_cfg)
                                                : sched_helper::get_pdsch_config_f1_0_c_rnti(cell_cfg, pdsch_td_cfg);

  // For DCI 1-0 scrambled with TC-RNTI, as per TS 38.213, Section 7.3.1.2.1, we should consider the size of CORESET#0
  // as the size for the BWP.
  // For DCI 1-0 scrambled with C-RNTI, if the DCI is monitored in a common search space and CORESET#0 is configured
  // for the cell, as per TS 38.213, Section 7.3.1.0, we should consider the size of CORESET#0 as the size for the
  // BWP.
  cell_slot_resource_allocator& pdsch_alloc = res_alloc[slot_offset + pdsch_td_cfg.k0];
  auto cset0_crbs_lim = pdsch_helper::get_ra_crb_limits_common(cell_cfg.dl_cfg_common.init_dl_bwp, ss_cfg.get_id());
  crb_bitmap used_crbs =
      pdsch_alloc.dl_res_grid.used_crbs(initial_active_dl_bwp.scs, cset0_crbs_lim, pdsch_cfg.symbols);

  crb_interval unused_crbs = rb_helper::find_next_empty_interval(used_crbs, cset0_crbs_lim);
  if (unused_crbs.empty()) {
    logger.debug("rnti={}: Postponed PDU scheduling for slot={}. Cause: No space in PDSCH.", u.crnti, pdsch_alloc.slot);
    // If there is no free PRBs left on this slot for this UE, then this slot should be avoided by the other UEs too.
    slots_with_no_pdxch_space[pdsch_alloc.slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = true;
    return {};
  }

  crb_interval  ue_grant_crbs;
  sch_prbs_tbs  prbs_tbs{};
  sch_mcs_index mcs_idx = 0;
  if (is_retx) {
    // Use the same MCS, nof PRBs and TBS as the last allocation.
    mcs_idx            = h_dl_retx->get_grant_params().mcs;
    prbs_tbs.nof_prbs  = h_dl_retx->get_grant_params().rbs.type1().length();
    prbs_tbs.tbs_bytes = h_dl_retx->get_grant_params().tbs_bytes;

    if (unused_crbs.length() < prbs_tbs.nof_prbs) {
      // In case of HARQ retxs, the number of RBs cannot change.
      logger.debug("rnti={}: Fallback retx postponed. Cause: Not enough RBs for PDSCH", u.crnti);
      return {};
    }
    ue_grant_crbs = {unused_crbs.start(), unused_crbs.start() + prbs_tbs.nof_prbs};

  } else {
    const unsigned only_conres_bytes = u.pending_conres_ce_bytes();
    const unsigned only_srb0_bytes   = u.pending_dl_newtx_bytes(LCID_SRB0);
    const unsigned only_srb1_bytes   = u.pending_dl_newtx_bytes(LCID_SRB1);
    const unsigned pending_bytes     = only_conres_bytes + only_srb0_bytes + only_srb1_bytes;
    srsran_assert(pending_bytes > 0, "Unexpected number of pending bytes");
    // There must be space for ConRes CE, if it is pending. If only SRB0 is pending (no ConRes), there must be space
    // for it, as the SRB0 cannot be segmented.
    const unsigned min_pending_bytes =
        only_conres_bytes > 0 ? only_conres_bytes : (only_srb0_bytes > 0 ? only_srb0_bytes : 0);

    std::optional<sch_mcs_index> fixed_mcs;
    if (only_srb1_bytes > 0) {
      fixed_mcs = map_cqi_to_mcs(expert_cfg.initial_cqi, pdsch_cfg.mcs_table);
      srsran_assert(fixed_mcs.has_value(), "Invalid Initial CQI {}", expert_cfg.initial_cqi);
    }
    std::tuple<unsigned, sch_mcs_index, units::bytes> result =
        select_tbs(pdsch_cfg, pending_bytes, unused_crbs, fixed_mcs);
    unsigned chosen_tbs = std::get<2>(result).value();
    if (chosen_tbs == 0) {
      logger.info("rnti={}: Fallback newTx allocation postponed. Cause: Unable to compute valid TBS for available RBs",
                  u.crnti);
      return {};
    }
    if (chosen_tbs < min_pending_bytes) {
      // We could not even fulfill the minimum pending bytes.
      logger.debug("rnti={}: Fallback newTx allocation postponed. Cause: Pending non-segmentable bytes ({}) exceed TBS "
                   "calculated ({})",
                   u.crnti,
                   min_pending_bytes,
                   chosen_tbs);
      if (only_conres_bytes > 0) {
        // In case not even a ConRes CE can fit, we can start ignoring this slot.
        slots_with_no_pdxch_space[pdsch_alloc.slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = true;
      }
      return {};
    }
    if (chosen_tbs < pending_bytes) {
      // We will need to break down the pending bytes in multiple grants.
      if (only_conres_bytes > 0 and only_srb0_bytes > 0) {
        // In case of both ConResCE and SRB0 are pending, we recompute the MCS, this time, just for scheduling ConRes
        // CE.
        result     = select_tbs(pdsch_cfg, min_pending_bytes, unused_crbs, std::nullopt);
        chosen_tbs = std::get<2>(result).value();
        if (chosen_tbs == 0) {
          logger.info(
              "rnti={}: Fallback newTx allocation postponed. Cause: Unable to compute valid TBS for available RBs",
              u.crnti);
          return {};
        }
      }
    }

    // Selection of Nof RBs, TBS and MCS complete.
    prbs_tbs      = {std::get<0>(result), std::get<2>(result).value()};
    mcs_idx       = std::get<1>(result);
    ue_grant_crbs = {unused_crbs.start(), unused_crbs.start() + prbs_tbs.nof_prbs};
  }

  // Allocate PDCCH resources.
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[slot_offset];
  pdcch_dl_information*         pdcch =
      pdcch_sch.alloc_dl_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.get_id(), aggregation_level::n4);
  if (pdcch == nullptr) {
    logger.debug("rnti={}: Postponed PDU scheduling for slot={}. Cause: No space in PDCCH.", u.crnti, pdcch_alloc.slot);
    // If there is no PDCCH space on this slot for this UE, then this slot should be avoided by the other UEs too.
    slots_with_no_pdxch_space[pdcch_alloc.slot.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = true;
    return {};
  }

  // Allocate PUCCH resources.
  // Note: In case of RRC Reconfiguration that is not after the RRC Reestablishment, common PUCCH is not required, as
  // the UE already has a dedicated config.
  // Note: In case the UE has no full config (RRC Reject), dedicated PUCCH is not required.
  // Note: If the actual UE has received the RRCSetup (with config) but the gNB doesn't receive an ACK=1, the UE can use
  // the PUCCH dedicated resource to ACK the RRCSetup Retx (as per TS 38.213, section 9.2.1, "if a ue has dedicated
  // PUCCH resource configuration, the UE is provided by higher layers with one or more PUCCH resources [...]")
  // Note: The confirmation of UE fallback exit coming from higher layers may be late. In such case, we err on the side
  // of caution and allocate a dedicated PUCCH as well. We do not need to do this for the CON RES CE or SRB0
  // allocations.
  // Note: \c u.is_reestablished() is only set at the start of the RRC Reconfiguration procedure following a
  // re-establishment.
  const bool use_common    = not u.is_reconfig_ongoing() or u.is_reestablished();
  bool       use_dedicated = u.is_reconfig_ongoing();
  if (u.ue_cfg_dedicated()->is_ue_cfg_complete()) {
    // Note: this check is meant for the case of the GNB missing the ACK for RRCSetup and then retransmitting it. In
    // this case, we need to schedule also on dedicated because the UE already has a dedicated configuration, even
    // though the gNB doesn't know that.
    // As a side effect, this will make the GNB also schedule on dedicated resources for the case of missing the ACK for
    // RRCReestablishment and then retransmitting it. This is not optimal, but not critical.
    use_dedicated |= is_retx;
    // Make sure the possible PDSCH grants that are scheduled after RRCSetupComplete use dedicated resources, while the
    // notification to remove the UE from the fallback scheduler hasn't arrived yet.
    use_dedicated |= (dci_type == dci_dl_rnti_config_type::c_rnti_f1_0 and u.has_pending_dl_newtx_bytes(LCID_SRB1));
  }
  std::optional<uci_allocation> uci = allocate_ue_fallback_pucch(u,
                                                                 res_alloc,
                                                                 pucch_alloc,
                                                                 uci_alloc,
                                                                 *pdcch,
                                                                 dci_1_0_k1_values,
                                                                 pdsch_alloc.slot,
                                                                 most_recent_ack_slot,
                                                                 use_common,
                                                                 use_dedicated);
  if (not uci.has_value() or not uci.value().pucch_res_indicator.has_value()) {
    if (uci.has_value() and not uci.value().pucch_res_indicator.has_value()) {
      // Note: Only log if there was at least one valid k1 candidate for this PDSCH slot.
      logger.debug("rnti={}: Failed to allocate fallback PDSCH for slot={}. Cause: No space in PUCCH",
                   u.crnti,
                   pdsch_alloc.slot);
    }
    pdcch_sch.cancel_last_pdcch(pdcch_alloc);
    return {};
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, ue_grant_crbs});

  // Update DRX controller state.
  u.drx_controller().on_new_dl_pdcch_alloc(pdcch_alloc.slot);

  // Fill ConRes and/or SRB grant.
  return fill_dl_srb_grant(u,
                           pdsch_alloc.slot,
                           h_dl_retx,
                           *pdcch,
                           dci_type,
                           pdsch_alloc.result.dl.ue_grants.emplace_back(),
                           uci.value(),
                           pdsch_time_res,
                           mcs_idx,
                           ue_grant_crbs,
                           pdsch_cfg,
                           prbs_tbs.tbs_bytes,
                           is_retx);
}

std::tuple<unsigned, sch_mcs_index, units::bytes>
ue_fallback_scheduler::select_tbs(const pdsch_config_params&          pdsch_cfg,
                                  unsigned                            pending_bytes,
                                  const crb_interval&                 unused_crbs,
                                  const std::optional<sch_mcs_index>& fixed_mcs) const
{
  sch_mcs_index  chosen_mcs_idx = 0;
  sch_prbs_tbs   prbs_tbs{};
  const unsigned nof_dmrs_per_rb = calculate_nof_dmrs_per_rb(pdsch_cfg.dmrs);

  if (fixed_mcs.has_value()) {
    // For the case where the MCS is already selected.

    chosen_mcs_idx                       = *fixed_mcs;
    const sch_mcs_description mcs_config = pdsch_mcs_get_config(pdsch_cfg.mcs_table, *fixed_mcs);
    prbs_tbs                             = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                       pdsch_cfg.symbols.length(),
                                                       nof_dmrs_per_rb,
                                                       pdsch_cfg.nof_oh_prb,
                                                       mcs_config,
                                                       pdsch_cfg.nof_layers},
                            unused_crbs.length());

    // [Implementation-defined] In case of partial slots and nof. PRBs allocated equals to 1 probability of KO is
    // high due to code not being able to cope with interference. So the solution is to increase the PRB allocation
    // to greater than 1 PRB.
    const bool set_min_nof_prbs =
        prbs_tbs.nof_prbs == 1 and pdsch_cfg.symbols.length() < NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
    if (set_min_nof_prbs) {
      if (unused_crbs.length() <= 1) {
        return std::make_tuple(0, *fixed_mcs, units::bytes{0});
      }
      prbs_tbs.nof_prbs = 2;
      tbs_calculator_configuration tbs_cfg{pdsch_cfg.symbols.length(),
                                           nof_dmrs_per_rb,
                                           pdsch_cfg.nof_oh_prb,
                                           pdsch_mcs_get_config(pdsch_cfg.mcs_table, chosen_mcs_idx),
                                           pdsch_cfg.nof_layers,
                                           pdsch_cfg.tb_scaling_field,
                                           prbs_tbs.nof_prbs};
      prbs_tbs.tbs_bytes = units::bits{tbs_calculator_calculate(tbs_cfg)}.round_up_to_bytes().value();
    }

    // As \c txDirectCurrentLocation, in \c SCS-SpecificCarrier, TS 38.331, "If this field (\c
    // txDirectCurrentLocation) is absent for downlink within ServingCellConfigCommon and ServingCellConfigCommonSIB,
    // the UE assumes the default value of 3300 (i.e. "Outside the carrier")".
    crb_interval grant_crbs{unused_crbs.start(), unused_crbs.start() + prbs_tbs.nof_prbs};
    bool         contains_dc = false;
    if (cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list.back().tx_direct_current_location.has_value()) {
      contains_dc = dc_offset_helper::is_contained(
          cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list.back().tx_direct_current_location.value(), grant_crbs);
    }
    std::optional<sch_mcs_tbs> mcs_tbs = compute_dl_mcs_tbs(pdsch_cfg, chosen_mcs_idx, prbs_tbs.nof_prbs, contains_dc);
    if (mcs_tbs.has_value()) {
      chosen_mcs_idx     = mcs_tbs.value().mcs;
      prbs_tbs.tbs_bytes = mcs_tbs.value().tbs;
    } else {
      return std::make_tuple(0, *fixed_mcs, units::bytes{0});
    }
  } else {
    // Dynamic MCS (without CQI)

    // Try to find lowest MCS to fit pending bytes.
    sch_mcs_index next_mcs_idx = 0;
    do {
      // As per TS 38.214, clause 5.1.3.1, if the PDSCH is scheduled by a PDCCH with CRC scrambled by TC-RNTI, the UE
      // use "Table 5.1.3.1-1: MCS index table 1" for MCS mapping. This is not stated explicitly, but can be inferred
      // from the sentence "... the UE shall use I_MCS and Table 5.1.3.1-1 to determine the modulation order (Qm) and
      // Target code rate (R) used in the physical downlink shared channel.".

      // At this point, xOverhead is not configured yet. As per TS 38.214, Clause 5.1.3.2, xOverhead is assumed to be
      // 0.
      const sch_mcs_description mcs_config = pdsch_mcs_get_config(pdsch_mcs_table::qam64, next_mcs_idx);
      prbs_tbs                             = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                         pdsch_cfg.symbols.length(),
                                                         nof_dmrs_per_rb,
                                                         pdsch_cfg.nof_oh_prb,
                                                         mcs_config,
                                                         pdsch_cfg.nof_layers},
                              unused_crbs.length());
      chosen_mcs_idx                       = next_mcs_idx;
      ++next_mcs_idx;
    } while (next_mcs_idx <= expert_cfg.max_msg4_mcs and prbs_tbs.tbs_bytes < pending_bytes);
  }

  if (prbs_tbs.nof_prbs == 0) {
    // No found solution found.
    return std::make_tuple(0, *fixed_mcs, units::bytes{0});
  }
  return std::make_tuple(prbs_tbs.nof_prbs, chosen_mcs_idx, units::bytes{prbs_tbs.tbs_bytes});
}

dl_harq_process_handle ue_fallback_scheduler::fill_dl_srb_grant(ue&                                   u,
                                                                slot_point                            pdsch_slot,
                                                                std::optional<dl_harq_process_handle> h_dl,
                                                                pdcch_dl_information&                 pdcch,
                                                                dci_dl_rnti_config_type               dci_type,
                                                                dl_msg_alloc&                         msg,
                                                                const uci_allocation&                 uci,
                                                                unsigned                              pdsch_time_res,
                                                                sch_mcs_index                         mcs_idx,
                                                                const crb_interval&                   ue_grant_crbs,
                                                                const pdsch_config_params&            pdsch_params,
                                                                unsigned                              tbs_bytes,
                                                                bool                                  is_retx)
{
  // Allocate DL HARQ.
  // NOTE: We do not multiplex the SRB1 PUCCH with existing PUCCH HARQs, thus both DAI and HARQ-ACK bit index are 0.
  if (not is_retx) {
    h_dl = u.get_pcell().harqs.alloc_dl_harq(
        pdsch_slot, uci.k1 + cell_cfg.ntn_cs_koffset, expert_cfg.max_nof_dl_harq_retxs, uci.harq_bit_idx);
  } else {
    bool result = h_dl->new_retx(pdsch_slot, uci.k1 + cell_cfg.ntn_cs_koffset, uci.harq_bit_idx);
    srsran_sanity_check(result, "Unable to allocate HARQ retx");
  }

  // Fill DL PDCCH DCI.
  static const uint8_t msg4_rv = 0;

  vrb_interval vrbs;
  switch (dci_type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0: {
      const crb_interval cs0_crbs = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs();
      vrbs                        = crb_to_vrb_f1_0_common_ss_non_interleaved(ue_grant_crbs, cs0_crbs.start());
    } break;
    case dci_dl_rnti_config_type::c_rnti_f1_0: {
      const search_space_info&   ss_info           = u.get_pcell().cfg().search_space(pdcch.ctx.context.ss_id);
      const bwp_downlink_common& active_dl_bwp_cmn = *ss_info.bwp->dl_common.value();
      const bwp_configuration&   active_dl_bwp     = active_dl_bwp_cmn.generic_params;
      vrbs                                         = rb_helper::crb_to_vrb_dl_non_interleaved(ue_grant_crbs,
                                                      active_dl_bwp.crbs.start(),
                                                      cs_cfg.get_coreset_start_crb(),
                                                      dci_dl_format::f1_0,
                                                      ss_info.cfg->is_common_search_space());
    } break;
    default:
      srsran_assert(false, "Invalid DCI type for SRB1");
  }

  switch (dci_type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0: {
      build_dci_f1_0_tc_rnti(pdcch.dci,
                             cell_cfg.dl_cfg_common.init_dl_bwp,
                             vrbs,
                             pdsch_time_res,
                             uci.k1,
                             uci.pucch_res_indicator.value(),
                             mcs_idx,
                             msg4_rv,
                             *h_dl);
      break;
    }
    case dci_dl_rnti_config_type::c_rnti_f1_0: {
      const unsigned DAI_MOD = 4U;
      build_dci_f1_0_c_rnti(pdcch.dci,
                            u.get_pcell().cfg().search_space(pdcch.ctx.context.ss_id),
                            cell_cfg.dl_cfg_common.init_dl_bwp,
                            vrbs,
                            pdsch_time_res,
                            uci.k1,
                            uci.pucch_res_indicator.value(),
                            uci.harq_bit_idx % DAI_MOD,
                            mcs_idx,
                            msg4_rv,
                            *h_dl);
      break;
    }
    default: {
      srsran_assert(false, "Invalid DCI type for SRB1");
    }
  }

  // Fill PDSCH PDU.
  msg.context.ue_index    = u.ue_index;
  msg.context.k1          = uci.k1;
  msg.context.ss_id       = pdcch.ctx.context.ss_id;
  msg.context.nof_retxs   = h_dl->nof_retxs();
  msg.context.olla_offset = 0;

  switch (dci_type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0: {
      build_pdsch_f1_0_tc_rnti(
          msg.pdsch_cfg, pdsch_params, tbs_bytes, u.crnti, cell_cfg, pdcch.dci.tc_rnti_f1_0, vrbs, not is_retx);
      break;
    }
    case dci_dl_rnti_config_type::c_rnti_f1_0: {
      build_pdsch_f1_0_c_rnti(msg.pdsch_cfg,
                              pdsch_params,
                              tbs_bytes,
                              u.crnti,
                              cell_cfg,
                              u.get_pcell().cfg().search_space(pdcch.ctx.context.ss_id),
                              pdcch.dci.c_rnti_f1_0,
                              vrbs,
                              not is_retx);
      break;
    }
    default: {
      srsran_assert(false, "Invalid DCI type for SRB0 or SRB1");
    }
  }

  // Set MAC logical channels to schedule in this PDU.
  if (not is_retx) {
    u.build_dl_fallback_transport_block_info(msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes);
    msg.context.buffer_occupancy = u.pending_dl_newtx_bytes();
  }

  // Save in HARQ the parameters set for this PDCCH and PDSCH PDUs.
  dl_harq_alloc_context ctxt{pdcch.dci.type, std::nullopt, std::nullopt, std::nullopt, true};
  h_dl->save_grant_params(ctxt, msg);

  return *h_dl;
}

ue_fallback_scheduler::ul_srb_sched_outcome ue_fallback_scheduler::schedule_ul_ue(cell_resource_allocator& res_alloc,
                                                                                  ue&                      u)
{
  std::optional<ul_harq_process_handle> h_ul_retx     = u.get_pcell().harqs.find_pending_ul_retx();
  const bool                            is_retx       = h_ul_retx.has_value();
  const unsigned                        pending_bytes = u.pending_ul_newtx_bytes();
  if (not is_retx and pending_bytes == 0) {
    return ul_srb_sched_outcome::next_ue;
  }

  // The caller ensures the slot is UL enabled.
  const cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  slot_point                          pdcch_slot  = pdcch_alloc.slot;

  // Fetch applicable PUSCH Time Domain resource index list.
  static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS> pusch_td_res_index_list =
      get_pusch_td_resource_indices(cell_cfg, pdcch_slot);

  if (is_retx) {
    srsran_sanity_check(h_ul_retx->get_grant_params().dci_cfg_type == dci_ul_rnti_config_type::c_rnti_f0_0,
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
          is_retx ? pusch_td.symbols.length() == h_ul_retx->get_grant_params().nof_symbols : true;
      if (pusch_td.symbols.start() < start_ul_symbols or
          pusch_td.symbols.stop() > (start_ul_symbols + cell_cfg.get_nof_ul_symbol_per_slot(pusch_slot)) or
          !sym_length_match_prev_grant_for_retx) {
        // Try next PUSCH time domain resource value.
        continue;
      }

      if (pusch_alloc.result.ul.puschs.full() or pdcch_alloc.result.dl.ul_pdcchs.full()) {
        logger.warning(
            "ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: No space available in scheduler output list",
            fmt::underlying(u.ue_index),
            u.crnti,
            pusch_alloc.slot);
        continue;
      }

      // When checking the number of remaining grants for PUSCH, take into account that the PUCCH grants for this UE
      // will be removed when multiplexing the UCI on PUSCH.
      if (pusch_alloc.result.ul.puschs.size() >=
          expert_cfg.max_ul_grants_per_slot - static_cast<unsigned>(pusch_alloc.result.ul.pucchs.size())) {
        logger.info("ue={} rnti={}: Failed to allocate PUSCH. Cause: Max number of UL grants per slot {} was reached.",
                    fmt::underlying(u.ue_index),
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

      bool remove_pucch = false;
      if (existing_pucch != pusch_alloc.result.ul.pucchs.end()) {
        auto existing_pucch_count =
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
          if (existing_pucch_count == 1 and existing_pucch->format() == pucch_format::FORMAT_1 and
              existing_pucch->uci_bits.sr_bits != sr_nof_bits::no_sr and
              existing_pucch->uci_bits.harq_ack_nof_bits == 0) {
            // No PUSCH in slots with PUCCH. We cannot remove the PUCCH here, as we need to make sure the PUSCH will be
            // allocated. If not, we risk removing a PUCCH with SR opportunity.
            remove_pucch = true;
          } else {
            continue;
          }
        }
      }
      ul_srb_sched_outcome outcome = schedule_ul_srb(u, res_alloc, pusch_td_res_idx, pusch_td, h_ul_retx, remove_pucch);
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
                                       std::optional<ul_harq_process_handle>        h_ul_retx,
                                       bool                                         remove_pucch)
{
  ue_cell&                      ue_pcell    = u.get_pcell();
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  cell_slot_resource_allocator& pusch_alloc = res_alloc[pusch_td.k2 + cell_cfg.ntn_cs_koffset];

  const crb_interval init_ul_bwp_crbs = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs;

  crb_bitmap used_crbs = pusch_alloc.ul_res_grid.used_crbs(
      cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, init_ul_bwp_crbs, pusch_td.symbols);
  // Mark the CRBs used by PUCCH as occupied.
  used_crbs |= pucch_crbs;

  const bool is_retx = h_ul_retx.has_value();

  // Search for empty HARQ.
  if (not h_ul_retx.has_value() and not ue_pcell.harqs.has_empty_ul_harqs()) {
    logger.debug(
        "ue={} rnti={} PUSCH allocation skipped. Cause: no HARQ available", fmt::underlying(u.ue_index), u.crnti);
    return ul_srb_sched_outcome::next_ue;
  }

  if (used_crbs.all()) {
    logger.debug(
        "ue={} rnti={} PUSCH allocation skipped. Cause: No more RBs available", fmt::underlying(u.ue_index), u.crnti);
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
    const unsigned final_nof_prbs = h_ul_retx->get_grant_params().rbs.type1().length();
    final_mcs_tbs.mcs             = h_ul_retx->get_grant_params().mcs;
    final_mcs_tbs.tbs             = h_ul_retx->get_grant_params().tbs_bytes;

    ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, final_nof_prbs);
    if (ue_grant_crbs.empty() or ue_grant_crbs.length() < final_nof_prbs) {
      logger.debug("ue={} rnti={} PUSCH SRB allocation for re-tx skipped. Cause: available RBs {} < required RBs {}",
                   fmt::underlying(u.ue_index),
                   u.crnti,
                   ue_grant_crbs.length(),
                   final_nof_prbs);
      return ul_srb_sched_outcome::next_slot;
    }
  } else {
    pusch_mcs_table     fallback_mcs_table = pusch_mcs_table::qam64;
    sch_mcs_index       mcs                = ue_pcell.get_ul_mcs(fallback_mcs_table, pusch_cfg.use_transform_precoder);
    sch_mcs_description ul_mcs_cfg =
        pusch_mcs_get_config(fallback_mcs_table, mcs, cell_cfg.use_msg3_transform_precoder(), false);

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

    if (cell_cfg.use_msg3_transform_precoder()) {
      // Obtain a higher valid suggestion for the number of PRBs (if available).
      prbs_tbs.nof_prbs = transform_precoding::get_nof_prbs_upper_bound(prbs_tbs.nof_prbs).value_or(prbs_tbs.nof_prbs);
    }

    ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, prbs_tbs.nof_prbs);
    if (cell_cfg.use_msg3_transform_precoder()) {
      // Checks if the grant size is correct if transform precoding is enabled.
      auto valid_nof_rbs = transform_precoding::get_nof_prbs_lower_bound(ue_grant_crbs.length());
      if (not valid_nof_rbs.has_value()) {
        logger.debug(
            "ue={} rnti={} PUSCH allocation for SRB1 skipped. Cause: not possible to select a valid number of PRBs",
            fmt::underlying(u.ue_index),
            u.crnti);
        return ul_srb_sched_outcome::next_slot;
      }
      ue_grant_crbs.resize(valid_nof_rbs.value());
    }

    if (ue_grant_crbs.empty()) {
      logger.debug("ue={} rnti={} PUSCH allocation for SRB1 skipped. Cause: no PRBs available",
                   fmt::underlying(u.ue_index),
                   u.crnti);
      return ul_srb_sched_outcome::next_slot;
    }

    if (ue_grant_crbs.length() <= min_allocable_prbs and mcs < min_mcs_for_1_prb) {
      logger.debug("ue={} rnti={} PUSCH allocation for SRB1 skipped. Cause: the scheduler couldn't allocate the min. "
                   "number of PRBs={} for MCS={}",
                   fmt::underlying(u.ue_index),
                   u.crnti,
                   prbs_tbs.nof_prbs,
                   mcs.to_uint());
      return ul_srb_sched_outcome::next_slot;
    }

    bool contains_dc = dc_offset_helper::is_contained(
        cell_cfg.expert_cfg.ue.initial_ul_dc_offset, cell_cfg.nof_ul_prbs, ue_grant_crbs);

    expected<sch_mcs_tbs, compute_ul_mcs_tbs_error> mcs_tbs_info =
        compute_ul_mcs_tbs(pusch_cfg, ue_pcell.active_bwp(), mcs, ue_grant_crbs.length(), contains_dc);

    // If there is not MCS-TBS info, it means no MCS exists such that the effective code rate is <= 0.95.
    if (not mcs_tbs_info.has_value()) {
      logger.warning("ue={} rnti={}: Failed to allocate PUSCH for SRB1. Cause: no MCS such that code rate <= 0.95",
                     fmt::underlying(u.ue_index),
                     u.crnti);
      return ul_srb_sched_outcome::next_slot;
    }
    final_mcs_tbs = mcs_tbs_info.value();
  }

  // Allocate PDCCH position.
  pdcch_ul_information* pdcch =
      pdcch_sch.alloc_ul_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.get_id(), aggregation_level::n4);
  if (pdcch == nullptr) {
    logger.info(
        "ue={} rnti={}: Failed to allocate PUSCH. Cause: No space in PDCCH.", fmt::underlying(u.ue_index), u.crnti);
    return ul_srb_sched_outcome::stop_ul_scheduling;
  }

  if (remove_pucch) {
    // If the PUCCH needs to be removed, it implies the UE has the dedicated config. This is because the only
    // case in which we remove the PUCCH is when the UCI bits only have SR (see explanation in the calling function).
    srsran_assert(u.ue_cfg_dedicated() != nullptr, "UE has no dedicated configuration");
    pucch_alloc.remove_ue_uci_from_pucch(pusch_alloc, u.crnti, u.get_pcell().cfg());
  }

  // Mark resources as occupied in the ResourceGrid.
  pusch_alloc.ul_res_grid.fill(
      grant_info{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, pusch_td.symbols, ue_grant_crbs});

  // Update the number of PRBs used in the PUSCH allocation.
  u.get_pcell().get_pusch_power_controller().update_pusch_pw_ctrl_state(pusch_alloc.slot, ue_grant_crbs.length());

  // Update DRX controller state.
  u.drx_controller().on_new_ul_pdcch_alloc(pdcch_alloc.slot, pusch_alloc.slot);

  fill_ul_srb_grant(u,
                    pdcch_alloc.slot,
                    h_ul_retx,
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

void ue_fallback_scheduler::fill_ul_srb_grant(ue&                                   u,
                                              slot_point                            pdcch_slot,
                                              std::optional<ul_harq_process_handle> h_ul,
                                              pdcch_ul_information&                 pdcch,
                                              ul_sched_info&                        msg,
                                              unsigned                              pusch_time_res,
                                              unsigned                              k2,
                                              sch_mcs_index                         mcs_idx,
                                              const crb_interval&                   ue_grant_crbs,
                                              const pusch_config_params&            pusch_params,
                                              unsigned                              tbs_bytes,
                                              bool                                  is_retx)
{
  if (is_retx) {
    // It is a retx.
    bool result = h_ul->new_retx(pdcch_slot + k2 + cell_cfg.ntn_cs_koffset);
    srsran_sanity_check(result, "Failed to setup HARQ retx");
  } else {
    // It is a new tx.
    h_ul =
        u.get_pcell().harqs.alloc_ul_harq(pdcch_slot + k2 + cell_cfg.ntn_cs_koffset, expert_cfg.max_nof_ul_harq_retxs);
  }

  uint8_t                  rv                  = u.get_pcell().get_pusch_rv(h_ul->nof_retxs());
  static constexpr uint8_t default_tpc_command = 1U;
  const vrb_interval       vrbs                = rb_helper::crb_to_vrb_ul_non_interleaved(
      ue_grant_crbs,
      u.get_pcell().cfg().search_space(pdcch.ctx.context.ss_id).bwp->ul_common->value().generic_params.crbs.start());
  build_dci_f0_0_c_rnti(pdcch.dci,
                        u.get_pcell().cfg().search_space(pdcch.ctx.context.ss_id),
                        cell_cfg.ul_cfg_common.init_ul_bwp,
                        vrbs,
                        pusch_time_res,
                        mcs_idx,
                        rv,
                        *h_ul,
                        default_tpc_command);

  // Fill PDSCH PDU.
  msg.context.ue_index  = u.ue_index;
  msg.context.ss_id     = ss_cfg.get_id();
  msg.context.k2        = k2;
  msg.context.nof_retxs = h_ul->nof_retxs();
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
                          vrbs,
                          not is_retx);

  // Save set PDCCH and PUSCH PDU parameters in HARQ process.
  h_ul->save_grant_params(ul_harq_alloc_context{pdcch.dci.type}, msg.pusch_cfg);

  // Notify UL TB scheduling.
  u.handle_ul_transport_block_info(msg.pusch_cfg.tb_size_bytes);
}

const pdsch_time_domain_resource_allocation& ue_fallback_scheduler::get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const
{
  return cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdsch_time_res_idx];
}

std::optional<unsigned>
ue_fallback_scheduler::get_pdsch_time_res_idx(const pdsch_config_common&                   pdsch_cfg,
                                              slot_point                                   sl_tx,
                                              const std::optional<dl_harq_process_handle>& h_dl_retx) const
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
    if (h_dl_retx.has_value()) {
      if (h_dl_retx->get_grant_params().nof_symbols != pdsch_td_cfg.symbols.length()) {
        continue;
      }
      candidate_pdsch_time_res_idx = time_res_idx;
      return candidate_pdsch_time_res_idx;
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

void ue_fallback_scheduler::store_harq_tx(du_ue_index_t ue_index, const dl_harq_process_handle& h_dl)
{
  srsran_sanity_check(ongoing_ues_ack_retxs.end() ==
                          std::find_if(ongoing_ues_ack_retxs.begin(),
                                       ongoing_ues_ack_retxs.end(),
                                       [ue_index, h_dl](const ack_and_retx_tracker& tracker) {
                                         return tracker.match_ue_harq(ue_index, h_dl);
                                       }),
                      "This UE and HARQ process were already in the list");

  ongoing_ues_ack_retxs.emplace_back(ue_index, h_dl);
}

void ue_fallback_scheduler::slot_indication(slot_point sl)
{
  // If there is any skipped slot, reset \ref slots_with_no_pdxch_space for all the skipped slots.
  if (SRSRAN_LIKELY(last_sl_ind.valid())) {
    if (SRSRAN_UNLIKELY(last_sl_ind + 1 != sl)) {
      logger.info("UE fallback scheduler: Detected skipped slots within [{}, {}).", last_sl_ind + 1, sl);
      while (last_sl_ind + 1 != sl) {
        // Reset the flag that indicates that there are no resources for the slot that has passed.
        slots_with_no_pdxch_space[last_sl_ind.to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = false;
        ++last_sl_ind;
      }
    }
  }

  // Reset the flag that indicates that there are no resources for the slot that has passed.
  slots_with_no_pdxch_space[(sl - 1).to_uint() % FALLBACK_SCHED_RING_BUFFER_SIZE] = false;
  last_sl_ind                                                                     = sl;

  // Remove any DL UE that is no longer in fallback mode. This happens when the higher layers confirm that the UE has
  // successfully received its config.
  for (auto ue_it = pending_dl_ues_new_tx.begin(); ue_it != pending_dl_ues_new_tx.end();) {
    if (not ues.contains(ue_it->ue_index)) {
      // UE was removed in the meantime.
      logger.debug(
          "ue={}: will be removed from fallback scheduler. Cause: not present anymore in the scheduler UE repository",
          fmt::underlying(ue_it->ue_index));
      ue_it = pending_dl_ues_new_tx.erase(ue_it);
      continue;
    }
    auto& u = ues[ue_it->ue_index];
    if (not u.get_pcell().is_in_fallback_mode()) {
      // UE exited fallback.
      logger.debug("ue={} rnti={}: will be removed from fallback scheduler. Cause: UE exited fallback mode",
                   fmt::underlying(ue_it->ue_index),
                   u.crnti);
      ue_it = pending_dl_ues_new_tx.erase(ue_it);
      continue;
    }
    if (not u.has_pending_dl_newtx_bytes()) {
      // UE has no new txs pending. It can be removed.
      logger.debug("ue={} rnti={}: will be removed from fallback scheduler. Cause: no pending new transmissions",
                   fmt::underlying(ue_it->ue_index),
                   u.crnti);
      ue_it = pending_dl_ues_new_tx.erase(ue_it);
      continue;
    }

    // Check if the \c ra-ContentionResolutionTimer has expired before the ConRes has been scheduled.
    if (not u.get_pcell().is_conres_complete() and u.get_pcell().get_msg3_rx_slot().valid()) {
      // We need to check if the UE has pending ACKs. If not and the ConRes procedure is not completed yes, it means
      // ra-ContentionResolutionTimer has expired.
      // NOTE If the gNB is waiting for a pending ACK, we'll handle it in the loop at the end of this function.
      const bool waiting_for_pending_acks =
          std::find_if(
              ongoing_ues_ack_retxs.begin(), ongoing_ues_ack_retxs.end(), [&u](const ack_and_retx_tracker& ue_tx) {
                return ue_tx.ue_index == u.ue_index and ue_tx.h_dl.is_waiting_ack();
              }) != ongoing_ues_ack_retxs.end();
      if (not waiting_for_pending_acks) {
        const auto ra_conres_timer_subframes = static_cast<uint32_t>(
            u.get_pcell().cfg().init_bwp().ul_common.value()->rach_cfg_common.value().ra_con_res_timer.count());
        const int slot_diff = sl - u.get_pcell().get_msg3_rx_slot();
        if (slot_diff < 0 or divide_ceil<uint32_t, uint32_t>(static_cast<uint32_t>(slot_diff),
                                                             sl.nof_slots_per_subframe()) > ra_conres_timer_subframes) {
          logger.warning("ue={} rnti={}: ra-ContentionResolutionTimer expired before UE's ConRes was scheduled. UE "
                         "will be removed from fallback scheduler",
                         fmt::underlying(u.ue_index),
                         u.crnti);
          // Remove the UE from the fallback scheduler.
          ue_it = pending_dl_ues_new_tx.erase(ue_it);
          pending_ul_ues.erase(std::remove(pending_ul_ues.begin(), pending_ul_ues.end(), u.ue_index),
                               pending_ul_ues.end());
          ongoing_ues_ack_retxs.erase(
              std::remove_if(ongoing_ues_ack_retxs.begin(),
                             ongoing_ues_ack_retxs.end(),
                             [&u](const ack_and_retx_tracker& tracker) { return tracker.ue_index == u.ue_index; }),
              ongoing_ues_ack_retxs.end());
          continue;
        }
      }
    }

    ++ue_it;
  }

  for (auto ue_it = pending_ul_ues.begin(); ue_it != pending_ul_ues.end();) {
    if (not ues.contains(*ue_it)) {
      // UE was removed in the meantime.
      ue_it = pending_ul_ues.erase(ue_it);
      continue;
    }
    auto& ue = ues[*ue_it];
    if (not ue.get_pcell().is_in_fallback_mode()) {
      // UE has left fallback mode.
      ue_it = pending_ul_ues.erase(ue_it);
      continue;
    }
    const auto& harqs               = ue.get_pcell().harqs;
    bool        all_harqs_are_empty = harqs.nof_ul_harqs() == harqs.nof_empty_ul_harqs();
    if (all_harqs_are_empty and ue.pending_ul_newtx_bytes() == 0) {
      // UE has no pending data.
      ue_it = pending_ul_ues.erase(ue_it);
      continue;
    }
    ++ue_it;
  }

  // Only remove the {UE, HARQ-process} elements that have been retransmitted and positively acked. The rest of the
  // elements are potential candidates for retransmissions.
  for (auto it_ue_harq = ongoing_ues_ack_retxs.begin(); it_ue_harq != ongoing_ues_ack_retxs.end();) {
    if (not ues.contains(it_ue_harq->ue_index) or not ues[it_ue_harq->ue_index].get_pcell().is_in_fallback_mode()) {
      it_ue_harq = ongoing_ues_ack_retxs.erase(it_ue_harq);
      continue;
    }
    if (it_ue_harq->h_dl.empty()) {
      it_ue_harq = ongoing_ues_ack_retxs.erase(it_ue_harq);
      continue;
    }

    // Check if the \c ra-ContentionResolutionTimer has expired before the ConRes has been tx-ed and acked.
    if (not ues[it_ue_harq->ue_index].get_pcell().is_conres_complete() and
        ues[it_ue_harq->ue_index].get_pcell().get_msg3_rx_slot().valid()) {
      // If the gNB is waiting for a pending ACK, we need to check if the slot at which the PDSCH will be sent is before
      // the ra-ContentionResolutionTimer will expire.
      const slot_point sl_tx                     = it_ue_harq->h_dl.pdsch_slot();
      const auto       ra_conres_timer_subframes = static_cast<uint32_t>(ues[it_ue_harq->ue_index]
                                                                       .get_pcell()
                                                                       .cfg()
                                                                       .init_bwp()
                                                                       .ul_common.value()
                                                                       ->rach_cfg_common.value()
                                                                       .ra_con_res_timer.count());
      const int        slot_diff                 = sl_tx - ues[it_ue_harq->ue_index].get_pcell().get_msg3_rx_slot();
      if (slot_diff < 0 or
          divide_ceil<uint32_t, uint32_t>(static_cast<uint32_t>(slot_diff), sl_tx.nof_slots_per_subframe()) >
              ra_conres_timer_subframes) {
        const auto  ue_index = it_ue_harq->ue_index;
        const auto& u        = ues[ue_index];
        logger.warning("ue={} rnti={}: ra-ContentionResolutionTimer expired before the UE has received and acked "
                       "ConRes. UE will be removed from fallback scheduler",
                       fmt::underlying(u.ue_index),
                       u.crnti);
        // Remove the UE from the fallback scheduler.
        it_ue_harq = ongoing_ues_ack_retxs.erase(it_ue_harq);
        pending_dl_ues_new_tx.erase(
            std::remove_if(pending_dl_ues_new_tx.begin(),
                           pending_dl_ues_new_tx.end(),
                           [ue_index = u.ue_index](const fallback_ue& ue) { return ue.ue_index == ue_index; }),
            pending_dl_ues_new_tx.end());
        pending_ul_ues.erase(std::remove(pending_ul_ues.begin(), pending_ul_ues.end(), u.ue_index),
                             pending_ul_ues.end());
        continue;
      }
    }
    ++it_ue_harq;
  }
}
