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
  static constexpr unsigned nof_layers = 1, tb_idx = 0;
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

  // Compute TBS.
  unsigned                   tb_scaling_field  = 0;  // TODO.
  unsigned                   nof_oh_prb        = 0;  // TODO: ue_cell_cfg.cfg_ded().pdsch_serv_cell_cfg;
  const static sch_mcs_index mcs               = 10; // TODO.
  constexpr static unsigned  nof_bits_per_byte = 8U;
  // TODO: DM-RS for UE-dedicated.
  dmrs_information dmrs = make_dmrs_info_common(
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common, grant.time_res_index, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);
  sch_mcs_description mcs_config = pdsch_mcs_get_config(pdsch_mcs_table::qam64, mcs);
  unsigned            tb_size_bytes =
      tbs_calculator_calculate(tbs_calculator_configuration{(unsigned)pdsch_td_cfg.symbols.length(),
                                                            calculate_nof_dmrs_per_rb(dmrs),
                                                            nof_oh_prb,
                                                            mcs_config.target_code_rate / 1024.0F,
                                                            mcs_config.modulation,
                                                            nof_layers,
                                                            tb_scaling_field,
                                                            grant.crbs.length()}) /
      nof_bits_per_byte;

  // Allocate UE DL HARQ.
  prb_interval     prbs = crb_to_prb(*pdcch->ctx.bwp_cfg, grant.crbs);
  dl_harq_process& h_dl = ue_cc->harqs.dl_harq(grant.h_id);
  if (h_dl.empty()) {
    // It is a new tx.
    const static unsigned      max_retx = 4;  // TODO.
    const static sch_mcs_index mcs      = 10; // TODO.
    h_dl.new_tx(dci_dl_rnti_config_type::c_rnti_f1_0,
                ue_cc->active_bwp_id(),
                pdsch_alloc.slot,
                k1,
                prbs,
                mcs,
                tb_size_bytes,
                max_retx);
  } else {
    // It is a retx.
    h_dl.new_retx(ue_cc->active_bwp_id(), pdsch_alloc.slot, k1, prbs, h_dl.tb(0).mcs);
  }

  // Fill DL PDCCH DCI.
  switch (h_dl.dci_config_type()) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0:
      build_dci_f1_0_tc_rnti(pdcch->dci, bwp_dl_cmn, prbs, grant.time_res_index, k1, pucch.pucch_res_indicator, h_dl);
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
                            h_dl,
                            tb_idx);
      break;
    default:
      report_fatal_error("Unsupported RNTI type for PDSCH allocation");
  }

  // Fill PDSCH.
  pdsch_alloc.result.dl.ue_grants.emplace_back();
  dl_msg_alloc& msg         = pdsch_alloc.result.dl.ue_grants.back();
  msg.pdsch_cfg.rnti        = u.crnti;
  msg.pdsch_cfg.bwp_cfg     = pdcch->ctx.bwp_cfg;
  msg.pdsch_cfg.coreset_cfg = pdcch->ctx.coreset_cfg;
  msg.pdsch_cfg.prbs        = prbs;
  msg.pdsch_cfg.symbols     = pdsch_td_cfg.symbols;
  // TODO: Use UE-dedicated DMRS info.
  msg.pdsch_cfg.dmrs           = dmrs;
  msg.pdsch_cfg.is_interleaved = pdcch->dci.c_rnti_f1_0.vrb_to_prb_mapping > 0;
  // See TS38.213, 10.1.
  msg.pdsch_cfg.ss_set_type = ss_cfg->type == search_space_configuration::type_t::ue_dedicated
                                  ? search_space_set_type::ue_specific
                                  : search_space_set_type::type3;
  msg.pdsch_cfg.dci_fmt     = grant.dci_fmt;
  // See TS 38.211, 7.3.1.1. - Scrambling.
  const bwp_downlink_dedicated* bwp_dl_ded = ue_cell_cfg.find_dl_bwp_ded(ue_cc->active_bwp_id());
  if (bwp_dl_ded != nullptr and bwp_dl_ded->pdsch_cfg.has_value() and
      bwp_dl_ded->pdsch_cfg->data_scrambling_id_pdsch.has_value() and
      (grant.dci_fmt != dci_dl_format::f1_0 or ss_cfg->type != search_space_configuration::type_t::common)) {
    msg.pdsch_cfg.n_id = *bwp_dl_ded->pdsch_cfg->data_scrambling_id_pdsch;
  } else {
    msg.pdsch_cfg.n_id = cell_cfg.pci;
  }
  // Add codeword.
  msg.pdsch_cfg.codewords.emplace_back();
  pdsch_codeword&                          cw     = msg.pdsch_cfg.codewords.back();
  static constexpr std::array<unsigned, 4> rv_idx = {0, 2, 3, 1};
  cw.rv_index                                     = rv_idx[h_dl.tb(tb_idx).nof_retxs % rv_idx.size()];
  cw.mcs_index                                    = h_dl.tb(tb_idx).mcs;
  cw.mcs_table                                    = pdsch_mcs_table::qam64;
  cw.qam_mod                                      = mcs_config.modulation;
  cw.target_code_rate                             = mcs_config.target_code_rate;
  cw.tb_size_bytes                                = tb_size_bytes;

  // Set MAC logical channels to schedule in this PDU.
  msg.tb_list.emplace_back();
  u.build_dl_transport_block_info(msg.tb_list.back(), cw.tb_size_bytes);

  return true;
}

