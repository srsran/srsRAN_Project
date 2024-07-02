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

#include "ue_cell_grid_allocator.h"
#include "../support/csi_report_helpers.h"
#include "../support/dci_builder.h"
#include "../support/mcs_calculator.h"
#include "../support/sched_result_helpers.h"
#include "ue_pdsch_alloc_param_candidate_searcher.h"
#include "ue_pusch_alloc_param_candidate_searcher.h"
#include "srsran/ran/pdcch/coreset.h"
#include "srsran/scheduler/scheduler_dci.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

ue_cell_grid_allocator::ue_cell_grid_allocator(const scheduler_ue_expert_config& expert_cfg_,
                                               ue_repository&                    ues_,
                                               srslog::basic_logger&             logger_) :
  expert_cfg(expert_cfg_), ues(ues_), logger(logger_)
{
}

void ue_cell_grid_allocator::add_cell(du_cell_index_t           cell_index,
                                      pdcch_resource_allocator& pdcch_sched,
                                      uci_allocator&            uci_alloc,
                                      cell_resource_allocator&  cell_alloc)
{
  cells.emplace(cell_index, cell_t{cell_index, &pdcch_sched, &uci_alloc, &cell_alloc});
}

void ue_cell_grid_allocator::slot_indication(slot_point sl)
{
  dl_attempts_count = 0;
  ul_attempts_count = 0;
  // Clear slots which are in the past relative to current slot indication.
  auto* pdsch_slot = slots_with_no_pdsch_space.begin();
  while (pdsch_slot != slots_with_no_pdsch_space.end()) {
    if (*pdsch_slot < sl) {
      pdsch_slot = slots_with_no_pdsch_space.erase(pdsch_slot);
      continue;
    }
    ++pdsch_slot;
  }
  auto* pusch_slot = slots_with_no_pusch_space.begin();
  while (pusch_slot != slots_with_no_pusch_space.end()) {
    if (*pusch_slot < sl) {
      pusch_slot = slots_with_no_pusch_space.erase(pusch_slot);
      continue;
    }
    ++pusch_slot;
  }
}

