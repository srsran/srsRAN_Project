/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_srb0_scheduler.h"
#include "../support/csi_rs_helpers.h"
#include "../support/dci_builder.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdsch/pdsch_resource_allocation.h"
#include "../support/prbs_calculator.h"
#include "srsran/ran/sch/tbs_calculator.h"

using namespace srsran;

ue_srb0_scheduler::ue_srb0_scheduler(const scheduler_ue_expert_config& expert_cfg_,
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
}

void ue_srb0_scheduler::run_slot(cell_resource_allocator& res_alloc)
{
  update_ongoing_ue_retxs();

  if (ues.empty()) {
    return;
  }

  const cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  if (not cell_cfg.is_dl_enabled(pdcch_alloc.slot)) {
    return;
  }

  auto next_ue_harq_retx = ongoing_ues_ack_retxs.begin();
  while (next_ue_harq_retx != ongoing_ues_ack_retxs.end()) {
    // TODO: Add a stopping condition in this loop, so next_ue won't iterate over all the UEs and HARQ-processes if
    //       there are no resources.

    auto& u    = *ues.find(next_ue_harq_retx->ue_index);
    auto* h_dl = next_ue_harq_retx->get_harq_process();

    if (h_dl->has_pending_retx()) {
      optional<slot_point> most_recent_tx = get_most_recent_slot_tx(u.ue_index);
      if (next_ue_harq_retx->is_srb0) {
        schedule_srb(res_alloc, u, true, h_dl, most_recent_tx);
      } else {
        // TODO: Change this into SRB1_retx.
        schedule_srb(res_alloc, u, true, h_dl, most_recent_tx);
      }
    }
    ++next_ue_harq_retx;
  }

  // Schedule SRB0 messages.
  auto next_ue = pending_ues.begin();
  while (next_ue != pending_ues.end()) {
    if (not next_ue->is_srb0) {
      ++next_ue;
      continue;
    }

    // The UE might have been deleted in the meantime, check if still exists.
    if (not ues.contains(next_ue->ue_index)) {
      next_ue = pending_ues.erase(next_ue);
      continue;
    }

    auto&                u              = ues[next_ue->ue_index];
    optional<slot_point> most_recent_tx = get_most_recent_slot_tx(u.ue_index);
    if (u.has_pending_dl_newtx_bytes(LCID_SRB0) and schedule_srb(res_alloc, u, true, nullptr, most_recent_tx)) {
      next_ue = pending_ues.erase(next_ue);
    } else {
      ++next_ue;
    }
  }

  // Keep SRB1 with lower priority than SRB0.
  next_ue = pending_ues.begin();
  while (next_ue != pending_ues.end()) {
    if (next_ue->is_srb0) {
      ++next_ue;
      continue;
    }

    // The UE might have been deleted in the meantime, check if still exists.
    if (not ues.contains(next_ue->ue_index)) {
      next_ue = pending_ues.erase(next_ue);
      continue;
    }

    auto&                u              = ues[next_ue->ue_index];
    optional<slot_point> most_recent_tx = get_most_recent_slot_tx(u.ue_index);
    if (u.has_pending_dl_newtx_bytes(LCID_SRB1) and schedule_srb(res_alloc, u, false, nullptr, most_recent_tx)) {
      next_ue = pending_ues.erase(next_ue);
    } else {
      ++next_ue;
    }
  }
}

