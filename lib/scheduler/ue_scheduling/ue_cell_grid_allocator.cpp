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
#include "../support/mcs_tbs_calculator.h"
#include "../support/sch_pdu_builder.h"
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

void ue_cell_grid_allocator::slot_indication()
{
  dl_attempts_count = 0;
  ul_attempts_count = 0;
}

alloc_outcome ue_cell_grid_allocator::allocate_dl_grant(const ue_pdsch_grant& grant)
{
  srsran_assert(ues.contains(grant.user->ue_index), "Invalid UE candidate index={}", grant.user->ue_index);
  srsran_assert(has_cell(grant.cell_index), "Invalid UE candidate cell_index={}", grant.cell_index);

  if (dl_attempts_count++ >= expert_cfg.max_pdcch_alloc_attempts_per_slot) {
    logger.debug("Stopping DL allocations. Cause: Max number of DL PDCCH allocation attempts {} reached.",
                 expert_cfg.max_pdcch_alloc_attempts_per_slot);
    return alloc_outcome::skip_slot;
  }

  ue& u = ues[grant.user->ue_index];

  // Verify UE carrier is active.
  ue_cell* ue_cc = u.find_cell(grant.cell_index);
  if (ue_cc == nullptr) {
    logger.warning("PDSCH allocation failed. Cause: The ue={} carrier with cell_index={} is inactive",
                   u.ue_index,
                   grant.cell_index);
    return alloc_outcome::skip_ue;
  }

  const ue_cell_configuration& ue_cell_cfg = ue_cc->cfg();
  const cell_configuration&    cell_cfg    = ue_cell_cfg.cell_cfg_common;
  const bwp_downlink_common&   init_dl_bwp = *ue_cell_cfg.bwp(to_bwp_id(0)).dl_common;
  const bwp_downlink_common&   bwp_dl_cmn  = *ue_cell_cfg.bwp(ue_cc->active_bwp_id()).dl_common;
  dl_harq_process&             h_dl        = ue_cc->harqs.dl_harq(grant.h_id);

  if (not ue_cc->is_active() and h_dl.empty()) {
    // newTxs are not allowed for inactive UEs.
    logger.warning("PDSCH allocation failed. Cause: The ue={} carrier with cell_index={} is inactive",
                   u.ue_index,
                   grant.cell_index);
    return alloc_outcome::skip_ue;
  }

  // Find a SearchSpace candidate.
  const search_space_info* ss_info = ue_cell_cfg.find_search_space(grant.ss_id);
  if (ss_info == nullptr) {
    logger.warning("Failed to allocate PDSCH. Cause: No valid SearchSpace found.");
    return alloc_outcome::invalid_params;
  }
  if (ss_info->bwp->bwp_id != ue_cc->active_bwp_id()) {
    logger.warning("Failed to allocate PDSCH. Cause: SearchSpace not valid for active BWP.");
    return alloc_outcome::invalid_params;
  }
  const search_space_configuration& ss_cfg = *ss_info->cfg;
  const coreset_configuration&      cs_cfg = *ss_info->coreset;

  dci_dl_rnti_config_type dci_type;
  if (not h_dl.empty()) {
    dci_type = h_dl.last_alloc_params().dci_cfg_type;
  } else if (u.is_conres_ce_pending()) {
    dci_type = dci_dl_rnti_config_type::tc_rnti_f1_0;
  } else {
    dci_type = ss_info->get_dl_dci_format() == dci_dl_format::f1_0 ? dci_dl_rnti_config_type::c_rnti_f1_0
                                                                   : dci_dl_rnti_config_type::c_rnti_f1_1;
  }

  // See 3GPP TS 38.213, clause 10.1,
  // A UE monitors PDCCH candidates in one or more of the following search spaces sets
  //  - a Type1-PDCCH CSS set configured by ra-SearchSpace in PDCCH-ConfigCommon for a DCI format with
  //    CRC scrambled by a RA-RNTI, a MsgB-RNTI, or a TC-RNTI on the primary cell.
  if (dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0 and
      grant.ss_id != cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id) {
    logger.info("Failed to allocate PDSCH. Cause: SearchSpace not valid for re-transmission of Msg4.");
    return alloc_outcome::invalid_params;
  }

  // Note: Unable to multiplex CSI and SRB0 retransmission.
  if (dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0 and
      not get_res_alloc(grant.cell_index)[0].result.dl.csi_rs.empty()) {
    logger.info("Failed to allocate PDSCH. Cause: Multiplexing of CSI-RS and TC-RNTI retransmission is not allowed.");
    return alloc_outcome::skip_ue;
  }

  const span<const pdsch_time_domain_resource_allocation> pdsch_list   = ss_info->pdsch_time_domain_list;
  const pdsch_time_domain_resource_allocation&            pdsch_td_cfg = pdsch_list[grant.time_res_index];

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = get_res_alloc(grant.cell_index)[0];
  cell_slot_resource_allocator& pdsch_alloc = get_res_alloc(grant.cell_index)[pdsch_td_cfg.k0];

  if (pdsch_alloc.result.dl.bc.sibs.size() + pdsch_alloc.result.dl.paging_grants.size() +
          pdsch_alloc.result.dl.rar_grants.size() + pdsch_alloc.result.dl.ue_grants.size() >=
      expert_cfg.max_pdschs_per_slot) {
    logger.info("Failed to allocate PDSCH. Cause: Max number of PDSCHs per slot {} was reached.",
                expert_cfg.max_pdschs_per_slot);
    return alloc_outcome::skip_slot;
  }
  if (not cell_cfg.is_dl_enabled(pdcch_alloc.slot)) {
    logger.warning("Failed to allocate PDSCH in slot={}. Cause: DL is not active in the PDCCH slot", pdsch_alloc.slot);
    return alloc_outcome::skip_slot;
  }
  if (not cell_cfg.is_dl_enabled(pdsch_alloc.slot)) {
    logger.warning("Failed to allocate PDSCH in slot={}. Cause: DL is not active in the PDSCH slot", pdsch_alloc.slot);
    return alloc_outcome::invalid_params;
  }
  // Check whether PDSCH time domain resource does not overlap with CORESET.
  if (pdsch_td_cfg.symbols.start() < ss_cfg.get_first_symbol_index() + cs_cfg.duration) {
    logger.warning("Failed to allocate PDSCH in slot={}. Cause: PDSCH time domain resource ={} is starting before the "
                   "CORESET symbols",
                   pdsch_alloc.slot,
                   grant.time_res_index);
    return alloc_outcome::invalid_params;
  }
  // Check whether PDSCH time domain resource fits in DL symbols of the slot.
  if (pdsch_td_cfg.symbols.stop() > cell_cfg.get_nof_dl_symbol_per_slot(pdsch_alloc.slot)) {
    logger.warning("Failed to allocate PDSCH in slot={}. Cause: Nof. DL symbols in PDSCH slot shorter than PDSCH time "
                   "domain resource ={}",
                   pdsch_alloc.slot,
                   grant.time_res_index);
    return alloc_outcome::invalid_params;
  }
  // Verify there is space in PDSCH and PDCCH result lists for new allocations.
  if (pdsch_alloc.result.dl.ue_grants.full() or pdcch_alloc.result.dl.dl_pdcchs.full()) {
    logger.info("ue={} rnti={}: Failed to allocate PDSCH. Cause: No space available in scheduler output list",
                u.ue_index,
                u.crnti);
    return alloc_outcome::skip_slot;
  }

  // Verify CRBs fit in the chosen BWP.
  if (not ss_info->dl_crb_lims.contains(grant.crbs)) {
    logger.warning("ue={} rnti={}: Failed to allocate PDSCH. Cause: CRBs={} are outside the valid limits={}.",
                   u.ue_index,
                   u.crnti,
                   grant.crbs,
                   ss_info->dl_crb_lims);
    return alloc_outcome::invalid_params;
  }

  // In case of retx, ensure the number of PRBs for the grant did not change.
  if (not h_dl.empty() and grant.crbs.length() != h_dl.last_alloc_params().rbs.type1().length()) {
    logger.warning("ue={} rnti={}: Failed to allocate PDSCH. Cause: Number of CRBs has to remain constant during "
                   "retxs (Harq-id={}, nof_prbs expected={} != actual={})",
                   u.ue_index,
                   u.crnti,
                   h_dl.id,
                   h_dl.last_alloc_params().rbs.type1().length(),
                   grant.crbs.length());
    return alloc_outcome::invalid_params;
  }

  // Verify there is no RB collision.
  if (pdsch_alloc.dl_res_grid.collides(bwp_dl_cmn.generic_params.scs, pdsch_td_cfg.symbols, grant.crbs)) {
    logger.warning("ue={} rnti={}: Failed to allocate PDSCH. Cause: No space available in scheduler RB resource grid.",
                   u.ue_index,
                   u.crnti);
    return alloc_outcome::invalid_params;
  }

  // Allocate PDCCH position.
  pdcch_dl_information* pdcch = nullptr;
  if (ue_cc->is_in_fallback_mode() or dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0) {
    pdcch =
        get_pdcch_sched(grant.cell_index).alloc_dl_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.get_id(), grant.aggr_lvl);
  } else {
    pdcch = get_pdcch_sched(grant.cell_index)
                .alloc_dl_pdcch_ue(pdcch_alloc, u.crnti, ue_cell_cfg, ss_cfg.get_id(), grant.aggr_lvl);
  }
  if (pdcch == nullptr) {
    logger.info("ue={} rnti={}: Failed to allocate PDSCH. Cause: No space in PDCCH.", u.ue_index, u.crnti);
    // TODO: For now, given that only one searchSpace is used, we skip the UE. In the future, we might still try
    // different searchSpaces.
    return alloc_outcome::skip_ue;
  }

  // Allocate UCI. UCI destination (i.e., PUCCH or PUSCH) depends on whether there exist a PUSCH grant for the UE.
  unsigned                 k1      = 0;
  span<const uint8_t>      k1_list = ss_info->get_k1_candidates();
  optional<uci_allocation> uci     = get_uci_alloc(grant.cell_index)
                                     .alloc_uci_harq_ue(get_res_alloc(grant.cell_index),
                                                        u.crnti,
                                                        u.get_pcell().cfg(),
                                                        pdsch_td_cfg.k0,
                                                        k1_list,
                                                        ue_cc->is_in_fallback_mode() ? pdcch : nullptr);
  if (uci.has_value()) {
    k1                                      = uci.value().k1;
    pdcch->ctx.context.harq_feedback_timing = k1;
  } else {
    logger.info("ue={} rnti={}: Failed to allocate PDSCH. Cause: No space in PUCCH.", u.ue_index, u.crnti);
    get_pdcch_sched(grant.cell_index).cancel_last_pdcch(pdcch_alloc);
    // TODO: For now, given that only one searchSpace is used, we skip the UE. In the future, we might still try
    // different searchSpaces.
    return alloc_outcome::skip_ue;
  }

  pdsch_config_params pdsch_cfg;
  switch (dci_type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0:
      pdsch_cfg = get_pdsch_config_f1_0_tc_rnti(cell_cfg, pdsch_list[grant.time_res_index]);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      pdsch_cfg = get_pdsch_config_f1_0_c_rnti(ue_cell_cfg, pdsch_list[grant.time_res_index]);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_1:
      pdsch_cfg = get_pdsch_config_f1_1_c_rnti(ue_cell_cfg, pdsch_list[grant.time_res_index], grant.nof_layers);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI UL format");
  }

  // Reduce estimated MCS by 1 whenever CSI-RS is sent over a particular slot to account for the overhead of CSI-RS REs.
  sch_mcs_index adjusted_mcs{grant.mcs};
  if (not pdsch_alloc.result.dl.csi_rs.empty()) {
    // [Implementation-defined] The max MCS values below are set empirically and should avoid the effective code rate to
    // exceed 0.95 due to the overhead of CSI-RS REs.
    adjusted_mcs                = adjusted_mcs == 0 ? adjusted_mcs : adjusted_mcs - 1;
    uint8_t max_mcs_with_csi_rs = 28;
    if (pdsch_cfg.mcs_table == pdsch_mcs_table::qam64) {
      max_mcs_with_csi_rs = 26U;
    } else if (pdsch_cfg.mcs_table == pdsch_mcs_table::qam256) {
      max_mcs_with_csi_rs = 24U;
    }
    adjusted_mcs = static_cast<sch_mcs_index>(std::min(adjusted_mcs.to_uint(), max_mcs_with_csi_rs));
  }

  optional<sch_mcs_tbs> mcs_tbs_info;
  // If it's a new Tx, compute the MCS and TBS.
  if (h_dl.empty()) {
    mcs_tbs_info = compute_dl_mcs_tbs(pdsch_cfg, ue_cell_cfg, adjusted_mcs, grant.crbs.length());
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
    return alloc_outcome::invalid_params;
  }

  // In case of retx, ensure the TBS does not change.
  if (not h_dl.empty() and mcs_tbs_info->tbs != h_dl.last_alloc_params().tb[0]->tbs_bytes) {
    logger.warning("ue={} rnti={}: Failed to allocate PDSCH. Cause: TBS has to remain constant during retxs "
                   "(Harq-id={}, tbs expected={} != actual={})",
                   u.ue_index,
                   u.crnti,
                   h_dl.id,
                   h_dl.last_alloc_params().tb[0]->tbs_bytes,
                   mcs_tbs_info->tbs);
    return alloc_outcome::invalid_params;
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{bwp_dl_cmn.generic_params.scs, pdsch_td_cfg.symbols, grant.crbs});

  // Allocate UE DL HARQ.
  if (h_dl.empty()) {
    // It is a new tx.
    h_dl.new_tx(pdsch_alloc.slot,
                k1,
                expert_cfg.max_nof_harq_retxs,
                uci.value().harq_bit_idx,
                ue_cc->channel_state_manager().get_wideband_cqi(),
                grant.nof_layers);
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
    case dci_dl_rnti_config_type::tc_rnti_f1_0:
      build_dci_f1_0_tc_rnti(pdcch->dci,
                             init_dl_bwp,
                             grant.crbs,
                             grant.time_res_index,
                             k1,
                             pucch_res_indicator,
                             mcs_tbs_info.value().mcs,
                             rv,
                             h_dl);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      build_dci_f1_0_c_rnti(pdcch->dci,
                            ue_cell_cfg,
                            grant.ss_id,
                            grant.crbs,
                            grant.time_res_index,
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
                            grant.ss_id,
                            crb_to_prb(ss_info->dl_crb_lims, grant.crbs),
                            grant.time_res_index,
                            k1,
                            pucch_res_indicator,
                            uci.value().harq_bit_idx % DAI_MOD,
                            mcs_tbs_info.value().mcs,
                            rv,
                            h_dl,
                            grant.nof_layers);
      break;
    default:
      report_fatal_error("Unsupported RNTI type for PDSCH allocation");
  }

  // Fill PDSCH PDU.
  dl_msg_alloc& msg     = pdsch_alloc.result.dl.ue_grants.emplace_back();
  msg.context.ue_index  = u.ue_index;
  msg.context.k1        = k1;
  msg.context.ss_id     = ss_cfg.get_id();
  msg.context.nof_retxs = h_dl.tb(0).nof_retxs;
  if (msg.context.nof_retxs == 0 and ue_cc->link_adaptation_controller().is_dl_olla_enabled()) {
    msg.context.olla_offset = ue_cc->link_adaptation_controller().dl_cqi_offset();
  }
  switch (pdcch->dci.type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0:
      build_pdsch_f1_0_tc_rnti(msg.pdsch_cfg,
                               pdsch_cfg,
                               mcs_tbs_info.value().tbs,
                               u.crnti,
                               cell_cfg,
                               pdcch->dci.tc_rnti_f1_0,
                               grant.crbs,
                               h_dl.tb(0).nof_retxs == 0);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      build_pdsch_f1_0_c_rnti(msg.pdsch_cfg,
                              pdsch_cfg,
                              mcs_tbs_info.value().tbs,
                              u.crnti,
                              ue_cell_cfg,
                              grant.ss_id,
                              pdcch->dci.c_rnti_f1_0,
                              grant.crbs,
                              h_dl.tb(0).nof_retxs == 0);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_1:
      build_pdsch_f1_1_c_rnti(msg.pdsch_cfg,
                              pdsch_cfg,
                              mcs_tbs_info.value(),
                              u.crnti,
                              ue_cell_cfg,
                              grant.ss_id,
                              pdcch->dci.c_rnti_f1_1,
                              grant.crbs,
                              h_dl,
                              ue_cc->channel_state_manager());
      break;
    default:
      report_fatal_error("Unsupported PDCCH DL DCI format");
  }

  // Save set PDCCH and PDSCH PDU parameters in HARQ process.
  h_dl.save_alloc_params(pdcch->dci.type, msg.pdsch_cfg);

  if (h_dl.tb(0).nof_retxs == 0) {
    // Set MAC logical channels to schedule in this PDU if it is a newtx.
    u.build_dl_transport_block_info(msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes);
  }

  return alloc_outcome::success;
}