bool ue_cell_grid_allocator::allocate_ul_grant(const ue_pusch_grant& grant)
{
  srsgnb_assert(ues.contains(grant.user->ue_index), "Invalid UE candidate index={}", grant.user->ue_index);
  srsgnb_assert(has_cell(grant.cell_index), "Invalid UE candidate cell_index={}", grant.cell_index);
  constexpr static unsigned time_resource        = 0; // TODO: Support other time resources.
  constexpr static unsigned pdcch_delay_in_slots = 0;
  constexpr static unsigned nof_layers           = 1;

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

  // Allocate UE UL HARQ.
  ul_harq_process&           h_ul = ue_cc->harqs.ul_harq(grant.h_id);
  prb_interval               prbs = crb_to_prb(*pdcch->ctx.bwp_cfg, grant.crbs);
  const static sch_mcs_index mcs  = 10; // TODO: Parameterize.
  dmrs_information           dmrs = make_dmrs_info_common(
      *cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common, time_resource, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);
  sch_mcs_description mcs_config = pusch_mcs_get_config(pusch_mcs_table::qam64, mcs, false);
  if (h_ul.empty()) {
    // It is a new tx.
    const static unsigned     max_retx          = 4; // TODO: Parameterize
    unsigned                  nof_oh_prb        = 0; // TODO.
    unsigned                  tb_scaling_field  = 0; // TODO.
    constexpr static unsigned nof_bits_per_byte = 8U;
    // Calculate TBS
    unsigned tbs_bytes = tbs_calculator_calculate(tbs_calculator_configuration{(unsigned)pusch_td_cfg.symbols.length(),
                                                                               calculate_nof_dmrs_per_rb(dmrs),
                                                                               nof_oh_prb,
                                                                               mcs_config.target_code_rate / 1024.0F,
                                                                               mcs_config.modulation,
                                                                               nof_layers,
                                                                               tb_scaling_field,
                                                                               grant.crbs.length()}) /
                         nof_bits_per_byte;
    h_ul.new_tx(
        dci_ul_rnti_config_type::c_rnti_f0_0, ue_cc->active_bwp_id(), pusch_alloc.slot, prbs, mcs, tbs_bytes, max_retx);
  } else {
    // It is a retx.
    h_ul.new_retx(ue_cc->active_bwp_id(), pusch_alloc.slot, prbs, h_ul.tb().mcs);
  }

  // Fill UL PDCCH DCI.
  build_dci_f0_0_c_rnti(pdcch->dci,
                        cell_cfg.dl_cfg_common.init_dl_bwp,
                        ue_cell_cfg.dl_bwp_common(ue_cc->active_bwp_id()).generic_params,
                        cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                        bwp_ul_cmn.generic_params,
                        ss_cfg->type,
                        prbs,
                        time_resource,
                        h_ul);

  // Fill PUSCH.
  pusch_alloc.result.ul.puschs.emplace_back();
  ul_sched_info& msg                       = pusch_alloc.result.ul.puschs.back();
  msg.pusch_cfg.rnti                       = u.crnti;
  msg.pusch_cfg.bwp_cfg                    = pdcch->ctx.bwp_cfg;
  msg.pusch_cfg.prbs                       = prbs;
  msg.pusch_cfg.symbols                    = pusch_td_cfg.symbols;
  msg.pusch_cfg.intra_slot_freq_hopping    = false; // TODO.
  msg.pusch_cfg.tx_direct_current_location = 0;     // TODO.
  msg.pusch_cfg.ul_freq_shift_7p5khz       = false;
  msg.pusch_cfg.mcs_table                  = pusch_mcs_table::qam64;
  msg.pusch_cfg.mcs_index                  = h_ul.tb().mcs;
  msg.pusch_cfg.qam_mod                    = mcs_config.modulation;
  msg.pusch_cfg.target_code_rate           = mcs_config.target_code_rate;
  msg.pusch_cfg.transform_precoding = cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->msg3_transform_precoder;
  msg.pusch_cfg.n_id                = cell_cfg.pci;
  msg.pusch_cfg.nof_layers          = nof_layers;
  msg.pusch_cfg.dmrs                = dmrs;
  msg.pusch_cfg.pusch_dmrs_id       = cell_cfg.pci;
  msg.pusch_cfg.dmrs_hopping_mode   = pusch_information::dmrs_hopping_mode::no_hopping; // TODO.
  static constexpr std::array<unsigned, 4> rv_idx = {0, 2, 3, 1};
  msg.pusch_cfg.rv_index                          = rv_idx[h_ul.tb().nof_retxs % rv_idx.size()];
  msg.pusch_cfg.harq_id                           = h_ul.id;
  msg.pusch_cfg.new_data                          = h_ul.tb().nof_retxs == 0;
  msg.pusch_cfg.tb_size_bytes                     = h_ul.tb().tbs_bytes;
  msg.pusch_cfg.num_cb                            = 0;

  // In case there is a SR pending. Reset it.
  u.reset_sr_indication();

  return true;
}