void ue_srb0_scheduler::handle_dl_buffer_state_indication_srb(du_ue_index_t ue_index, bool is_srb0)
{
  is_srb0 ? pending_ues.push_back({ue_index, is_srb0}) : pending_ues.push_back({ue_index, is_srb0});
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

bool ue_srb0_scheduler::schedule_srb(cell_resource_allocator& res_alloc,
                                     ue&                      u,
                                     bool                     is_srb0,
                                     dl_harq_process*         h_dl_retx,
                                     optional<slot_point>     starting_sl)
{
  const auto& bwp_cfg_common = cell_cfg.dl_cfg_common.init_dl_bwp;
  // Search valid PDSCH time domain resource.

  const bool is_retx = h_dl_retx != nullptr;

  // \ref sched_ref_slot is the slot that we take as reference for the scheduler, which is processed when calling the
  // slot_indication().
  // \ref starting_sl is the slot from which the SRB0 starts scheduling this given UE. Assuming the UE was assigned a
  // PDSCH grant for SRB1 that was fragmented, we want to avoid allocating the second part of SRB1 in a PDSCH that is
  // scheduled for an earlier slot than the PDSCH of the first part of the SRB1.
  // TODO: Make this compatible with k0 > 0.
  slot_point sched_ref_slot = res_alloc[0].slot;

  if (not cell_cfg.is_dl_enabled(sched_ref_slot)) {
    return false;
  }

  if (starting_sl.has_value() and sched_ref_slot + max_dl_slots_ahead_sched < starting_sl.value()) {
    return false;
  }
  starting_sl = sched_ref_slot;

  // We keep track of the number of scheduling attempts for the given UE.
  unsigned   sched_attempts_cnt = 0;
  slot_point next_slot          = starting_sl.value();

  while (next_slot <= sched_ref_slot + max_dl_slots_ahead_sched) {
    unsigned                            offset_to_sched_ref_slot = static_cast<unsigned>(next_slot - sched_ref_slot);
    const cell_slot_resource_allocator& pdcch_alloc              = res_alloc[offset_to_sched_ref_slot];

    for (unsigned time_res_idx = 0; time_res_idx != bwp_cfg_common.pdsch_common.pdsch_td_alloc_list.size();
         ++time_res_idx) {
      if (sched_attempts_cnt >= max_sched_attempts_per_ue) {
        return false;
      }

      const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(time_res_idx);
      const cell_slot_resource_allocator&          pdsch_alloc  = res_alloc[offset_to_sched_ref_slot + pdsch_td_cfg.k0];

      if (not cell_cfg.is_dl_enabled(pdsch_alloc.slot)) {
        continue;
      }

      // We do not support multiplexing of PDSCH for SRB0 and SRB1 when in fallback with CSI-RS.
      const bool is_csi_rs_slot = next_slot == sched_ref_slot ? not pdsch_alloc.result.dl.csi_rs.empty()
                                                              : csi_helper::is_csi_rs_slot(cell_cfg, pdsch_alloc.slot);
      if (is_csi_rs_slot) {
        continue;
      }

      // Check whether PDSCH time domain resource does not overlap with CORESET.
      if (pdsch_td_cfg.symbols.start() < ss_cfg.get_first_symbol_index() + cs_cfg.duration) {
        continue;
      }

      // Check whether PDSCH time domain resource fits in DL symbols of the slot.
      if (pdsch_td_cfg.symbols.stop() > cell_cfg.get_nof_dl_symbol_per_slot(pdsch_alloc.slot)) {
        continue;
      }

      if (is_retx and h_dl_retx->last_alloc_params().nof_symbols != pdsch_td_cfg.symbols.length()) {
        continue;
      }

      // Verify there is space in PDSCH and PDCCH result lists for new allocations.
      if (pdcch_alloc.result.dl.dl_pdcchs.full() or pdsch_alloc.result.dl.ue_grants.full()) {
        logger.debug("rnti={}: Failed to allocate PDSCH for {}. Cause: No space available in scheduler output list.",
                     u.crnti,
                     is_srb0 ? "SRB0" : "SRB1");
        return false;
      }

      dl_harq_process* candidate_h_dl =
          is_srb0 ? schedule_srb0(u, res_alloc, time_res_idx, offset_to_sched_ref_slot, h_dl_retx)
                  : schedule_srb1(u, res_alloc, time_res_idx, offset_to_sched_ref_slot, h_dl_retx);

      const bool alloc_successful = candidate_h_dl != nullptr;
      if (alloc_successful) {
        if (not is_retx) {
          store_harq_tx(u.ue_index, candidate_h_dl, is_srb0);
        }
        return true;
      }

      ++sched_attempts_cnt;
    }

    next_slot = get_next_srb_slot(cell_cfg, next_slot);
  }

  // No resource found in UE's carriers and Search spaces.
  slot_point pdcch_slot = res_alloc[0].slot;
  logger.debug("rnti={}: Not enough PDSCH space for {} message found in any of the slots:[{},{}).",
               u.crnti,
               is_srb0 ? "SRB0" : "SRB1",
               pdcch_slot,
               pdcch_slot + max_dl_slots_ahead_sched);
  return false;
}

dl_harq_process* ue_srb0_scheduler::schedule_srb0(ue&                      u,
                                                  cell_resource_allocator& res_alloc,
                                                  unsigned                 pdsch_time_res,
                                                  unsigned                 slot_offset,
                                                  dl_harq_process*         h_dl_retx)
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
    return nullptr;
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

    // Try to find least MCS to fit SRB0 message.
    while (mcs_idx <= expert_cfg.max_msg4_mcs) {
      // As per TS 38.214, clause 5.1.3.1, if the PDSCH is scheduled by a PDCCH with CRC scrambled by TC-RNTI, the UE
      // use "Table 5.1.3.1-1: MCS index table 1" for MCS mapping. This is not stated explicitly, but can be inferred
      // from the sentence "... the UE shall use I_MCS and Table 5.1.3.1-1 to determine the modulation order (Qm) and
      // Target code rate (R) used in the physical downlink shared channel.".

      // At this point, xOverhead is not configured yet. As per TS 38.214, Clause 5.1.3.2, xOverhead is assumed to be 0.
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
          "rnti={}: SRB0 PDU size ({}) exceeds TBS calculated ({}).", pending_bytes, prbs_tbs.tbs_bytes, u.crnti);
      return nullptr;
    }

    if (mcs_idx > expert_cfg.max_msg4_mcs) {
      logger.debug("rnti={}: Postponing SRB0 PDU allocation. Cause: MCS index chosen ({}) for SRB0 exceeds maximum"
                   "allowed MCS index ({})",
                   u.crnti,
                   mcs_idx,
                   expert_cfg.max_msg4_mcs);
      return nullptr;
    }
  }

  crb_interval ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, prbs_tbs.nof_prbs, 0);
  if (ue_grant_crbs.length() < prbs_tbs.nof_prbs) {
    logger.debug("rnti={}: Postponed SRB0 PDU scheduling. Cause: Not enough PRBs ({} < {})",
                 u.crnti,
                 ue_grant_crbs.length(),
                 prbs_tbs.nof_prbs);
    return nullptr;
  }

  // Allocate PDCCH resources.
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[slot_offset];
  pdcch_dl_information*         pdcch =
      pdcch_sch.alloc_dl_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.get_id(), aggregation_level::n4);
  if (pdcch == nullptr) {
    logger.debug("rnti={}: Postponed SRB0 PDU scheduling. Cause: No space in PDCCH.", u.crnti);
    return nullptr;
  }

  // Allocate PUCCH resources.
  unsigned k1 = 4;
  // Minimum k1 value supported is 4.
  static const std::array<uint8_t, 5> dci_1_0_k1_values = {4, 5, 6, 7, 8};
  optional<unsigned>                  pucch_res_indicator;
  for (const auto k1_candidate : dci_1_0_k1_values) {
    pucch_res_indicator = pucch_alloc.alloc_common_pucch_harq_ack_ue(
        res_alloc, u.crnti, slot_offset + pdsch_td_cfg.k0, k1_candidate, *pdcch);
    if (pucch_res_indicator.has_value()) {
      k1 = k1_candidate;
      break;
    }
  }
  if (not pucch_res_indicator.has_value()) {
    logger.debug("Failed to allocate PDSCH for SRB0. Cause: No space in PUCCH.");
    pdcch_sch.cancel_last_pdcch(pdcch_alloc);
    return nullptr;
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, ue_grant_crbs});

  fill_srb0_grant(u,
                  pdsch_alloc.slot,
                  *h_dl,
                  *pdcch,
                  pdsch_alloc.result.dl.ue_grants.emplace_back(),
                  pucch_res_indicator.value(),
                  pdsch_time_res,
                  k1,
                  mcs_idx,
                  ue_grant_crbs,
                  pdsch_cfg,
                  prbs_tbs.tbs_bytes,
                  is_retx);

  return h_dl;
}

