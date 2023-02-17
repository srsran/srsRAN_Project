/*
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_cell_grid_allocator.h"
#include "../support/mcs_calculator.h"
#include "../support/mcs_tbs_calculator.h"
#include "ue_dci_builder.h"
#include "ue_sch_pdu_builder.h"
#include "srsgnb/support/error_handling.h"

using namespace srsgnb;

ue_cell_grid_allocator::ue_cell_grid_allocator(const scheduler_ue_expert_config& expert_cfg_,
                                               ue_list&                          ues_,
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

bool ue_cell_grid_allocator::allocate_dl_grant(const ue_pdsch_grant& grant)
{
  srsgnb_assert(ues.contains(grant.user->ue_index), "Invalid UE candidate index={}", grant.user->ue_index);
  srsgnb_assert(has_cell(grant.cell_index), "Invalid UE candidate cell_index={}", grant.cell_index);
  ue& u = ues[grant.user->ue_index];

  // Verify UE carrier is active.
  ue_cell* ue_cc = u.find_cell(grant.cell_index);
  if (ue_cc == nullptr or not ue_cc->is_active()) {
    logger.warning("PDSCH allocation failed. Cause: The ue={} carrier with cell_index={} is inactive",
                   u.ue_index,
                   grant.cell_index);
    return false;
  }

  const ue_cell_configuration&  ue_cell_cfg = ue_cc->cfg();
  const cell_configuration&     cell_cfg    = ue_cell_cfg.cell_cfg_common;
  const bwp_downlink_common&    init_dl_bwp = ue_cell_cfg.dl_bwp_common(to_bwp_id(0));
  const bwp_downlink_common&    bwp_dl_cmn  = ue_cell_cfg.dl_bwp_common(ue_cc->active_bwp_id());
  dl_harq_process&              h_dl        = ue_cc->harqs.dl_harq(grant.h_id);
  const dci_dl_rnti_config_type dci_type =
      h_dl.empty() ? dci_dl_rnti_config_type::c_rnti_f1_0 : h_dl.last_alloc_params().dci_cfg_type;

  // See 3GPP TS 38.213, clause 10.1,
  // A UE monitors PDCCH candidates in one or more of the following search spaces sets
  //  - a Type1-PDCCH CSS set configured by ra-SearchSpace in PDCCH-ConfigCommon for a DCI format with
  //    CRC scrambled by a RA-RNTI, a MsgB-RNTI, or a TC-RNTI on the primary cell.
  if (dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0 and
      grant.ss_id != cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id) {
    logger.debug("Failed to allocate PDSCH. Cause: SearchSpace not valid for re-transmission of msg4.");
    return false;
  }

  // Find a SearchSpace candidate.
  const search_space_configuration* ss_cfg = ue_cell_cfg.find_search_space(grant.ss_id);
  if (ss_cfg == nullptr) {
    logger.warning("Failed to allocate PDSCH. Cause: No valid SearchSpace found.");
    return false;
  }

  const bwp_configuration bwp_cfg =
      get_resource_alloc_type_1_dl_bwp_size(dci_type, init_dl_bwp, bwp_dl_cmn, ss_cfg->type);
  const subcarrier_spacing                                scs = bwp_cfg.scs;
  const span<const pdsch_time_domain_resource_allocation> pdsch_list =
      ue_cell_cfg.get_pdsch_time_domain_list(ss_cfg->id);
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = pdsch_list[grant.time_res_index];

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = get_res_alloc(grant.cell_index)[0];
  cell_slot_resource_allocator& pdsch_alloc = get_res_alloc(grant.cell_index)[pdsch_td_cfg.k0];

  if (not cell_cfg.is_dl_enabled(pdcch_alloc.slot)) {
    logger.warning("Failed to allocate PDSCH in slot={}. Cause: DL is not active in the PDCCH slot", pdsch_alloc.slot);
    return false;
  }
  if (not cell_cfg.is_dl_enabled(pdsch_alloc.slot)) {
    logger.warning("Failed to allocate PDSCH in slot={}. Cause: DL is not active in the PDSCH slot", pdsch_alloc.slot);
    return false;
  }

  // Verify there is space in PDSCH and PDCCH result lists for new allocations.
  if (pdsch_alloc.result.dl.ue_grants.full() or pdcch_alloc.result.dl.dl_pdcchs.full()) {
    logger.warning("Failed to allocate PDSCH. Cause: No space available in scheduler output list");
    return false;
  }

  // Verify CRBs fit in the chosen BWP.
  if (not bwp_cfg.crbs.contains(grant.crbs)) {
    logger.warning(
        "Failed to allocate PDSCH. Cause: CRBs allocated outside the BWP.", grant.crbs.length(), bwp_cfg.crbs.length());
    return false;
  }

  // In case of retx, ensure the number of PRBs for the grant did not change.
  if (not h_dl.empty() and grant.crbs.length() != h_dl.last_alloc_params().prbs.prbs().length()) {
    logger.warning("Failed to allocate PDSCH. Cause: Number of CRBs has to remain constant during retxs (Harq-id={}, "
                   "nof_prbs={}!={})",
                   h_dl.id,
                   h_dl.last_alloc_params().prbs.prbs().length(),
                   grant.crbs.length());
    return false;
  }

  // Verify there is no RB collision.
  if (pdsch_alloc.dl_res_grid.collides(scs, pdsch_td_cfg.symbols, grant.crbs)) {
    logger.warning("Failed to allocate PDSCH. Cause: No space available in scheduler RB resource grid.");
    return false;
  }

  // Allocate PDCCH position.
  pdcch_dl_information* pdcch =
      get_pdcch_sched(grant.cell_index)
          .alloc_dl_pdcch_ue(pdcch_alloc, u.crnti, ue_cell_cfg, ue_cc->active_bwp_id(), ss_cfg->id, grant.aggr_lvl);
  if (pdcch == nullptr) {
    logger.info("Failed to allocate PDSCH. Cause: No space in PDCCH.");
    return false;
  }

  // Allocate UCI. UCI destination (i.e., PUCCH or PUSCH) depends on whether there exist a PUSCH grant for the UE.
  unsigned       k1      = 0;
  auto           k1_list = ue_cell_cfg.get_k1_candidates();
  uci_allocation uci     = {};
  // Note: Right now, k1 candidates are hardcoded for TDD to avoid PUCCH format2.
  // TODO: Do not hardcode k1.
  if (ue_cell_cfg.cell_cfg_common.tdd_cfg_common.has_value()) {
    static constexpr std::array<uint8_t, 6> tdd_slot_idx_to_k1 = {7, 6, 6, 5, 5, 4};
    k1_list                                                    = span<const uint8_t>{tdd_slot_idx_to_k1}.subspan(
        pdsch_alloc.slot.slot_index() % nof_slots_per_tdd_period(*ue_cell_cfg.cell_cfg_common.tdd_cfg_common), 1);
  }
  for (const uint8_t k1_candidate : k1_list) {
    const slot_point uci_slot = pdsch_alloc.slot + k1_candidate;
    if (not cell_cfg.is_ul_enabled(uci_slot)) {
      continue;
    }
    uci = get_uci_alloc(grant.cell_index)
              .alloc_uci_harq_ue(
                  get_res_alloc(grant.cell_index), u.crnti, u.get_pcell().cfg(), pdsch_td_cfg.k0, k1_candidate);
    if (uci.alloc_successful) {
      k1 = k1_candidate;
      break;
    }
  }
  if (not uci.alloc_successful) {
    logger.warning("Failed to allocate PDSCH. Cause: No space in PUCCH.");
    get_pdcch_sched(grant.cell_index).cancel_last_pdcch(pdcch_alloc);
    return false;
  }

  pdsch_config_params pdsch_cfg;
  switch (dci_type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0:
      pdsch_cfg = get_pdsch_config_f1_0_tc_rnti(cell_cfg, grant.time_res_index);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      pdsch_cfg = get_pdsch_config_f1_0_c_rnti(cell_cfg, ue_cell_cfg, grant.time_res_index);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI UL format");
  }

  optional<sch_mcs_tbs> mcs_tbs_info;
  // If it's a new Tx, compute the MCS and TBS.
  if (h_dl.empty()) {
    mcs_tbs_info = compute_dl_mcs_tbs(pdsch_cfg, ue_cell_cfg, grant.mcs, grant.crbs.length());
  } else {
    // It is a retx.
    mcs_tbs_info.emplace(
        sch_mcs_tbs{.mcs = h_dl.last_alloc_params().tb[0]->mcs, .tbs = h_dl.last_alloc_params().tb[0]->tbs_bytes});
  }

  // If there is not MCS-TBS info, it means no MCS exists such that the effective code rate is <= 0.95.
  if (not mcs_tbs_info.has_value()) {
    logger.warning("Failed to allocate PDSCH. Cause: no MCS such that code rate <= 0.95.");
    get_pdcch_sched(grant.cell_index).cancel_last_pdcch(pdcch_alloc);
    return false;
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, grant.crbs});

  // Allocate UE DL HARQ.
  prb_interval prbs = crb_to_prb(bwp_cfg, grant.crbs);

  // Allocate UE UL HARQ.
  if (h_dl.empty()) {
    // It is a new tx.
    h_dl.new_tx(pdsch_alloc.slot, k1, expert_cfg.max_nof_harq_retxs, uci.dai);
  } else {
    // It is a retx.
    h_dl.new_retx(pdsch_alloc.slot, k1, uci.dai);
  }

  // Fill DL PDCCH DCI PDU.
  switch (dci_type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0:
      build_dci_f1_0_tc_rnti(pdcch->dci,
                             init_dl_bwp,
                             prbs,
                             grant.time_res_index,
                             k1,
                             uci.pucch_grant.pucch_res_indicator,
                             mcs_tbs_info.value().mcs,
                             h_dl);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      build_dci_f1_0_c_rnti(pdcch->dci,
                            init_dl_bwp,
                            bwp_dl_cmn.generic_params,
                            ss_cfg->type,
                            prbs,
                            grant.time_res_index,
                            k1,
                            uci.pucch_grant.pucch_res_indicator,
                            uci.dai,
                            mcs_tbs_info.value().mcs,
                            h_dl);
      break;
    default:
      report_fatal_error("Unsupported RNTI type for PDSCH allocation");
  }

  // Fill PDSCH PDU.
  dl_msg_alloc& msg    = pdsch_alloc.result.dl.ue_grants.emplace_back();
  msg.context.ue_index = u.ue_index;
  msg.context.k1       = k1;
  msg.context.ss_id    = ss_cfg->id;
  switch (pdcch->dci.type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0:
      build_pdsch_f1_0_tc_rnti(msg.pdsch_cfg,
                               pdsch_cfg,
                               mcs_tbs_info.value().tbs,
                               u.crnti,
                               cell_cfg,
                               pdcch->dci.tc_rnti_f1_0,
                               h_dl.tb(0).nof_retxs == 0);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      build_pdsch_f1_0_c_rnti(msg.pdsch_cfg,
                              pdsch_cfg,
                              mcs_tbs_info.value().tbs,
                              u.crnti,
                              ue_cell_cfg,
                              ue_cc->active_bwp_id(),
                              *ss_cfg,
                              pdcch->dci.c_rnti_f1_0,
                              h_dl.tb(0).nof_retxs == 0);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI format");
  }

  // Save set PDCCH and PDSCH PDU parameters in HARQ process.
  h_dl.save_alloc_params(pdcch->dci.type, msg.pdsch_cfg);

  if (h_dl.tb(0).nof_retxs == 0) {
    // Set MAC logical channels to schedule in this PDU if it is a newtx.
    u.build_dl_transport_block_info(msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes);
  }

  logger.debug(
      "ue={}'s PDSCH allocation on slot={} at cell_id={} completed.", u.ue_index, pdsch_alloc.slot, grant.cell_index);

  return true;
}

bool ue_cell_grid_allocator::allocate_ul_grant(const ue_pusch_grant& grant)
{
  srsgnb_assert(ues.contains(grant.user->ue_index), "Invalid UE candidate index={}", grant.user->ue_index);
  srsgnb_assert(has_cell(grant.cell_index), "Invalid UE candidate cell_index={}", grant.cell_index);
  constexpr static unsigned pdcch_delay_in_slots = 0;

  ue& u = ues[grant.user->ue_index];

  // Verify UE carrier is active.
  ue_cell* ue_cc = u.find_cell(grant.cell_index);
  if (ue_cc == nullptr or not ue_cc->is_active()) {
    logger.warning("PUSCH allocation failed. Cause: The ue={} carrier with cell_index={} is inactive",
                   u.ue_index,
                   grant.cell_index);
    return false;
  }

  const ue_cell_configuration& ue_cell_cfg = ue_cc->cfg();
  const cell_configuration&    cell_cfg    = ue_cell_cfg.cell_cfg_common;
  const bwp_uplink_common&     init_ul_bwp = ue_cell_cfg.ul_bwp_common(to_bwp_id(0));
  const bwp_uplink_common&     bwp_ul_cmn  = ue_cell_cfg.ul_bwp_common(ue_cc->active_bwp_id());
  ul_harq_process&             h_ul        = ue_cc->harqs.ul_harq(grant.h_id);
  // TODO: Get correct DCI format.
  const dci_ul_rnti_config_type dci_type = dci_ul_rnti_config_type::c_rnti_f0_0;

  // Find a SearchSpace candidate.
  const search_space_configuration* ss_cfg = ue_cell_cfg.find_search_space(grant.ss_id);
  if (ss_cfg == nullptr) {
    logger.warning("Failed to allocate PUSCH. Cause: No valid SearchSpace found.");
    return false;
  }

  const bwp_configuration  bwp_lims = ue_cc->alloc_type1_bwp_limits(dci_ul_format::f0_0, ss_cfg->type);
  const subcarrier_spacing scs      = bwp_ul_cmn.generic_params.scs;
  span<const pusch_time_domain_resource_allocation> pusch_list   = ue_cell_cfg.get_pusch_time_domain_list(ss_cfg->id);
  const pusch_time_domain_resource_allocation&      pusch_td_cfg = pusch_list[grant.time_res_index];

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = get_res_alloc(grant.cell_index)[pdcch_delay_in_slots];
  cell_slot_resource_allocator& pusch_alloc = get_res_alloc(grant.cell_index)[pdcch_delay_in_slots + pusch_td_cfg.k2];

  if (not cell_cfg.is_dl_enabled(pdcch_alloc.slot)) {
    logger.warning("Failed to allocate PUSCH in slot={}. Cause: DL is not active in the PDCCH slot", pusch_alloc.slot);
    return false;
  }
  if (not cell_cfg.is_ul_enabled(pusch_alloc.slot)) {
    logger.warning("Failed to allocate PUSCH in slot={}. Cause: UL is not active in the PUSCH slot (k2={})",
                   pusch_alloc.slot,
                   pusch_td_cfg.k2);
    return false;
  }

  // Verify there is space in PUSCH and PDCCH result lists for new allocations.
  if (pusch_alloc.result.ul.puschs.full() or pdcch_alloc.result.dl.dl_pdcchs.full()) {
    logger.warning("Failed to allocate PUSCH in slot={}. Cause: No space available in scheduler output list",
                   pusch_alloc.slot);
    return false;
  }

  // Verify CRBs allocation.
  if (not bwp_lims.crbs.contains(grant.crbs)) {
    logger.warning("Failed to allocate PUSCH. Cause: CRBs allocated outside the BWP.",
                   grant.crbs.length(),
                   bwp_lims.crbs.length());
    return false;
  }

  // In case of retx, ensure the number of PRBs for the grant did not change.
  if (not h_ul.empty() and grant.crbs.length() != h_ul.last_tx_params().prbs.prbs().length()) {
    logger.warning("Failed to allocate PUSCH. Cause: Number of CRBs has to remain constant during retxs (harq-id={}, "
                   "nof_prbs={}!={})",
                   h_ul.id,
                   h_ul.last_tx_params().prbs.prbs().length(),
                   grant.crbs.length());
    return false;
  }

  // Verify there is no RB collision.
  if (pusch_alloc.ul_res_grid.collides(scs, pusch_td_cfg.symbols, grant.crbs)) {
    logger.warning("Failed to allocate PUSCH. Cause: No space available in scheduler RB resource grid.");
    return false;
  }

  // Allocate PDCCH position.
  pdcch_ul_information* pdcch =
      get_pdcch_sched(grant.cell_index)
          .alloc_ul_pdcch_ue(pdcch_alloc, u.crnti, ue_cell_cfg, ue_cc->active_bwp_id(), ss_cfg->id, grant.aggr_lvl);
  if (pdcch == nullptr) {
    logger.info("Failed to allocate PUSCH. Cause: No space in PDCCH.");
    return false;
  }

  // Fetch PUSCH parameters based on type of transmission.
  pusch_config_params pusch_cfg;
  switch (dci_type) {
    case dci_ul_rnti_config_type::tc_rnti_f0_0:
      pusch_cfg = get_pusch_config_f0_0_tc_rnti(cell_cfg, grant.time_res_index);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_0:
      pusch_cfg = get_pusch_config_f0_0_c_rnti(cell_cfg, ue_cell_cfg, bwp_ul_cmn, grant.time_res_index);
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
    logger.warning("Failed to allocate PUSCH. Cause: no MCS such that code rate <= 0.95.");
    get_pdcch_sched(grant.cell_index).cancel_last_pdcch(pdcch_alloc);
    return false;
  }

  // Mark resources as occupied in the ResourceGrid.
  pusch_alloc.ul_res_grid.fill(grant_info{scs, pusch_td_cfg.symbols, grant.crbs});

  // Compute the available PRBs in the grid for this transmission.
  prb_interval prbs = crb_to_prb(bwp_lims, grant.crbs);

  // Allocate UE UL HARQ.
  if (h_ul.empty()) {
    // It is a new tx.
    h_ul.new_tx(pusch_alloc.slot, expert_cfg.max_nof_harq_retxs);
  } else {
    // It is a retx.
    h_ul.new_retx(pusch_alloc.slot);
  }

  // Fill UL PDCCH DCI.
  build_dci_f0_0_c_rnti(pdcch->dci,
                        cell_cfg.dl_cfg_common.init_dl_bwp,
                        ue_cell_cfg.dl_bwp_common(ue_cc->active_bwp_id()).generic_params,
                        init_ul_bwp.generic_params,
                        bwp_ul_cmn.generic_params,
                        ss_cfg->type,
                        prbs,
                        grant.time_res_index,
                        mcs_tbs_info.value().mcs,
                        h_ul);

  // Fill PUSCH.
  ul_sched_info& msg   = pusch_alloc.result.ul.puschs.emplace_back();
  msg.context.ue_index = u.ue_index;
  msg.context.ss_id    = ss_cfg->id;
  switch (pdcch->dci.type) {
    case dci_ul_rnti_config_type::tc_rnti_f0_0:
      build_pusch_f0_0_tc_rnti(msg.pusch_cfg,
                               pusch_cfg,
                               mcs_tbs_info.value().tbs,
                               u.crnti,
                               cell_cfg,
                               pdcch->dci.tc_rnti_f0_0,
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
                              h_ul.tb().nof_retxs == 0);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI format");
  }

  // Check if there is any UCI grant allocated on the PUCCH that can be moved to the PUSCH.
  get_uci_alloc(grant.cell_index).multiplex_uci_on_pusch(msg, pusch_alloc, ue_cell_cfg, u.crnti);

  // Save set PDCCH and PUSCH PDU parameters in HARQ process.
  h_ul.save_alloc_params(pdcch->dci.type, msg.pusch_cfg);

  // In case there is a SR pending. Reset it.
  u.reset_sr_indication();

  logger.debug(
      "ue={}'s PUSCH allocation on slot={} at cell_id={} completed.", u.ue_index, pusch_alloc.slot, grant.cell_index);
  return true;
}