alloc_result ue_cell_grid_allocator::allocate_dl_grant(const ue_pdsch_grant& grant)
{
  srsran_assert(ues.contains(grant.user->ue_index), "Invalid UE candidate index={}", grant.user->ue_index);
  srsran_assert(has_cell(grant.cell_index), "Invalid UE candidate cell_index={}", grant.cell_index);

  if (dl_attempts_count++ >= expert_cfg.max_pdcch_alloc_attempts_per_slot) {
    logger.debug("Stopping DL allocations. Cause: Max number of DL PDCCH allocation attempts {} reached.",
                 expert_cfg.max_pdcch_alloc_attempts_per_slot);
    return {alloc_status::skip_slot};
  }

  ue& u = ues[grant.user->ue_index];

  // Verify UE carrier is active.
  ue_cell* ue_cc = u.find_cell(grant.cell_index);
  if (ue_cc == nullptr) {
    logger.warning("PDSCH allocation failed. Cause: The ue={} carrier with cell_index={} is inactive",
                   u.ue_index,
                   grant.cell_index);
    return {alloc_status::skip_ue};
  }

  const ue_cell_configuration& ue_cell_cfg = ue_cc->cfg();
  const cell_configuration&    cell_cfg    = ue_cell_cfg.cell_cfg_common;
  const bwp_downlink_common&   bwp_dl_cmn  = *ue_cell_cfg.bwp(ue_cc->active_bwp_id()).dl_common;
  dl_harq_process&             h_dl        = ue_cc->harqs.dl_harq(grant.h_id);
  const bool                   is_retx     = not h_dl.empty();

  // Fetch PDCCH resource grid allocator.
  cell_slot_resource_allocator& pdcch_alloc = get_res_alloc(grant.cell_index)[0];
  if (not cell_cfg.is_dl_enabled(pdcch_alloc.slot)) {
    logger.warning("ue={} rnti={}: Failed to allocate PDSCH. Cause: DL is not active in the PDCCH slot={}",
                   u.ue_index,
                   u.crnti,
                   pdcch_alloc.slot);
    return {alloc_status::skip_slot};
  }

  if (not ue_cc->is_active() and not is_retx) {
    // newTxs are not allowed for inactive UEs.
    logger.warning("ue={} rnti={}: Failed to allocate PDSCH. Cause: Carrier with cell_index={} is inactive. New DL Tx "
                   "transmissions are not allowed",
                   u.ue_index,
                   u.crnti,
                   grant.cell_index);
    return {alloc_status::skip_ue};
  }

  if (ue_cc->is_in_fallback_mode()) {
    // Skip allocation for UEs in fallback mode, as it is handled by the SRB fallback scheduler.
    return {alloc_status::skip_ue};
  }

  if (not is_retx and not grant.recommended_nof_bytes.has_value()) {
    logger.warning("ue={} rnti={}: Failed to allocate PDSCH. Cause: Recommended nof. bytes to schedule is not given "
                   "for new Tx with h_id={}",
                   u.ue_index,
                   u.crnti,
                   grant.h_id);
    return {alloc_status::invalid_params};
  }

  // [Implementation-defined]
  // If there is large gap between two PDSCHs scheduled for a UE, \c last_pdsch_allocated_slot could be having an old
  // slot value and the condition pdsch_alloc.slot (e.g. 47.0) <= ue_cc->last_pdsch_allocated_slot (e.g. 989.0) maybe be
  // true for long time and UE may not get scheduled.
  // This scenario can be prevented by resetting \c last_pdsch_allocated_slot when its behind current PDCCH slot by
  // SCHEDULER_MAX_K0 number of slots.
  if (ue_cc->last_pdsch_allocated_slot.valid() and
      std::abs(pdcch_alloc.slot - ue_cc->last_pdsch_allocated_slot) > SCHEDULER_MAX_K0) {
    ue_cc->last_pdsch_allocated_slot.clear();
  }

  // Create PDSCH param candidate search object.
  ue_pdsch_alloc_param_candidate_searcher candidates{
      u, grant.cell_index, h_dl, pdcch_alloc.slot, slots_with_no_pdsch_space};
  if (candidates.is_empty()) {
    // The conditions for a new PDSCH allocation for this UE were not met (e.g. lack of available SearchSpaces).
    return {alloc_status::skip_ue};
  }

  // Iterate through allocation parameter candidates.
  for (const ue_pdsch_alloc_param_candidate_searcher::candidate& param_candidate : candidates) {
    const pdsch_time_domain_resource_allocation& pdsch_td_cfg = param_candidate.pdsch_td_res();
    const search_space_info&                     ss_info      = param_candidate.ss();
    const dci_dl_rnti_config_type                dci_type     = param_candidate.dci_dl_rnti_cfg_type();
    const search_space_configuration&            ss_cfg       = *ss_info.cfg;

    // Fetch PDSCH resource grid allocator.
    cell_slot_resource_allocator& pdsch_alloc = get_res_alloc(grant.cell_index)[pdsch_td_cfg.k0];

    // Verify only one PDSCH exists for an RNTI.
    // See TS 38.214, clause 5.1, "For any HARQ process ID(s) in a given scheduled cell, the UE is not
    // expected to receive a PDSCH that overlaps in time with another PDSCH".
    if (ue_cc->last_pdsch_allocated_slot.valid() and pdsch_alloc.slot <= ue_cc->last_pdsch_allocated_slot) {
      // Try next candidate.
      continue;
    }

    // Check if there is space in PDSCH resource grid.
    const bool is_pdsch_full =
        std::find(slots_with_no_pdsch_space.begin(), slots_with_no_pdsch_space.end(), pdsch_alloc.slot) !=
        slots_with_no_pdsch_space.end();
    if (is_pdsch_full) {
      // Try next candidate.
      continue;
    }

    if (pdsch_alloc.result.dl.bc.sibs.size() + pdsch_alloc.result.dl.paging_grants.size() +
            pdsch_alloc.result.dl.rar_grants.size() + pdsch_alloc.result.dl.ue_grants.size() >=
        expert_cfg.max_pdschs_per_slot) {
      logger.debug("ue={} rnti={}: Failed to allocate PDSCH. Cause: Max number of PDSCHs per slot {} was reached.",
                   u.ue_index,
                   u.crnti,
                   expert_cfg.max_pdschs_per_slot);
      return {alloc_status::skip_slot};
    }

    // Verify there is space in PDSCH and PDCCH result lists for new allocations.
    if (pdsch_alloc.result.dl.ue_grants.full() or pdcch_alloc.result.dl.dl_pdcchs.full()) {
      logger.debug("ue={} rnti={}: Failed to allocate PDSCH. Cause: No space available in scheduler output list",
                   u.ue_index,
                   u.crnti);
      return {alloc_status::skip_slot};
    }

    if (not cell_cfg.is_dl_enabled(pdsch_alloc.slot)) {
      logger.debug("ue={} rnti={}: Failed to allocate PDSCH in slot={}. Cause: DL is not active in the PDSCH slot",
                   u.ue_index,
                   u.crnti,
                   pdsch_alloc.slot);
      return {alloc_status::skip_slot};
    }

    // Apply RB allocation limits.
    const unsigned start_rb = std::max(expert_cfg.pdsch_crb_limits.start(), ss_info.dl_crb_lims.start());
    const unsigned end_rb   = std::min(expert_cfg.pdsch_crb_limits.stop(), ss_info.dl_crb_lims.stop());
    if (start_rb >= end_rb) {
      // Invalid RB allocation range.
      return {alloc_status::skip_slot};
    }

    const crb_interval dl_crb_lims = {start_rb, end_rb};
    const crb_bitmap   used_crbs =
        pdsch_alloc.dl_res_grid.used_crbs(bwp_dl_cmn.generic_params.scs, dl_crb_lims, pdsch_td_cfg.symbols);
    if (used_crbs.all()) {
      slots_with_no_pdsch_space.push_back(pdsch_alloc.slot);
      return {alloc_status::skip_slot};
    }

    grant_prbs_mcs mcs_prbs =
        is_retx ? grant_prbs_mcs{h_dl.last_alloc_params().tb.front().value().mcs,
                                 h_dl.last_alloc_params().rbs.type1().length()}
                : ue_cc->required_dl_prbs(pdsch_td_cfg, grant.recommended_nof_bytes.value(), dci_type);
    // Try to limit the grant PRBs.
    if (not is_retx) {
      // Limit nof. RBs to allocate to maximum RBs provided in grant.
      if (grant.max_nof_rbs.has_value()) {
        mcs_prbs.n_prbs = std::min(mcs_prbs.n_prbs, grant.max_nof_rbs.value());
      }
      // [Implementation-defined] In case of partial slots and nof. PRBs allocated equals to 1 probability of KO is
      // high due to code not being able to cope with interference. So the solution is to increase the PRB allocation
      // to greater than 1 PRB.
      if (not cell_cfg.is_fully_dl_enabled(pdsch_alloc.slot) and mcs_prbs.n_prbs == 1) {
        mcs_prbs.n_prbs = 2;
      }
      // [Implementation-defined]
      // Check whether to allocate all remaining RBs or not. This is done to ensure we allocate only X nof. UEs per slot
      // and not X+1 nof. UEs. One way is by checking if the emtpy interval is less than 2 times the required RBs. If
      // so, allocate all remaining RBs. NOTE: This approach won't hold good in case of low traffic scenario.
      const unsigned twice_grant_crbs_length =
          rb_helper::find_empty_interval_of_length(used_crbs, mcs_prbs.n_prbs * 2, 0).length();
      if (twice_grant_crbs_length < (mcs_prbs.n_prbs * 2)) {
        mcs_prbs.n_prbs = twice_grant_crbs_length;
      }
    }

    if (mcs_prbs.n_prbs == 0) {
      logger.debug("ue={} rnti={} PDSCH allocation skipped. Cause: UE's CQI=0 ", u.ue_index, u.crnti);
      return {alloc_status::skip_ue};
    }

    crb_interval crbs = rb_helper::find_empty_interval_of_length(used_crbs, mcs_prbs.n_prbs, 0);
    if (crbs.empty()) {
      logger.debug("ue={} rnti={}: Failed to allocate PDSCH. Cause: Cause: No more RBs available at slot={}",
                   u.ue_index,
                   u.crnti,
                   pdsch_alloc.slot);
      return {alloc_status::skip_slot};
    }

    // In case of Retx, the #CRBs need to stay the same.
    if (is_retx and crbs.length() != h_dl.last_alloc_params().rbs.type1().length()) {
      logger.debug(
          "ue={} rnti={}: Failed to allocate PDSCH. Cause: No more RBs available at slot={} for h_id={} retransmission",
          u.ue_index,
          u.crnti,
          pdsch_alloc.slot,
          h_dl.id);
      return {alloc_status::skip_ue};
    }

    const aggregation_level aggr_lvl =
        ue_cc->get_aggregation_level(ue_cc->link_adaptation_controller().get_effective_cqi(), ss_info, true);
    // In case of retx, ensure the RI does not change.
    const unsigned nof_dl_layers =
        is_retx ? h_dl.last_alloc_params().nof_layers : ue_cc->channel_state_manager().get_nof_dl_layers();

    // Allocate PDCCH position.
    pdcch_dl_information* pdcch = get_pdcch_sched(grant.cell_index)
                                      .alloc_dl_pdcch_ue(pdcch_alloc, u.crnti, ue_cell_cfg, ss_cfg.get_id(), aggr_lvl);
    if (pdcch == nullptr) {
      logger.info("ue={} rnti={}: Failed to allocate PDSCH. Cause: No space in PDCCH.", u.ue_index, u.crnti);
      return {alloc_status::skip_ue};
    }

    // Allocate UCI. UCI destination (i.e., PUCCH or PUSCH) depends on whether there exist a PUSCH grant for the UE.
    unsigned                      k1      = 0;
    span<const uint8_t>           k1_list = ss_info.get_k1_candidates();
    std::optional<uci_allocation> uci =
        get_uci_alloc(grant.cell_index)
            .alloc_uci_harq_ue(
                get_res_alloc(grant.cell_index), u.crnti, u.get_pcell().cfg(), pdsch_td_cfg.k0, k1_list, nullptr);
    if (uci.has_value()) {
      k1                                      = uci.value().k1;
      pdcch->ctx.context.harq_feedback_timing = k1;
    } else {
      logger.debug("ue={} rnti={}: Failed to allocate PDSCH. Cause: UCI allocation failed.", u.ue_index, u.crnti);
      get_pdcch_sched(grant.cell_index).cancel_last_pdcch(pdcch_alloc);
      return {alloc_status::skip_ue};
    }

    // Fetch UL resource allocator.
    cell_slot_resource_allocator& ul_alloc = get_res_alloc(grant.cell_index)[pdsch_td_cfg.k0 + k1];

    // [Implementation-defined] Check whether max. PUCCHs per slot or max. UL grants per slot is reached if PDSCH
    // allocation for current UE succeeds. If so, allocate remaining RBs to the current UE only if it's a new Tx.
    // NOTE: At this point UCI is already allocated hence '>' is used rather than '>='.
    if (not is_retx and ((ul_alloc.result.ul.pucchs.size() > (expert_cfg.max_pucchs_per_slot - 1)) or
                         ((ul_alloc.result.ul.pucchs.size() + ul_alloc.result.ul.puschs.size()) >
                          (expert_cfg.max_ul_grants_per_slot - 1)))) {
      crbs = rb_helper::find_empty_interval_of_length(used_crbs, used_crbs.size(), 0);
    }

    // Verify there is no RB collision.
    if (pdsch_alloc.dl_res_grid.collides(bwp_dl_cmn.generic_params.scs, pdsch_td_cfg.symbols, crbs)) {
      logger.warning(
          "ue={} rnti={}: Failed to allocate PDSCH. Cause: No space available in scheduler RB resource grid.",
          u.ue_index,
          u.crnti);
      get_pdcch_sched(grant.cell_index).cancel_last_pdcch(pdcch_alloc);
      // TODO: Remove UCI allocated?
      return {alloc_status::invalid_params};
    }

    pdsch_config_params pdsch_cfg;
    switch (dci_type) {
      case dci_dl_rnti_config_type::c_rnti_f1_0:
        pdsch_cfg = get_pdsch_config_f1_0_c_rnti(cell_cfg, &ue_cell_cfg, pdsch_td_cfg);
        break;
      case dci_dl_rnti_config_type::c_rnti_f1_1:
        pdsch_cfg = get_pdsch_config_f1_1_c_rnti(ue_cell_cfg, pdsch_td_cfg, nof_dl_layers);
        break;
      default:
        report_fatal_error("Unsupported PDCCH DCI UL format");
    }

    // Reduce estimated MCS by 1 whenever CSI-RS is sent over a particular slot to account for the overhead of CSI-RS
    // REs.
    sch_mcs_index adjusted_mcs{mcs_prbs.mcs};
    if (not is_retx and not pdsch_alloc.result.dl.csi_rs.empty()) {
      // [Implementation-defined] The max MCS values below are set empirically and should avoid the effective code rate
      // to exceed 0.95 due to the overhead of CSI-RS REs.
      adjusted_mcs                = adjusted_mcs == 0 ? adjusted_mcs : adjusted_mcs - 1;
      uint8_t max_mcs_with_csi_rs = 28;
      if (pdsch_cfg.mcs_table == pdsch_mcs_table::qam64) {
        max_mcs_with_csi_rs = 26U;
      } else if (pdsch_cfg.mcs_table == pdsch_mcs_table::qam256) {
        max_mcs_with_csi_rs = 24U;
      }
      adjusted_mcs = static_cast<sch_mcs_index>(std::min(adjusted_mcs.to_uint(), max_mcs_with_csi_rs));
    }

    std::optional<sch_mcs_tbs> mcs_tbs_info;
    // If it's a new Tx, compute the MCS and TBS.
    if (not is_retx) {
      // As \c txDirectCurrentLocation, in \c SCS-SpecificCarrier, TS 38.331, "If this field (\c
      // txDirectCurrentLocation) is absent for downlink within ServingCellConfigCommon and ServingCellConfigCommonSIB,
      // the UE assumes the default value of 3300 (i.e. "Outside the carrier")".
      bool contains_dc = false;
      if (cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list.back().tx_direct_current_location.has_value()) {
        contains_dc = dc_offset_helper::is_contained(
            cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list.back().tx_direct_current_location.value(), crbs);
      }

      mcs_tbs_info = compute_dl_mcs_tbs(pdsch_cfg, adjusted_mcs, crbs.length(), contains_dc);
    } else {
      // It is a retx.
      mcs_tbs_info.emplace(
          sch_mcs_tbs{.mcs = h_dl.last_alloc_params().tb[0]->mcs, .tbs = h_dl.last_alloc_params().tb[0]->tbs_bytes});
    }

    // If there is not MCS-TBS info, it means no MCS exists such that the effective code rate is <= 0.95.
    if (not mcs_tbs_info.has_value()) {
      logger.warning(
          "ue={} rnti={}: Failed to allocate PDSCH. Cause: no MCS such that code rate <= 0.95.", u.ue_index, u.crnti);
      get_pdcch_sched(grant.cell_index).cancel_last_pdcch(pdcch_alloc);
      return {alloc_status::skip_ue};
    }

    // Mark resources as occupied in the ResourceGrid.
    pdsch_alloc.dl_res_grid.fill(grant_info{bwp_dl_cmn.generic_params.scs, pdsch_td_cfg.symbols, crbs});

    // Allocate UE DL HARQ.
    bool is_new_data = not is_retx;
    if (is_new_data) {
      // It is a new tx.
      h_dl.new_tx(pdsch_alloc.slot,
                  k1,
                  expert_cfg.max_nof_harq_retxs,
                  uci.value().harq_bit_idx,
                  ue_cc->channel_state_manager().get_wideband_cqi(),
                  nof_dl_layers);
    } else {
      // It is a retx.
      h_dl.new_retx(pdsch_alloc.slot, k1, uci.value().harq_bit_idx);
    }

    // Fill DL PDCCH DCI PDU.
    // Number of possible Downlink Assignment Indexes {0, ..., 3} as per TS38.213 Section 9.1.3.
    static constexpr unsigned DAI_MOD = 4U;
    uint8_t                   rv      = ue_cc->get_pdsch_rv(h_dl);
    // For allocation on PUSCH, we use a PUCCH resource indicator set to 0, as it will get ignored by the UE.
    const unsigned pucch_res_indicator =
        uci.value().pucch_res_indicator.has_value() ? uci.value().pucch_res_indicator.value() : 0U;
    switch (dci_type) {
      case dci_dl_rnti_config_type::c_rnti_f1_0:
        build_dci_f1_0_c_rnti(pdcch->dci,
                              ss_info,
                              cell_cfg.dl_cfg_common.init_dl_bwp,
                              crbs,
                              param_candidate.pdsch_td_res_index(),
                              k1,
                              pucch_res_indicator,
                              uci.value().harq_bit_idx % DAI_MOD,
                              mcs_tbs_info.value().mcs,
                              rv,
                              h_dl);
        break;
      case dci_dl_rnti_config_type::c_rnti_f1_1:
        build_dci_f1_1_c_rnti(pdcch->dci,
                              ue_cell_cfg,
                              ss_cfg.get_id(),
                              crb_to_prb(ss_info.dl_crb_lims, crbs),
                              param_candidate.pdsch_td_res_index(),
                              k1,
                              pucch_res_indicator,
                              uci.value().harq_bit_idx % DAI_MOD,
                              mcs_tbs_info.value().mcs,
                              rv,
                              h_dl,
                              nof_dl_layers);
        break;
      default:
        report_fatal_error("Unsupported RNTI type for PDSCH allocation");
    }

    // Fill PDSCH PDU.
    dl_msg_alloc& msg            = pdsch_alloc.result.dl.ue_grants.emplace_back();
    msg.context.ue_index         = u.ue_index;
    msg.context.k1               = k1;
    msg.context.ss_id            = ss_cfg.get_id();
    msg.context.nof_retxs        = h_dl.tb(0).nof_retxs;
    msg.context.buffer_occupancy = 0; // We fill this value later, after the TB is built.
    if (is_new_data and ue_cc->link_adaptation_controller().is_dl_olla_enabled()) {
      msg.context.olla_offset = ue_cc->link_adaptation_controller().dl_cqi_offset();
    }
    switch (pdcch->dci.type) {
      case dci_dl_rnti_config_type::c_rnti_f1_0:
        build_pdsch_f1_0_c_rnti(msg.pdsch_cfg,
                                pdsch_cfg,
                                mcs_tbs_info.value().tbs,
                                u.crnti,
                                cell_cfg,
                                ss_info,
                                pdcch->dci.c_rnti_f1_0,
                                crbs,
                                is_new_data);
        break;
      case dci_dl_rnti_config_type::c_rnti_f1_1:
        build_pdsch_f1_1_c_rnti(msg.pdsch_cfg,
                                pdsch_cfg,
                                mcs_tbs_info.value(),
                                u.crnti,
                                ue_cell_cfg,
                                ss_cfg.get_id(),
                                pdcch->dci.c_rnti_f1_1,
                                crbs,
                                h_dl,
                                ue_cc->channel_state_manager());
        break;
      default:
        report_fatal_error("Unsupported PDCCH DL DCI format");
    }

    // Save set PDCCH and PDSCH PDU parameters in HARQ process.
    dl_harq_sched_context pdsch_sched_ctx;
    pdsch_sched_ctx.dci_cfg_type = pdcch->dci.type;
    if (is_new_data) {
      pdsch_sched_ctx.olla_mcs =
          ue_cc->link_adaptation_controller().calculate_dl_mcs(msg.pdsch_cfg.codewords[0].mcs_table);
    }
    ue_cc->last_pdsch_allocated_slot = pdsch_alloc.slot;

    h_dl.save_alloc_params(pdsch_sched_ctx, msg.pdsch_cfg);

    if (is_new_data) {
      // Set MAC logical channels to schedule in this PDU if it is a newtx.
      u.build_dl_transport_block_info(msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes);

      // Update context with buffer occupancy after the TB is built.
      msg.context.buffer_occupancy = u.pending_dl_newtx_bytes();
    }

    return {alloc_status::success, h_dl.last_alloc_params().tb[0]->tbs_bytes};
  }

  // No candidates for PDSCH allocation.
  return {alloc_status::invalid_params};
}