dl_harq_process* ue_srb0_scheduler::schedule_srb1(ue&                      u,
                                                  cell_resource_allocator& res_alloc,
                                                  unsigned                 pdsch_time_res,
                                                  unsigned                 slot_offset,
                                                  dl_harq_process*         h_dl_retx)
{
  ue_cell&                                     ue_pcell     = u.get_pcell();
  const subcarrier_spacing                     scs          = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(pdsch_time_res);

  const bool is_retx = h_dl_retx != nullptr;

  // Search for empty HARQ.
  dl_harq_process* h_dl = is_retx ? h_dl_retx : ue_pcell.harqs.find_empty_dl_harq();
  if (h_dl == nullptr) {
    return nullptr;
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
  // for the cell, as per TS 38.213, Section 7.3.1.0, we should consider the size of CORESET#0  as the size for the BWP.
  cell_slot_resource_allocator& pdsch_alloc = res_alloc[slot_offset + pdsch_td_cfg.k0];
  prb_bitmap                    used_crbs   = pdsch_alloc.dl_res_grid.used_crbs(
      initial_active_dl_bwp.scs,
      pdsch_helper::get_ra_crb_limits_common(cell_cfg.dl_cfg_common.init_dl_bwp, ss_cfg.get_id()),
      pdsch_cfg.symbols);

  crb_interval ue_grant_crbs;
  sch_mcs_tbs  final_mcs_tbs;

  if (is_retx) {
    const unsigned final_nof_prbs = h_dl->last_alloc_params().rbs.type1().length();
    final_mcs_tbs.mcs             = h_dl->last_alloc_params().tb.front().value().mcs;
    final_mcs_tbs.tbs             = h_dl->last_alloc_params().tb.front().value().tbs_bytes;

    ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, final_nof_prbs, 0);
    if (ue_grant_crbs.empty() or ue_grant_crbs.length() < final_nof_prbs) {
      // return alloc_outcome::skip_ue;
      return nullptr;
    }

  } else {
    // Find available symbol x RB resources.
    const unsigned pending_bytes     = dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0
                                           ? u.pending_dl_srb0_or_srb1_newtx_bytes(false)
                                           : u.pending_dl_newtx_bytes(lcid_t::LCID_SRB1);
    grant_prbs_mcs mcs_prbs_estimate = ue_pcell.required_dl_prbs(pdsch_td_cfg, pending_bytes, dci_type);

    if (mcs_prbs_estimate.n_prbs == 0) {
      // return alloc_outcome::skip_ue;
      return nullptr;
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
      // return alloc_outcome::skip_ue;
      return nullptr;
    }

    const unsigned final_nof_prbs = ue_grant_crbs.length();
    // As \c txDirectCurrentLocation, in \c SCS-SpecificCarrier, TS 38.331, "If this field (\c txDirectCurrentLocation)
    // is absent for downlink within ServingCellConfigCommon and ServingCellConfigCommonSIB, the UE assumes the default
    // value of 3300 (i.e. "Outside the carrier")".
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
      return nullptr;
    }
    final_mcs_tbs = mcs_tbs.value();
  }

  // Allocate PDCCH resources.
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[slot_offset];
  pdcch_dl_information*         pdcch =
      pdcch_sch.alloc_dl_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.get_id(), aggregation_level::n4);
  if (pdcch == nullptr) {
    logger.debug("rnti={}: Postponed SRB0 PDU scheduling. Cause: No space in PDCCH.", u.crnti);
    return nullptr;
  }

  // Allocate PUCCH resources.
  unsigned k1 = 4;
  // Minimum k1 value supported is 4.
  static const std::array<uint8_t, 5> dci_1_0_k1_values = {4, 5, 6, 7, 8};
  optional<unsigned>                  pucch_res_indicator;
  for (const auto k1_candidate : dci_1_0_k1_values) {
    pucch_res_indicator = pucch_alloc.alloc_common_pucch_harq_ack_ue(
        res_alloc, u.crnti, slot_offset + pdsch_td_cfg.k0, k1_candidate, *pdcch);
    if (pucch_res_indicator.has_value()) {
      k1 = k1_candidate;
      break;
    }
  }
  if (not pucch_res_indicator.has_value()) {
    logger.debug("Failed to allocate PDSCH for SRB0. Cause: No space in PUCCH.");
    pdcch_sch.cancel_last_pdcch(pdcch_alloc);
    return nullptr;
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, ue_grant_crbs});

  fill_srb1_grant(u,
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
                  is_retx);

  return h_dl;
}