alloc_outcome ue_cell_grid_allocator::allocate_ul_grant(const ue_pusch_grant& grant)
{
  srsran_assert(ues.contains(grant.user->ue_index), "Invalid UE candidate index={}", grant.user->ue_index);
  srsran_assert(has_cell(grant.cell_index), "Invalid UE candidate cell_index={}", grant.cell_index);
  constexpr static unsigned pdcch_delay_in_slots = 0;

  if (ul_attempts_count++ >= expert_cfg.max_pdcch_alloc_attempts_per_slot) {
    logger.debug("Stopping UL allocations. Cause: Max number of UL PDCCH allocation attempts {} reached.",
                 expert_cfg.max_pdcch_alloc_attempts_per_slot);
    return alloc_outcome::skip_slot;
  }

  ue& u = ues[grant.user->ue_index];

  // Verify UE carrier is active.
  ue_cell* ue_cc = u.find_cell(grant.cell_index);
  if (ue_cc == nullptr) {
    logger.warning("PUSCH allocation failed. Cause: The ue={} carrier with cell_index={} is inactive",
                   u.ue_index,
                   grant.cell_index);
    return alloc_outcome::skip_ue;
  }

  const ue_cell_configuration& ue_cell_cfg = ue_cc->cfg();
  const cell_configuration&    cell_cfg    = ue_cell_cfg.cell_cfg_common;
  ul_harq_process&             h_ul        = ue_cc->harqs.ul_harq(grant.h_id);

  if (not ue_cc->is_active() and h_ul.empty()) {
    // newTxs are not allowed for inactive UEs.
    logger.warning("PUSCH allocation failed. Cause: The ue={} carrier with cell_index={} is inactive",
                   u.ue_index,
                   grant.cell_index);
    return alloc_outcome::skip_ue;
  }

  // Find a SearchSpace candidate.
  const search_space_info* ss_info = ue_cell_cfg.find_search_space(grant.ss_id);
  if (ss_info == nullptr) {
    logger.warning("Failed to allocate PUSCH. Cause: No valid SearchSpace found.");
    return alloc_outcome::invalid_params;
  }
  if (ss_info->bwp->bwp_id != ue_cc->active_bwp_id()) {
    logger.warning("Failed to allocate PUSCH. Cause: Chosen SearchSpace {} does not belong to the active BWP {}",
                   grant.ss_id,
                   ue_cc->active_bwp_id());
    return alloc_outcome::invalid_params;
  }
  const search_space_configuration&            ss_cfg       = *ss_info->cfg;
  const bwp_uplink_common&                     bwp_ul_cmn   = *ss_info->bwp->ul_common;
  const subcarrier_spacing                     scs          = bwp_ul_cmn.generic_params.scs;
  const pusch_time_domain_resource_allocation& pusch_td_cfg = ss_info->pusch_time_domain_list[grant.time_res_index];
  const dci_ul_rnti_config_type ss_supported_crnti_dci_type = ss_info->get_ul_dci_format() == dci_ul_format::f0_0
                                                                  ? dci_ul_rnti_config_type::c_rnti_f0_0
                                                                  : dci_ul_rnti_config_type::c_rnti_f0_1;
  const dci_ul_rnti_config_type dci_type =
      h_ul.empty() ? ss_supported_crnti_dci_type : h_ul.last_tx_params().dci_cfg_type;

  // See 3GPP TS 38.213, clause 10.1,
  // A UE monitors PDCCH candidates in one or more of the following search spaces sets
  //  - a Type1-PDCCH CSS set configured by ra-SearchSpace in PDCCH-ConfigCommon for a DCI format with
  //    CRC scrambled by a RA-RNTI, a MsgB-RNTI, or a TC-RNTI on the primary cell.
  if (dci_type == dci_ul_rnti_config_type::tc_rnti_f0_0 and
      grant.ss_id != cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id) {
    logger.info("Failed to allocate PUSCH. Cause: SearchSpace not valid for re-transmission.");
    return alloc_outcome::invalid_params;
  }

  // In case of re-transmission DCI format must remain same and therefore its necessary to find the SS which support
  // that DCI format.
  if (dci_type != dci_ul_rnti_config_type::tc_rnti_f0_0 and dci_type != ss_supported_crnti_dci_type) {
    logger.info(
        "rnti={} Failed to allocate PUSCH. Cause: DCI format {} in HARQ retx is not supported in SearchSpace {}.",
        u.crnti,
        dci_ul_rnti_config_format(h_ul.last_tx_params().dci_cfg_type),
        grant.ss_id);
    return alloc_outcome::invalid_params;
  }

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = get_res_alloc(grant.cell_index)[pdcch_delay_in_slots];
  cell_slot_resource_allocator& pusch_alloc =
      get_res_alloc(grant.cell_index)[pdcch_delay_in_slots + pusch_td_cfg.k2 + cell_cfg.ntn_cs_koffset];

  if (not cell_cfg.is_dl_enabled(pdcch_alloc.slot)) {
    logger.warning("rnti={} Failed to allocate PUSCH in slot={}. Cause: DL is not active in the PDCCH slot",
                   u.crnti,
                   pusch_alloc.slot);
    return alloc_outcome::skip_slot;
  }
  if (not cell_cfg.is_ul_enabled(pusch_alloc.slot)) {
    logger.warning("rnti={} Failed to allocate PUSCH in slot={}. Cause: UL is not active in the PUSCH slot (k2={})",
                   u.crnti,
                   pusch_alloc.slot,
                   pusch_td_cfg.k2);
    return alloc_outcome::invalid_params;
  }
  if (pusch_alloc.result.ul.puschs.size() >= expert_cfg.max_puschs_per_slot) {
    logger.info("Failed to allocate PUSCH. Cause: Max number of PUSCHs per slot {} was reached.",
                expert_cfg.max_puschs_per_slot);
    return alloc_outcome::skip_slot;
  }
  if (pusch_alloc.result.ul.puschs.size() >=
      expert_cfg.max_ul_grants_per_slot - static_cast<unsigned>(pusch_alloc.result.ul.pucchs.size())) {
    logger.info("Failed to allocate PUSCH. Cause: Max number of UL grants per slot {} was reached.",
                expert_cfg.max_puschs_per_slot);
    return alloc_outcome::skip_slot;
  }

  // Verify there is space in PUSCH and PDCCH result lists for new allocations.
  if (pusch_alloc.result.ul.puschs.full() or pdcch_alloc.result.dl.ul_pdcchs.full()) {
    logger.warning("rnti={} Failed to allocate PUSCH in slot={}. Cause: No space available in scheduler output list",
                   u.crnti,
                   pusch_alloc.slot);
    return alloc_outcome::skip_slot;
  }

  // [Implementation-defined] We skip allocation of PUSCH if there is already a PUCCH grant scheduled over the same slot
  // and the UE is in fallback mode.
  // NOTE: This is due to the lack of clarity of the TS when it comes to define what \c betaOffsets to use for PUSCH
  // when the UE does not have a dedicated configuration.
  if (ue_cc->is_in_fallback_mode()) {
    const auto* pucch_grant_it =
        std::find_if(pusch_alloc.result.ul.pucchs.begin(),
                     pusch_alloc.result.ul.pucchs.end(),
                     [&u](const pucch_info& pucch_grant) { return pucch_grant.crnti == u.crnti; });
    if (pucch_grant_it != pusch_alloc.result.ul.pucchs.end()) {
      logger.debug("rnti={} Allocation of PUSCH in slot={} skipped. Cause: this UE is in fallback mode and has "
                   "PUCCH grants scheduled",
                   u.crnti,
                   pusch_alloc.slot);
      return alloc_outcome::skip_ue;
    }
  }

  // Verify CRBs allocation.
  if (not ss_info->ul_crb_lims.contains(grant.crbs)) {
    logger.warning("rnti={} Failed to allocate PUSCH. Cause: CRBs {} allocated outside the BWP {}",
                   u.crnti,
                   grant.crbs,
                   ss_info->ul_crb_lims);
    return alloc_outcome::invalid_params;
  }

  // In case of retx, ensure the number of PRBs for the grant did not change.
  if (not h_ul.empty() and grant.crbs.length() != h_ul.last_tx_params().rbs.type1().length()) {
    logger.warning(
        "rnti={} Failed to allocate PUSCH. Cause: Number of CRBs has to remain constant during retxs (harq-id={}, "
        "nof_prbs={}!={})",
        u.crnti,
        h_ul.id,
        h_ul.last_tx_params().rbs.type1().length(),
        grant.crbs.length());
    return alloc_outcome::invalid_params;
  }

  // Verify there is no RB collision.
  if (pusch_alloc.ul_res_grid.collides(scs, pusch_td_cfg.symbols, grant.crbs)) {
    logger.warning("rnti={} Failed to allocate PUSCH. Cause: No space available in scheduler RB resource grid.",
                   u.crnti);
    return alloc_outcome::invalid_params;
  }

  // Allocate PDCCH position.
  pdcch_ul_information* pdcch =
      get_pdcch_sched(grant.cell_index)
          .alloc_ul_pdcch_ue(pdcch_alloc, u.crnti, ue_cell_cfg, ss_cfg.get_id(), grant.aggr_lvl);
  if (pdcch == nullptr) {
    logger.info("rnti={}: Failed to allocate PUSCH. Cause: No space in PDCCH.", u.crnti);
    return alloc_outcome::skip_ue;
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
      pusch_cfg =
          get_pusch_config_f0_0_c_rnti(ue_cell_cfg, bwp_ul_cmn, pusch_td_cfg, nof_harq_ack_bits, is_csi_report_slot);
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
  optional<sch_mcs_tbs> mcs_tbs_info;
  // If it's a new Tx, compute the MCS and TBS from SNR, payload size, and available RBs.
  if (h_ul.empty()) {
    mcs_tbs_info = compute_ul_mcs_tbs(pusch_cfg, ue_cell_cfg, grant.mcs, grant.crbs.length());
  }
  // If it's a reTx, fetch the MCS and TBS from the previous transmission.
  else {
    mcs_tbs_info.emplace(sch_mcs_tbs{.mcs = h_ul.last_tx_params().mcs, .tbs = h_ul.last_tx_params().tbs_bytes});
  }

  // If there is not MCS-TBS info, it means no MCS exists such that the effective code rate is <= 0.95.
  if (not mcs_tbs_info.has_value()) {
    logger.warning(
        "rnti={} Failed to allocate PUSCH. Cause: no MCS such that code rate <= 0.95 with this "
        "configuration: mcs={} crbs={} symbols={} nof_oh={} tb-sc-field={} layers={} pi2bpsk={} "
        "harq_bits={} csi1_bits={} csi2_bits={} mcs_table_idx={} dmrs_A_pos={} is_dmrs_type2={} dmrs_add_pos_idx={}",
        u.crnti,
        grant.mcs.to_uint(),
        grant.crbs,
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
    return alloc_outcome::invalid_params;
  }

  // Mark resources as occupied in the ResourceGrid.
  pusch_alloc.ul_res_grid.fill(grant_info{scs, pusch_td_cfg.symbols, grant.crbs});

  // Remove NTN offset when adding slot to HARQ process.
  slot_point harq_slot = pusch_alloc.slot - ue_cell_cfg.cell_cfg_common.ntn_cs_koffset;
  // Allocate UE UL HARQ.
  if (h_ul.empty()) {
    // It is a new tx.
    h_ul.new_tx(harq_slot, expert_cfg.max_nof_harq_retxs);
  } else {
    // It is a retx.
    h_ul.new_retx(harq_slot);
  }

  // Compute total DAI. See TS 38.213, 9.1.3.2.
  // Total DAI provides total number of transmissions at the end of each interval (slot in a cell). Values range from 1
  // to 4.
  // If a UE is not provided PDSCH-CodeBlockGroupTransmission and the UE is scheduled for a PUSCH transmission by
  // DCI format 0_1 with DAI field value V_T_DAI_UL = 4 and the UE has not received any PDCCH within the monitoring
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
                             grant.crbs,
                             grant.time_res_index,
                             mcs_tbs_info.value().mcs,
                             rv,
                             h_ul);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_0:
      build_dci_f0_0_c_rnti(
          pdcch->dci, ue_cell_cfg, grant.ss_id, grant.crbs, grant.time_res_index, mcs_tbs_info.value().mcs, rv, h_ul);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_1:
      build_dci_f0_1_c_rnti(pdcch->dci,
                            ue_cell_cfg,
                            grant.ss_id,
                            grant.crbs,
                            grant.time_res_index,
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
  msg.context.k2        = pusch_td_cfg.k2;
  msg.context.nof_retxs = h_ul.tb().nof_retxs;
  if (msg.context.nof_retxs == 0 and ue_cc->link_adaptation_controller().is_ul_olla_enabled()) {
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
                               grant.crbs,
                               h_ul.tb().nof_retxs == 0);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_0:
      build_pusch_f0_0_c_rnti(msg.pusch_cfg,
                              u.crnti,
                              pusch_cfg,
                              mcs_tbs_info.value().tbs,
                              cell_cfg,
                              bwp_ul_cmn,
                              pdcch->dci.c_rnti_f0_0,
                              grant.crbs,
                              h_ul.tb().nof_retxs == 0);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_1:
      build_pusch_f0_1_c_rnti(msg.pusch_cfg,
                              u.crnti,
                              pusch_cfg,
                              mcs_tbs_info.value(),
                              ue_cell_cfg,
                              ss_cfg.get_id(),
                              pdcch->dci.c_rnti_f0_1,
                              grant.crbs,
                              h_ul);
      break;
    default:
      report_fatal_error("Unsupported PDCCH UL DCI format");
  }

  // Check if there is any UCI grant allocated on the PUCCH that can be moved to the PUSCH.
  get_uci_alloc(grant.cell_index).multiplex_uci_on_pusch(msg, pusch_alloc, ue_cell_cfg, u.crnti);

  // Save set PDCCH and PUSCH PDU parameters in HARQ process.
  h_ul.save_alloc_params(pdcch->dci.type, msg.pusch_cfg);

  // In case there is a SR pending. Reset it.
  u.reset_sr_indication();

  return alloc_outcome::success;
}
