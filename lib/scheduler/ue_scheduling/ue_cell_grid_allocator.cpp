/*
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_cell_grid_allocator.h"
#include "../support/config_helpers.h"
#include "../support/dmrs_helpers.h"
#include "../support/tbs_calculator.h"
#include "srsgnb/support/error_handling.h"
#include "ue_dci_builder.h"
#include "ue_sch_pdu_builder.h"

using namespace srsgnb;

ue_cell_grid_allocator::ue_cell_grid_allocator(ue_list& ues_, srslog::basic_logger& logger_) :
  ues(ues_), logger(logger_)
{
}

void ue_cell_grid_allocator::add_cell(du_cell_index_t           cell_index,
                                      pdcch_resource_allocator& pdcch_sched,
                                      pucch_allocator&          pucch_alloc,
                                      cell_resource_allocator&  cell_alloc)
{
  cells.emplace(cell_index, cell_t{cell_index, &pdcch_sched, &pucch_alloc, &cell_alloc});
}

bool ue_cell_grid_allocator::allocate_dl_grant(const ue_pdsch_grant& grant)
{
  srsgnb_assert(ues.contains(grant.user->ue_index), "Invalid UE candidate index={}", grant.user->ue_index);
  srsgnb_assert(has_cell(grant.cell_index), "Invalid UE candidate cell_index={}", grant.cell_index);
  ue& u = ues[grant.user->ue_index];

  // Verify UE carrier is active.
  ue_cell* ue_cc = u.find_cell(grant.cell_index);
  if (ue_cc == nullptr or not ue_cc->is_active()) {
    logger.warning("SCHED: PDSCH allocation failed. Cause: The UE={} carrier with cell_index={} is inactive",
                   u.ue_index,
                   grant.cell_index);
    return false;
  }
  const ue_cell_configuration&                 ue_cell_cfg = ue_cc->cfg();
  const cell_configuration&                    cell_cfg    = ue_cell_cfg.cell_cfg_common;
  const bwp_downlink_common&                   bwp_dl_cmn  = ue_cell_cfg.dl_bwp_common(ue_cc->active_bwp_id());
  subcarrier_spacing                           scs         = bwp_dl_cmn.generic_params.scs;
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg =
      ue_cc->cfg().get_pdsch_time_domain_list(grant.ss_id)[grant.time_res_index];

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = get_res_alloc(grant.cell_index)[0];
  cell_slot_resource_allocator& pdsch_alloc = get_res_alloc(grant.cell_index)[pdsch_td_cfg.k0];

  // Verify there is space in PDSCH and PDCCH result lists for new allocations.
  if (pdsch_alloc.result.dl.ue_grants.full() or pdcch_alloc.result.dl.dl_pdcchs.full()) {
    logger.warning("Failed to allocate PDSCH. Cause: No space available in scheduler output list");
    return false;
  }

  // Verify there is no RB collision.
  if (pdsch_alloc.dl_res_grid.collides(scs, pdsch_td_cfg.symbols, grant.crbs)) {
    logger.warning("Failed to allocate PDSCH. Cause: No space available in scheduler RB resource grid.");
    return false;
  }

  // Find a SearchSpace candidate.
  const search_space_configuration* ss_cfg = ue_cell_cfg.find_search_space(grant.ss_id);
  if (ss_cfg == nullptr) {
    logger.warning("Failed to allocate PDSCH. Cause: No valid SearchSpace found.");
    return false;
  }

  // Allocate PDCCH position.
  pdcch_dl_information* pdcch =
      get_pdcch_sched(grant.cell_index)
          .alloc_dl_pdcch_ue(pdcch_alloc, u.crnti, ue_cell_cfg, ue_cc->active_bwp_id(), ss_cfg->id, grant.aggr_lvl);
  if (pdcch == nullptr) {
    logger.warning("Failed to allocate PDSCH. Cause: No space in PDCCH.");
    return false;
  }

  // Allocate PUCCH.
  const unsigned       k1 = 4;
  pucch_harq_ack_grant pucch =
      get_pucch_alloc(grant.cell_index)
          .alloc_ded_pucch_harq_ack_ue(
              get_res_alloc(grant.cell_index), u.crnti, u.get_pcell().cfg(), pdsch_td_cfg.k0, k1);
  if (pucch.pucch_pdu == nullptr) {
    logger.warning("Failed to allocate PDSCH. Cause: No space in PUCCH.");
    // TODO: remove PDCCH allocation.
    return false;
  }
  // TODO: fills DCI with PUCCH info.

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, grant.crbs});

  // Allocate UE DL HARQ.
  prb_interval            prbs = crb_to_prb(*pdcch->ctx.bwp_cfg, grant.crbs);
  dl_harq_process&        h_dl = ue_cc->harqs.dl_harq(grant.h_id);
  sch_mcs_index           mcs;
  dci_dl_rnti_config_type dci_cfg_type;
  if (h_dl.empty()) {
    // It is a new tx.
    const static unsigned max_retx = 4;  // TODO.
    mcs                            = 10; // TODO: Parameterize.
    dci_cfg_type                   = dci_dl_rnti_config_type::c_rnti_f1_0;

    h_dl.new_tx(pdsch_alloc.slot, k1, max_retx);
  } else {
    // It is a retx.
    mcs          = h_dl.last_alloc_params().tb[0]->mcs;
    dci_cfg_type = h_dl.last_alloc_params().dci_cfg_type;

    h_dl.new_retx(pdsch_alloc.slot, k1);
  }

  // Fill DL PDCCH DCI PDU.
  switch (dci_cfg_type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0:
      build_dci_f1_0_tc_rnti(
          pdcch->dci, bwp_dl_cmn, prbs, grant.time_res_index, k1, pucch.pucch_res_indicator, mcs, h_dl);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      build_dci_f1_0_c_rnti(pdcch->dci,
                            cell_cfg.dl_cfg_common.init_dl_bwp,
                            bwp_dl_cmn.generic_params,
                            ss_cfg->type,
                            prbs,
                            grant.time_res_index,
                            k1,
                            pucch.pucch_res_indicator,
                            mcs,
                            h_dl);
      break;
    default:
      report_fatal_error("Unsupported RNTI type for PDSCH allocation");
  }

  // Fill PDSCH PDU.
  pdsch_alloc.result.dl.ue_grants.emplace_back();
  dl_msg_alloc& msg = pdsch_alloc.result.dl.ue_grants.back();
  switch (pdcch->dci.type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0:
      build_pdsch_f1_0_tc_rnti(msg.pdsch_cfg, u.crnti, cell_cfg, pdcch->dci.tc_rnti_f1_0);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      build_pdsch_f1_0_c_rnti(
          msg.pdsch_cfg, u.crnti, ue_cell_cfg, ue_cc->active_bwp_id(), *ss_cfg, pdcch->dci.c_rnti_f1_0);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI format");
  }

  // Save set PDCCH and PDSCH PDU parameters in HARQ process.
  h_dl.save_alloc_params(pdcch->dci.type, msg.pdsch_cfg);

  // Set MAC logical channels to schedule in this PDU.
  msg.tb_list.emplace_back();
  u.build_dl_transport_block_info(msg.tb_list.back(), msg.pdsch_cfg.codewords[0].tb_size_bytes);

  return true;
}

bool ue_cell_grid_allocator::allocate_ul_grant(const ue_pusch_grant& grant)
{
  srsgnb_assert(ues.contains(grant.user->ue_index), "Invalid UE candidate index={}", grant.user->ue_index);
  srsgnb_assert(has_cell(grant.cell_index), "Invalid UE candidate cell_index={}", grant.cell_index);
  constexpr static unsigned time_resource        = 0; // TODO: Support other time resources.
  constexpr static unsigned pdcch_delay_in_slots = 0;

  ue& u = ues[grant.user->ue_index];

  // Verify UE carrier is active.
  ue_cell* ue_cc = u.find_cell(grant.cell_index);
  if (ue_cc == nullptr or not ue_cc->is_active()) {
    logger.warning("SCHED: PDSCH allocation failed. Cause: The UE={} carrier with cell_index={} is inactive",
                   u.ue_index,
                   grant.cell_index);
    return false;
  }
  const ue_cell_configuration&                 ue_cell_cfg = ue_cc->cfg();
  const cell_configuration&                    cell_cfg    = ue_cell_cfg.cell_cfg_common;
  const bwp_uplink_common&                     bwp_ul_cmn  = ue_cell_cfg.ul_bwp_common(ue_cc->active_bwp_id());
  subcarrier_spacing                           scs         = bwp_ul_cmn.generic_params.scs;
  const pusch_time_domain_resource_allocation& pusch_td_cfg =
      bwp_ul_cmn.pusch_cfg_common->pusch_td_alloc_list[time_resource];

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = get_res_alloc(grant.cell_index)[pdcch_delay_in_slots];
  cell_slot_resource_allocator& pusch_alloc = get_res_alloc(grant.cell_index)[pdcch_delay_in_slots + pusch_td_cfg.k2];

  // Verify there is space in PUSCH and PDCCH result lists for new allocations.
  if (pusch_alloc.result.ul.puschs.full() or pdcch_alloc.result.dl.dl_pdcchs.full()) {
    logger.warning("Failed to allocate PUSCH. Cause: No space available in scheduler output list");
    return false;
  }

  // Verify there is no RB collision.
  if (pusch_alloc.ul_res_grid.collides(scs, pusch_td_cfg.symbols, grant.crbs)) {
    logger.warning("Failed to allocate PUSCH. Cause: No space available in scheduler RB resource grid.");
    return false;
  }

  // Find a SearchSpace candidate.
  const search_space_configuration* ss_cfg = ue_cell_cfg.find_search_space(grant.ss_id);
  if (ss_cfg == nullptr) {
    logger.warning("Failed to allocate PDSCH. Cause: No valid SearchSpace found.");
    return false;
  }

  // Allocate PDCCH position.
  pdcch_ul_information* pdcch =
      get_pdcch_sched(grant.cell_index)
          .alloc_ul_pdcch_ue(pdcch_alloc, u.crnti, ue_cell_cfg, ue_cc->active_bwp_id(), ss_cfg->id, grant.aggr_lvl);
  if (pdcch == nullptr) {
    logger.warning("Failed to allocate PDSCH. Cause: No space in PDCCH.");
    return false;
  }

  // Mark resources as occupied in the ResourceGrid.
  pusch_alloc.ul_res_grid.fill(grant_info{scs, pusch_td_cfg.symbols, grant.crbs});

  // Derive MCS.
  sch_mcs_index    mcs;
  ul_harq_process& h_ul = ue_cc->harqs.ul_harq(grant.h_id);
  if (h_ul.empty()) {
    mcs = 10; // TODO: Parameterize.
  } else {
    mcs = h_ul.last_tx_params().mcs;
  }

  // Allocate UE UL HARQ.
  if (h_ul.empty()) {
    // It is a new tx.
    const static unsigned max_retx = 4; // TODO: Parameterize
    h_ul.new_tx(pusch_alloc.slot, max_retx);
  } else {
    // It is a retx.
    h_ul.new_retx(pusch_alloc.slot);
  }

  // Fill UL PDCCH DCI.
  prb_interval prbs = crb_to_prb(*pdcch->ctx.bwp_cfg, grant.crbs);
  build_dci_f0_0_c_rnti(pdcch->dci,
                        cell_cfg.dl_cfg_common.init_dl_bwp,
                        ue_cell_cfg.dl_bwp_common(ue_cc->active_bwp_id()).generic_params,
                        cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                        bwp_ul_cmn.generic_params,
                        ss_cfg->type,
                        prbs,
                        time_resource,
                        mcs,
                        h_ul);

  // Fill PUSCH.
  pusch_alloc.result.ul.puschs.emplace_back();
  ul_sched_info& msg = pusch_alloc.result.ul.puschs.back();
  switch (pdcch->dci.type) {
    case dci_ul_rnti_config_type::tc_rnti_f0_0:
      build_pusch_f0_0_tc_rnti(msg.pusch_cfg, u.crnti, cell_cfg, pdcch->dci.tc_rnti_f0_0, h_ul.tb().nof_retxs == 0);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_0:
      build_pusch_f0_0_c_rnti(
          msg.pusch_cfg, u.crnti, cell_cfg, bwp_ul_cmn, pdcch->dci.c_rnti_f0_0, h_ul.tb().nof_retxs == 0);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI format");
  }

  // Save set PDCCH and PUSCH PDU parameters in HARQ process.
  h_ul.save_alloc_params(pdcch->dci.type, msg.pusch_cfg);

  // In case there is a SR pending. Reset it.
  u.reset_sr_indication();

  return true;
}