void ue_srb0_scheduler::fill_srb0_grant(ue&                        u,
                                        slot_point                 pdsch_slot,
                                        dl_harq_process&           h_dl,
                                        pdcch_dl_information&      pdcch,
                                        dl_msg_alloc&              msg,
                                        unsigned                   pucch_res_indicator,
                                        unsigned                   pdsch_time_res,
                                        unsigned                   k1,
                                        sch_mcs_index              mcs_idx,
                                        const crb_interval&        ue_grant_crbs,
                                        const pdsch_config_params& pdsch_params,
                                        unsigned                   tbs_bytes,
                                        bool                       is_retx)
{
  // Allocate DL HARQ.
  // NOTE: We do not multiplex the SRBO PUCCH with existing PUCCH HARQs, thus both DAI and HARQ-ACK bit index are 0.
  if (not is_retx) {
    const bool               is_fallback = true;
    static constexpr uint8_t srb0_dai    = 0;
    h_dl.new_tx(pdsch_slot,
                k1,
                expert_cfg.max_nof_harq_retxs,
                srb0_dai,
                u.get_pcell().channel_state_manager().get_wideband_cqi(),
                u.get_pcell().channel_state_manager().get_nof_dl_layers(),
                is_fallback);
  } else {
    const unsigned harq_bit_idx = 0U;
    h_dl.new_retx(pdsch_slot, k1, harq_bit_idx);
  }

  // Fill DL PDCCH DCI.
  static const uint8_t msg4_rv = 0;
  build_dci_f1_0_tc_rnti(pdcch.dci,
                         cell_cfg.dl_cfg_common.init_dl_bwp,
                         ue_grant_crbs,
                         pdsch_time_res,
                         k1,
                         pucch_res_indicator,
                         mcs_idx,
                         msg4_rv,
                         h_dl);

  // Fill PDSCH PDU.
  msg.context.ue_index    = u.ue_index;
  msg.context.k1          = k1;
  msg.context.ss_id       = pdcch.ctx.context.ss_id;
  msg.context.nof_retxs   = h_dl.tb(0).nof_retxs;
  msg.context.olla_offset = 0;
  build_pdsch_f1_0_tc_rnti(
      msg.pdsch_cfg, pdsch_params, tbs_bytes, u.crnti, cell_cfg, pdcch.dci.tc_rnti_f1_0, ue_grant_crbs, not is_retx);

  if (not is_retx) {
    // Set MAC logical channels to schedule in this PDU.
    u.build_dl_fallback_transport_block_info(
        msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes, true);
  }

  // Save in HARQ the parameters set for this PDCCH and PDSCH PDUs.
  h_dl.save_alloc_params(pdcch.dci.type, msg.pdsch_cfg);
}