alloc_result ue_cell_grid_allocator::allocate_ul_grant(const ue_pusch_grant& grant)
{
  srsran_assert(ues.contains(grant.user->ue_index), "Invalid UE candidate index={}", grant.user->ue_index);
  srsran_assert(has_cell(grant.cell_index), "Invalid UE candidate cell_index={}", grant.cell_index);
  constexpr static unsigned pdcch_delay_in_slots = 0;

  if (ul_attempts_count++ >= expert_cfg.max_pdcch_alloc_attempts_per_slot) {
    logger.debug("Stopping UL allocations. Cause: Max number of UL PDCCH allocation attempts {} reached.",
                 expert_cfg.max_pdcch_alloc_attempts_per_slot);
    return {alloc_status::skip_slot};
  }

  ue& u = ues[grant.user->ue_index];

  // Verify UE carrier is active.
  ue_cell* ue_cc = u.find_cell(grant.cell_index);
  if (ue_cc == nullptr) {
    logger.warning("PUSCH allocation failed. Cause: The ue={} carrier with cell_index={} is inactive",
                   u.ue_index,
                   grant.cell_index);
    return {alloc_status::skip_ue};
  }

  if (ue_cc->is_in_fallback_mode()) {
    // Skip allocation for UEs in fallback mode, as it is handled by the SRB fallback scheduler.
    return {alloc_status::skip_ue};
  }

  const ue_cell_configuration& ue_cell_cfg = ue_cc->cfg();
  const cell_configuration&    cell_cfg    = ue_cell_cfg.cell_cfg_common;
  ul_harq_process&             h_ul        = ue_cc->harqs.ul_harq(grant.h_id);
  const bool                   is_retx     = not h_ul.empty();

  // Fetch PDCCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = get_res_alloc(grant.cell_index)[pdcch_delay_in_slots];
  if (not cell_cfg.is_dl_enabled(pdcch_alloc.slot)) {
    logger.warning("ue={} rnti={}: Failed to allocate PUSCH. Cause: DL is not active in the PDCCH slot={}",
                   u.ue_index,
                   u.crnti,
                   pdcch_alloc.slot);
    return {alloc_status::skip_slot};
  }

  // Verify there is space in PDCCH result lists for new allocations.
  if (pdcch_alloc.result.dl.ul_pdcchs.full()) {
    logger.warning("ue={} rnti={}: Failed to allocate PUSCH. Cause: Maximum number of PDCCH grants per "
                   "slot {} reached",
                   u.ue_index,
                   u.crnti,
                   pdcch_alloc.result.dl.ul_pdcchs.capacity());
    return {alloc_status::skip_slot};
  }

  if (not ue_cc->is_active() and not is_retx) {
    // newTxs are not allowed for inactive UEs.
    logger.warning("ue={} rnti={}: Failed to allocate PUSCH. Cause: The ue={} carrier with cell_index={} is inactive",
                   u.ue_index,
                   u.crnti,
                   grant.cell_index);
    return {alloc_status::skip_ue};
  }

  if (not is_retx and not grant.recommended_nof_bytes.has_value()) {
    logger.warning("ue={} rnti={}: Failed to allocate PUSCH. Cause: Recommended nof. bytes to schedule is not given "
                   "for new Tx with h_id={}",
                   u.ue_index,
                   u.crnti,
                   grant.h_id);
    return {alloc_status::invalid_params};
  }

  // [Implementation-defined]
  // If there is large gap between two PUSCHs scheduled for a UE, \c last_pusch_allocated_slot could be having an old
  // slot value and the condition pusch_alloc.slot (e.g. 47.3) <= ue_cc->last_pusch_allocated_slot (e.g. 989.3) maybe be
  // true for long time and UE may not get scheduled even after receiving maximum nof. SR indication configured to UE
  // and eventually UE PRACHes.
  // This scenario can be prevented by resetting \c last_pusch_allocated_slot when its behind current PDCCH slot by
  // SCHEDULER_MAX_K2 number of slots.
  if (ue_cc->last_pusch_allocated_slot.valid() and
      std::abs(pdcch_alloc.slot - ue_cc->last_pusch_allocated_slot) > SCHEDULER_MAX_K2) {
    ue_cc->last_pusch_allocated_slot.clear();
  }

  // Create PUSCH param candidate search object.
  ue_pusch_alloc_param_candidate_searcher candidates{
      u, grant.cell_index, h_ul, pdcch_alloc.slot, slots_with_no_pusch_space};
  if (candidates.is_empty()) {
    // The conditions for a new PUSCH allocation for this UE were not met (e.g. lack of available SearchSpaces).
    return {alloc_status::skip_ue};
  }

  // Iterate through allocation parameter candidates.
  for (const ue_pusch_alloc_param_candidate_searcher::candidate& param_candidate : candidates) {
    const pusch_time_domain_resource_allocation& pusch_td_cfg = param_candidate.pusch_td_res();
    const search_space_info&                     ss_info      = param_candidate.ss();
    const dci_ul_rnti_config_type                dci_type     = param_candidate.dci_ul_rnti_cfg_type();
    const search_space_configuration&            ss_cfg       = *ss_info.cfg;
    const bwp_uplink_common&                     bwp_ul_cmn   = *ss_info.bwp->ul_common;
    const subcarrier_spacing                     scs          = bwp_ul_cmn.generic_params.scs;
    const unsigned                               final_k2     = pusch_td_cfg.k2 + cell_cfg.ntn_cs_koffset;

    // Fetch PUSCH resource grid allocators.
    cell_slot_resource_allocator& pusch_alloc = get_res_alloc(grant.cell_index)[pdcch_delay_in_slots + final_k2];

    // Verify only one PUSCH exists for an RNTI.
    // See TS 38.214, clause 6.1, "For any HARQ process ID(s) in a given scheduled cell, the UE is not expected to
    // transmit a PUSCH that overlaps in time with another PUSCH".
    // "For any two HARQ process IDs in a given scheduled cell, if the UE is scheduled to start a first PUSCH
    // transmission starting in symbol j by a PDCCH ending in symbol i, the UE is not expected to be scheduled to
    // transmit a PUSCH starting earlier than the end of the first PUSCH by a PDCCH that ends later than symbol i".
    if (ue_cc->last_pusch_allocated_slot.valid() and pusch_alloc.slot <= ue_cc->last_pusch_allocated_slot) {
      // Try next candidate.
      continue;
    }

    // Check if there is space in PUSCH resource grid.
    const bool is_pusch_full =
        std::find(slots_with_no_pusch_space.begin(), slots_with_no_pusch_space.end(), pusch_alloc.slot) !=
        slots_with_no_pusch_space.end();
    if (is_pusch_full) {
      // Try next candidate.
      continue;
    }

    if (not cell_cfg.is_ul_enabled(pusch_alloc.slot)) {
      logger.warning(
          "ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: UL is not active in the PUSCH slot (k2={})",
          u.ue_index,
          u.crnti,
          pusch_alloc.slot,
          final_k2);
      // Try next candidate.
      continue;
    }

    // When checking the number of remaining grants for PUSCH, take into account that the PUCCH grants for this UE will
    // be removed when multiplexing the UCI on PUSCH.
    unsigned pusch_pdu_rem_space = get_space_left_for_pusch_pdus(pusch_alloc.result, u.crnti, expert_cfg);
    if (pusch_pdu_rem_space == 0) {
      if (pusch_alloc.result.ul.puschs.size() >= expert_cfg.max_puschs_per_slot) {
        logger.info(
            "ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: Max number of PUSCHs per slot {} was reached.",
            u.ue_index,
            u.crnti,
            pusch_alloc.slot,
            expert_cfg.max_puschs_per_slot);
      } else {
        logger.info("ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: Max number of UL grants per slot {} "
                    "was reached.",
                    u.ue_index,
                    u.crnti,
                    pusch_alloc.slot,
                    expert_cfg.max_puschs_per_slot);
      }
      // Try next candidate.
      continue;
    }

    // [Implementation-defined] We skip allocation of PUSCH if there is already a PUCCH grant scheduled using common
    // PUCCH resources.
    if (get_uci_alloc(grant.cell_index).has_uci_harq_on_common_pucch_res(u.crnti, pusch_alloc.slot)) {
      logger.debug("ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: UE has PUCCH grant using common PUCCH "
                   "resources scheduled",
                   u.ue_index,
                   u.crnti,
                   pusch_alloc.slot);
      // Try next candidate.
      continue;
    }

    const unsigned start_ul_symbols =
        NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.get_nof_ul_symbol_per_slot(pusch_alloc.slot);
    // If it is a retx, we need to ensure we use a time_domain_resource with the same number of symbols as used for
    // the first transmission.
    const bool sym_length_match_prev_grant_for_retx =
        is_retx ? pusch_td_cfg.symbols.length() == h_ul.last_tx_params().nof_symbols : true;
    if (pusch_td_cfg.symbols.start() < start_ul_symbols or
        pusch_td_cfg.symbols.stop() > (start_ul_symbols + cell_cfg.get_nof_ul_symbol_per_slot(pusch_alloc.slot)) or
        !sym_length_match_prev_grant_for_retx) {
      // Try next candidate.
      continue;
    }

    // Apply RB allocation limits.
    const unsigned start_rb = std::max(expert_cfg.pusch_crb_limits.start(), ss_info.ul_crb_lims.start());
    const unsigned end_rb   = std::min(expert_cfg.pusch_crb_limits.stop(), ss_info.ul_crb_lims.stop());
    if (start_rb >= end_rb) {
      logger.debug("ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: Invalid RB allocation range [{}, {})",
                   u.ue_index,
                   u.crnti,
                   pusch_alloc.slot,
                   start_rb,
                   end_rb);
      // Try next candidate.
      continue;
    }

    const crb_interval ul_crb_lims = {start_rb, end_rb};
    const prb_bitmap   used_crbs   = pusch_alloc.ul_res_grid.used_crbs(scs, ul_crb_lims, pusch_td_cfg.symbols);
    if (used_crbs.all()) {
      slots_with_no_pusch_space.push_back(pusch_alloc.slot);
      // Try next candidate.
      continue;
    }

    // Compute the MCS and the number of PRBs, depending on the pending bytes to transmit.
    grant_prbs_mcs mcs_prbs =
        is_retx ? grant_prbs_mcs{h_ul.last_tx_params().mcs, h_ul.last_tx_params().rbs.type1().length()}
                : ue_cc->required_ul_prbs(pusch_td_cfg, grant.recommended_nof_bytes.value(), dci_type);
    // Try to limit the grant PRBs.
    if (not is_retx) {
      // Limit nof. RBs to allocate to maximum RBs provided in grant.
      if (grant.max_nof_rbs.has_value()) {
        mcs_prbs.n_prbs = std::min(mcs_prbs.n_prbs, grant.max_nof_rbs.value());
      }
      // [Implementation-defined] Check whether max. UL grants per slot is reached if PUSCH for current UE succeeds. If
      // so, allocate remaining RBs to the current UE only if it's a new Tx.
      if (pusch_pdu_rem_space == 1) {
        mcs_prbs.n_prbs = rb_helper::find_empty_interval_of_length(used_crbs, used_crbs.size(), 0).length();
      }
      // Due to the pre-allocated UCI bits, MCS 0 and PRB 1 would not leave any space for the payload on the TBS, as
      // all the space would be taken by the UCI bits. As a result of this, the effective code rate would be 0 and the
      // allocation would fail and be postponed to the next slot.
      // [Implementation-defined] In our tests, we have seen that MCS 5 with 1 PRB can lead (depending on the
      // configuration) to a non-valid MCS-PRB allocation; therefore, we set 6 as minimum value for 1 PRB.
      // TODO: Remove this part and handle the problem with a loop that is general for any configuration.
      const sch_mcs_index min_mcs_for_1_prb  = static_cast<sch_mcs_index>(6U);
      const unsigned      min_allocable_prbs = 1U;
      if (mcs_prbs.mcs < min_mcs_for_1_prb and mcs_prbs.n_prbs <= min_allocable_prbs) {
        ++mcs_prbs.n_prbs;
      }
      // [Implementation-defined]
      // Check whether to allocate all remaining RBs or not. This is done to ensure we allocate only X nof. UEs per slot
      // and not X+1 nof. UEs. One way is by checking if the emtpy interval is less than 2 times the required RBs. If
      // so, allocate all remaining RBs. NOTE: This approach won't hold good in case of low traffic scenario.
      const unsigned twice_grant_crbs_length =
          rb_helper::find_empty_interval_of_length(used_crbs, mcs_prbs.n_prbs * 2, 0).length();
      if (twice_grant_crbs_length < (mcs_prbs.n_prbs * 2)) {
        mcs_prbs.n_prbs = twice_grant_crbs_length;
      }
    }

    // NOTE: This should never happen, but it's safe not to proceed if we get n_prbs == 0.
    if (mcs_prbs.n_prbs == 0) {
      logger.debug(
          "ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: MCS and PRBs computation resulted in no PRBs "
          "allocated to this UE",
          u.ue_index,
          u.crnti,
          pusch_alloc.slot);
      return {alloc_status::skip_ue};
    }

    crb_interval crbs = rb_helper::find_empty_interval_of_length(used_crbs, mcs_prbs.n_prbs, 0);
    if (crbs.empty()) {
      logger.debug("ue={} rnti={}: Failed to allocate PUSCH. Cause: Cause: No more RBs available at slot={}",
                   u.ue_index,
                   u.crnti,
                   pusch_alloc.slot);
      // Try next candidate.
      continue;
    }

    // In case of Retx, the #CRBs need to stay the same.
    if (is_retx and crbs.length() != h_ul.last_tx_params().rbs.type1().length()) {
      logger.debug(
          "ue={} rnti={}: Failed to allocate PUSCH. Cause: No more RBs available at slot={} for h_id={} retransmission",
          u.ue_index,
          u.crnti,
          pusch_alloc.slot,
          h_ul.id);
      // Try next candidate.
      continue;
    }

    // Verify there is no RB collision.
    if (pusch_alloc.ul_res_grid.collides(scs, pusch_td_cfg.symbols, crbs)) {
      logger.warning("ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: Allocation collides with existing "
                     "PUSCH in RB grid [{}, {})",
                     u.ue_index,
                     u.crnti,
                     pusch_alloc.slot,
                     crbs.start(),
                     crbs.stop());
      // Try next candidate.
      continue;
    }

    const aggregation_level aggr_lvl =
        ue_cc->get_aggregation_level(ue_cc->link_adaptation_controller().get_effective_cqi(), ss_info, false);

    // Allocate PDCCH position.
    pdcch_ul_information* pdcch = get_pdcch_sched(grant.cell_index)
                                      .alloc_ul_pdcch_ue(pdcch_alloc, u.crnti, ue_cell_cfg, ss_cfg.get_id(), aggr_lvl);
    if (pdcch == nullptr) {
      logger.info("ue={} rnti={}: Failed to allocate PUSCH. Cause: No space in PDCCH.", u.ue_index, u.crnti);
      return {alloc_status::skip_ue};
    }

    const unsigned nof_harq_ack_bits =
        get_uci_alloc(grant.cell_index).get_scheduled_pdsch_counter_in_ue_uci(pusch_alloc, u.crnti);

    const bool is_csi_report_slot =
        csi_helper::is_csi_reporting_slot(u.get_pcell().cfg().cfg_dedicated(), pusch_alloc.slot);

    // Fetch PUSCH parameters based on type of transmission.
    pusch_config_params pusch_cfg;
    switch (dci_type) {
      case dci_ul_rnti_config_type::tc_rnti_f0_0:
        pusch_cfg = get_pusch_config_f0_0_tc_rnti(cell_cfg, pusch_td_cfg);
        break;
      case dci_ul_rnti_config_type::c_rnti_f0_0:
        pusch_cfg = get_pusch_config_f0_0_c_rnti(
            cell_cfg, &ue_cell_cfg, bwp_ul_cmn, pusch_td_cfg, nof_harq_ack_bits, is_csi_report_slot);
        break;
      case dci_ul_rnti_config_type::c_rnti_f0_1:
        pusch_cfg = get_pusch_config_f0_1_c_rnti(ue_cell_cfg,
                                                 pusch_td_cfg,
                                                 ue_cc->channel_state_manager().get_nof_ul_layers(),
                                                 nof_harq_ack_bits,
                                                 is_csi_report_slot);
        break;
      default:
        report_fatal_error("Unsupported PDCCH DCI UL format");
    }

    // Compute MCS and TBS for this transmission.
    std::optional<sch_mcs_tbs> mcs_tbs_info;
    // If it's a new Tx, compute the MCS and TBS from SNR, payload size, and available RBs.
    if (not is_retx) {
      bool contains_dc =
          dc_offset_helper::is_contained(cell_cfg.expert_cfg.ue.initial_ul_dc_offset, cell_cfg.nof_ul_prbs, crbs);

      mcs_tbs_info = compute_ul_mcs_tbs(pusch_cfg, &ue_cell_cfg, mcs_prbs.mcs, crbs.length(), contains_dc);
    }
    // If it's a reTx, fetch the MCS and TBS from the previous transmission.
    else {
      mcs_tbs_info.emplace(sch_mcs_tbs{.mcs = h_ul.last_tx_params().mcs, .tbs = h_ul.last_tx_params().tbs_bytes});
    }

    // If there is not MCS-TBS info, it means no MCS exists such that the effective code rate is <= 0.95.
    if (not mcs_tbs_info.has_value()) {
      logger.warning(
          "ue={} rnti={}: Failed to allocate PUSCH. Cause: no MCS such that code rate <= 0.95 with this "
          "configuration: mcs={} crbs={} symbols={} nof_oh={} tb-sc-field={} layers={} pi2bpsk={} "
          "harq_bits={} csi1_bits={} csi2_bits={} mcs_table_idx={} dmrs_A_pos={} is_dmrs_type2={} dmrs_add_pos_idx={}",
          u.ue_index,
          u.crnti,
          mcs_tbs_info.value().mcs.to_uint(),
          crbs,
          pusch_cfg.symbols,
          pusch_cfg.nof_oh_prb,
          pusch_cfg.tb_scaling_field,
          pusch_cfg.nof_layers,
          pusch_cfg.tp_pi2bpsk_present ? "yes" : "no",
          pusch_cfg.nof_harq_ack_bits,
          pusch_cfg.nof_csi_part1_bits,
          pusch_cfg.max_nof_csi_part2_bits,
          static_cast<unsigned>(pusch_cfg.mcs_table),
          ue_cell_cfg.cell_cfg_common.dmrs_typeA_pos == dmrs_typeA_position::pos2 ? "pos2" : "pos3",
          ue_cell_cfg.cfg_dedicated().ul_config->init_ul_bwp.pusch_cfg->pusch_mapping_type_a_dmrs.value().is_dmrs_type2
              ? "yes"
              : "no",
          static_cast<unsigned>(ue_cell_cfg.cfg_dedicated()
                                    .ul_config->init_ul_bwp.pusch_cfg->pusch_mapping_type_a_dmrs.value()
                                    .additional_positions));
      get_pdcch_sched(grant.cell_index).cancel_last_pdcch(pdcch_alloc);
      return {alloc_status::invalid_params};
    }

    // Mark resources as occupied in the ResourceGrid.
    pusch_alloc.ul_res_grid.fill(grant_info{scs, pusch_td_cfg.symbols, crbs});

    // Remove NTN offset when adding slot to HARQ process.
    slot_point harq_slot = pusch_alloc.slot - ue_cell_cfg.cell_cfg_common.ntn_cs_koffset;
    // Allocate UE UL HARQ.
    bool is_new_data = not is_retx;
    if (is_new_data) {
      // It is a new tx.
      h_ul.new_tx(harq_slot, expert_cfg.max_nof_harq_retxs);
    } else {
      // It is a retx.
      h_ul.new_retx(harq_slot);
    }

    // Compute total DAI. See TS 38.213, 9.1.3.2.
    // Total DAI provides total number of transmissions at the end of each interval (slot in a cell). Values range from
    // 1 to 4. If a UE is not provided PDSCH-CodeBlockGroupTransmission and the UE is scheduled for a PUSCH transmission
    // by DCI format 0_1 with DAI field value V_T_DAI_UL = 4 and the UE has not received any PDCCH within the monitoring
    // occasions for PDCCH with DCI format 1_0 or DCI format 1_1 for scheduling PDSCH receptions or SPS PDSCH
    // release on any serving cell c and the UE does not have HARQ-ACK information in response to a SPS PDSCH
    // reception to multiplex in the PUSCH, the UE does not multiplex HARQ-ACK information in the PUSCH transmission.
    // NOTE: DAI is encoded as per left most column in Table 9.1.3-2 of TS 38.213.
    unsigned dai = 3;
    if (dci_type == dci_ul_rnti_config_type::c_rnti_f0_1) {
      unsigned total_harq_ack_in_uci = nof_harq_ack_bits;
      if (total_harq_ack_in_uci != 0) {
        // See TS 38.213, Table 9.1.3-2. dai value below maps to the leftmost column in the table.
        dai = ((total_harq_ack_in_uci - 1) % 4);
      }
    }

    // Fill UL PDCCH DCI.
    uint8_t rv = ue_cc->get_pusch_rv(h_ul);
    switch (dci_type) {
      case dci_ul_rnti_config_type::tc_rnti_f0_0:
        build_dci_f0_0_tc_rnti(pdcch->dci,
                               *ue_cell_cfg.bwp(to_bwp_id(0)).dl_common,
                               ue_cell_cfg.bwp(ue_cc->active_bwp_id()).ul_common->generic_params,
                               crbs,
                               param_candidate.pusch_td_res_index(),
                               mcs_tbs_info.value().mcs,
                               rv,
                               h_ul);
        break;
      case dci_ul_rnti_config_type::c_rnti_f0_0:
        build_dci_f0_0_c_rnti(pdcch->dci,
                              ss_info,
                              cell_cfg.ul_cfg_common.init_ul_bwp,
                              crbs,
                              param_candidate.pusch_td_res_index(),
                              mcs_tbs_info.value().mcs,
                              rv,
                              h_ul);
        break;
      case dci_ul_rnti_config_type::c_rnti_f0_1:
        build_dci_f0_1_c_rnti(pdcch->dci,
                              ue_cell_cfg,
                              ss_cfg.get_id(),
                              crbs,
                              param_candidate.pusch_td_res_index(),
                              mcs_tbs_info.value().mcs,
                              rv,
                              h_ul,
                              dai,
                              ue_cc->channel_state_manager().get_nof_ul_layers());
        break;
      default:
        report_fatal_error("Unsupported PDCCH UL DCI format");
    }

    // Fill PUSCH.
    ul_sched_info& msg    = pusch_alloc.result.ul.puschs.emplace_back();
    msg.context.ue_index  = u.ue_index;
    msg.context.ss_id     = ss_cfg.get_id();
    msg.context.k2        = final_k2;
    msg.context.nof_retxs = h_ul.tb().nof_retxs;
    if (is_new_data and ue_cc->link_adaptation_controller().is_ul_olla_enabled()) {
      msg.context.olla_offset = ue_cc->link_adaptation_controller().ul_snr_offset_db();
    }
    switch (pdcch->dci.type) {
      case dci_ul_rnti_config_type::tc_rnti_f0_0:
        build_pusch_f0_0_tc_rnti(msg.pusch_cfg,
                                 pusch_cfg,
                                 mcs_tbs_info.value().tbs,
                                 u.crnti,
                                 cell_cfg,
                                 pdcch->dci.tc_rnti_f0_0,
                                 crbs,
                                 is_new_data);
        break;
      case dci_ul_rnti_config_type::c_rnti_f0_0:
        build_pusch_f0_0_c_rnti(msg.pusch_cfg,
                                u.crnti,
                                pusch_cfg,
                                mcs_tbs_info.value().tbs,
                                cell_cfg,
                                bwp_ul_cmn,
                                pdcch->dci.c_rnti_f0_0,
                                crbs,
                                is_new_data);
        break;
      case dci_ul_rnti_config_type::c_rnti_f0_1:
        build_pusch_f0_1_c_rnti(msg.pusch_cfg,
                                u.crnti,
                                pusch_cfg,
                                mcs_tbs_info.value(),
                                ue_cell_cfg,
                                ss_cfg.get_id(),
                                pdcch->dci.c_rnti_f0_1,
                                crbs,
                                h_ul);
        break;
      default:
        report_fatal_error("Unsupported PDCCH UL DCI format");
    }

    // Check if there is any UCI grant allocated on the PUCCH that can be moved to the PUSCH.
    get_uci_alloc(grant.cell_index).multiplex_uci_on_pusch(msg, pusch_alloc, ue_cell_cfg, u.crnti);

    // Save set PDCCH and PUSCH PDU parameters in HARQ process.
    ul_harq_sched_context pusch_sched_ctx;
    pusch_sched_ctx.dci_cfg_type = pdcch->dci.type;
    if (is_new_data) {
      pusch_sched_ctx.olla_mcs = ue_cc->link_adaptation_controller().calculate_ul_mcs(msg.pusch_cfg.mcs_table);
    }
    ue_cc->last_pusch_allocated_slot = pusch_alloc.slot;

    h_ul.save_alloc_params(pusch_sched_ctx, msg.pusch_cfg);

    // In case there is a SR pending. Reset it.
    u.reset_sr_indication();

    return {alloc_status::success, h_ul.last_tx_params().tbs_bytes};
  }

  // No candidates for PUSCH allocation.
  return {alloc_status::invalid_params};
}