void ue_srb0_scheduler::fill_srb1_grant(ue&                        u,
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
                                        bool                       is_retx)
{
  // Allocate DL HARQ.
  // NOTE: We do not multiplex the SRBO PUCCH with existing PUCCH HARQs, thus both DAI and HARQ-ACK bit index are 0.
  static constexpr uint8_t srb1_dai = 0;
  if (not is_retx) {
    const bool is_fallback = true;
    h_dl.new_tx(pdsch_slot,
                k1,
                expert_cfg.max_nof_harq_retxs,
                srb1_dai,
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
                            srb1_dai,
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
            msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes, false);
      }
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
                              ue_grant_crbs,
                              not is_retx);
      // Set MAC logical channels to schedule in this PDU.
      if (not is_retx) {
        u.build_dl_transport_block_info(
            msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes, lcid_t::LCID_SRB1);
      }
      break;
    }
    default: {
      srsran_assert(false, "Invalid DCI type for SRB1");
    }
  }

  // Save in HARQ the parameters set for this PDCCH and PDSCH PDUs.
  h_dl.save_alloc_params(pdcch.dci.type, msg.pdsch_cfg);
}

const pdsch_time_domain_resource_allocation& ue_srb0_scheduler::get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const
{
  return cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdsch_time_res_idx];
}

optional<slot_point> ue_srb0_scheduler::get_most_recent_slot_tx(du_ue_index_t ue_idx) const
{
  optional<slot_point> most_recent_tx;
  for (const auto& ue_proc : ongoing_ues_ack_retxs) {
    if (ue_proc.ue_index == ue_idx) {
      slot_point h_dl_slot_tx = ue_proc.get_harq_process()->slot_tx();
      if (most_recent_tx.has_value() and h_dl_slot_tx > most_recent_tx.value()) {
        most_recent_tx = ue_proc.get_harq_process()->slot_tx();
      } else {
        most_recent_tx.emplace(ue_proc.get_harq_process()->slot_tx());
      }
    }
  }
  return most_recent_tx;
}

void ue_srb0_scheduler::store_harq_tx(du_ue_index_t ue_index, dl_harq_process* h_dl, bool is_srb0)
{
  srsran_assert(ongoing_ues_ack_retxs.end() == std::find_if(ongoing_ues_ack_retxs.begin(),
                                                            ongoing_ues_ack_retxs.end(),
                                                            [ue_index, h_dl](const ack_and_retx_tracker& tracker) {
                                                              return tracker.match_ue_harq(ue_index, h_dl);
                                                            }),
                "This UE and HARQ process were already in the list");

  ongoing_ues_ack_retxs.emplace_back(ue_index, h_dl, is_srb0, ues);
}

void ue_srb0_scheduler::update_ongoing_ue_retxs()
{
  if (ongoing_ues_ack_retxs.empty()) {
    return;
  }

  // Only remove the {UE, HARQ-process} elements that have been retransmitted and positively acked. The rest of the
  // elements are potential candidate for retransmissions.
  auto it_ue_harq = ongoing_ues_ack_retxs.begin();
  while (it_ue_harq != ongoing_ues_ack_retxs.end()) {
    if (not ues.contains(it_ue_harq->ue_index)) {
      it_ue_harq = ongoing_ues_ack_retxs.erase(it_ue_harq);
      continue;
    }
    dl_harq_process& h_dl = *it_ue_harq->get_harq_process();
    if (h_dl.empty()) {
      it_ue_harq = ongoing_ues_ack_retxs.erase(it_ue_harq);
      continue;
    }
    ++it_ue_harq;
  }
}
