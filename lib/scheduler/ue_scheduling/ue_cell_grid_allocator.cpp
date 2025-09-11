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

#include "ue_cell_grid_allocator.h"
#include "../support/dci_builder.h"
#include "../ue_context/ue_drx_controller.h"
#include "grant_params_selector.h"
#include "srsran/scheduler/result/dci_info.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

ue_cell_grid_allocator::ue_cell_grid_allocator(const scheduler_ue_expert_config& expert_cfg_,
                                               ue_repository&                    ues_,
                                               pdcch_resource_allocator&         pdcch_sched_,
                                               uci_allocator&                    uci_alloc_,
                                               cell_resource_allocator&          cell_alloc_,
                                               srslog::basic_logger&             logger_) :
  expert_cfg(expert_cfg_),
  ues(ues_),
  pdcch_sched(pdcch_sched_),
  uci_alloc(uci_alloc_),
  cell_alloc(cell_alloc_),
  logger(logger_)
{
  dl_grants.reserve(MAX_UE_PDUS_PER_SLOT);
  ul_grants.reserve(MAX_PUSCH_PDUS_PER_SLOT);
}

std::optional<sch_mcs_tbs> ue_cell_grid_allocator::calculate_dl_mcs_tbs(cell_slot_resource_allocator& pdsch_alloc,
                                                                        const search_space_info&      ss_info,
                                                                        uint8_t pdsch_td_res_index,
                                                                        std::pair<crb_interval, crb_interval> crbs,
                                                                        sch_mcs_index                         mcs,
                                                                        unsigned nof_layers)
{
  // Reduce estimated MCS by 1 whenever CSI-RS is sent over a particular slot to account for the overhead of CSI-RS
  // REs.
  const auto&   pdsch_cfg = ss_info.get_pdsch_config(pdsch_td_res_index, nof_layers);
  sch_mcs_index adjusted_mcs{mcs};
  if (not pdsch_alloc.result.dl.csi_rs.empty()) {
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

  // If it's a new Tx, compute the MCS and TBS.
  // As \c txDirectCurrentLocation, in \c SCS-SpecificCarrier, TS 38.331, "If this field (\c
  // txDirectCurrentLocation) is absent for downlink within ServingCellConfigCommon and ServingCellConfigCommonSIB,
  // the UE assumes the default value of 3300 (i.e. "Outside the carrier")".
  bool                           contains_dc = false;
  const std::optional<unsigned>& tx_dc =
      cell_alloc.cfg.dl_cfg_common.freq_info_dl.scs_carrier_list.back().tx_direct_current_location;
  if (tx_dc.has_value()) {
    contains_dc = dc_offset_helper::is_contained(tx_dc.value(), crbs.first) ||
                  dc_offset_helper::is_contained(tx_dc.value(), crbs.second);
  }
  auto mcs_tbs_info =
      compute_dl_mcs_tbs(pdsch_cfg, adjusted_mcs, crbs.first.length() + crbs.second.length(), contains_dc);

  return mcs_tbs_info;
}

expected<pdcch_dl_information*, alloc_status> ue_cell_grid_allocator::alloc_dl_pdcch(const ue_cell&           ue_cc,
                                                                                     const search_space_info& ss_info)
{
  const rnti_t crnti = ue_cc.rnti();

  const aggregation_level aggr_lvl =
      ue_cc.get_aggregation_level(ue_cc.link_adaptation_controller().get_wideband_cqi(), ss_info, true);

  cell_slot_resource_allocator& pdcch_alloc = cell_alloc[0];
  pdcch_dl_information*         pdcch =
      pdcch_sched.alloc_dl_pdcch_ue(pdcch_alloc, crnti, ue_cc.cfg(), ss_info.cfg->get_id(), aggr_lvl);
  if (pdcch == nullptr) {
    logger.info(
        "ue={} rnti={}: Failed to allocate PDSCH. Cause: No space in PDCCH.", fmt::underlying(ue_cc.ue_index), crnti);
    // Note: (Implementation-defined) Assuming all UEs share the same CORESET, if there are no more CCEs left in the
    // CORESET, stop attempting to allocate new PDCCHs in the slot.
    unsigned nof_cces_left = ss_info.coreset->get_nof_cces();
    for (const auto& dl_pdcch : pdcch_alloc.result.dl.dl_pdcchs) {
      nof_cces_left -= std::min(nof_cces_left, to_nof_cces(dl_pdcch.ctx.cces.aggr_lvl));
    }
    for (const auto& ul_pdcch : pdcch_alloc.result.dl.ul_pdcchs) {
      nof_cces_left -= std::min(nof_cces_left, to_nof_cces(ul_pdcch.ctx.cces.aggr_lvl));
    }
    return make_unexpected(nof_cces_left == 0 ? alloc_status::skip_slot : alloc_status::skip_ue);
  }

  return pdcch;
}

std::optional<uci_allocation>
ue_cell_grid_allocator::alloc_uci(const ue_cell& ue_cc, const search_space_info& ss_info, uint8_t pdsch_td_res_index)
{
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = ss_info.pdsch_time_domain_list[pdsch_td_res_index];

  // Allocate UCI. UCI destination (i.e., PUCCH or PUSCH) depends on whether there exist a PUSCH grant for the UE.
  span<const uint8_t>           k1_list = ss_info.get_k1_candidates();
  std::optional<uci_allocation> uci =
      uci_alloc.alloc_uci_harq_ue(cell_alloc, ue_cc.rnti(), ue_cc.cfg(), pdsch_td_cfg.k0, k1_list, nullptr);
  if (not uci.has_value()) {
    logger.debug("ue={} rnti={}: Failed to allocate PDSCH. Cause: UCI allocation failed.",
                 fmt::underlying(ue_cc.ue_index),
                 ue_cc.rnti());
  }
  return uci;
}

expected<ue_cell_grid_allocator::dl_newtx_grant_builder, dl_alloc_failure_cause>
ue_cell_grid_allocator::allocate_dl_grant(const ue_newtx_dl_grant_request& request)
{
  // Select PDCCH searchSpace and PDSCH time-domain resource config.
  auto sched_ctxt = sched_helper::get_newtx_dl_sched_context(
      request.user, cell_alloc[0].slot, request.pdsch_slot, request.interleaving_enabled, request.pending_bytes);
  if (not sched_ctxt.has_value()) {
    // No valid parameters were found for this UE.
    return make_unexpected(dl_alloc_failure_cause::other);
  }

  // Setup a DL grant.
  auto result = setup_dl_grant_builder(request.user, sched_ctxt.value(), std::nullopt, request.pending_bytes);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }

  // Add DL grant to list of pending grants and create a newTx DL grant builder.
  dl_grants.push_back(*result);
  return dl_newtx_grant_builder{*this, static_cast<unsigned>(dl_grants.size()) - 1};
}

expected<ue_cell_grid_allocator::dl_grant_info, dl_alloc_failure_cause>
ue_cell_grid_allocator::setup_dl_grant_builder(const slice_ue&                       user,
                                               const sched_helper::dl_sched_context& params,
                                               std::optional<dl_harq_process_handle> h_dl,
                                               unsigned                              pending_bytes)
{
  const bool            is_retx            = h_dl.has_value();
  const search_space_id ss_id              = params.ss_id;
  const uint8_t         pdsch_td_res_index = params.pdsch_td_res_index;

  // Derive remaining parameters from \c dl_grant_params.
  ue&                                          u            = ues[user.ue_index()];
  ue_cell&                                     ue_cc        = *u.find_cell(cell_alloc.cell_index());
  const ue_cell_configuration&                 ue_cell_cfg  = ue_cc.cfg();
  const search_space_info&                     ss_info      = ue_cell_cfg.search_space(ss_id);
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = ss_info.pdsch_time_domain_list[pdsch_td_res_index];

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = cell_alloc[0];
  cell_slot_resource_allocator& pdsch_alloc = cell_alloc[pdsch_td_cfg.k0];
  srsran_sanity_check(not pdcch_alloc.result.dl.dl_pdcchs.full(), "No space available for PDCCH");
  srsran_sanity_check(pdsch_alloc.result.dl.bc.sibs.size() + pdsch_alloc.result.dl.paging_grants.size() +
                              pdsch_alloc.result.dl.rar_grants.size() + pdsch_alloc.result.dl.ue_grants.size() <
                          expert_cfg.max_pdschs_per_slot,
                      "Max number of PDSCHs per slot was reached");
  srsran_sanity_check(not pdsch_alloc.result.dl.ue_grants.full(), "No space available in scheduler PDSCH outputs");

  // Allocate PDCCH.
  auto pdcch_result = alloc_dl_pdcch(ue_cc, ss_info);
  if (not pdcch_result.has_value()) {
    ++pdcch_alloc.result.failed_attempts.pdcch;
    return make_unexpected(dl_alloc_failure_cause::pdcch_alloc_failed);
  }
  pdcch_dl_information* pdcch = pdcch_result.value();

  // Allocate UCI.
  auto uci_result = alloc_uci(ue_cc, ss_info, pdsch_td_res_index);
  if (not uci_result.has_value()) {
    ++pdcch_alloc.result.failed_attempts.uci;
    pdcch_sched.cancel_last_pdcch(pdcch_alloc);
    return make_unexpected(dl_alloc_failure_cause::uci_alloc_failed);
  }
  uci_allocation& uci                     = uci_result.value();
  unsigned        k1                      = uci.k1;
  pdcch->ctx.context.harq_feedback_timing = k1;

  // Allocate UE DL HARQ.
  if (not is_retx) {
    // It is a new tx.
    h_dl = ue_cc.harqs
               .alloc_dl_harq(pdsch_alloc.slot,
                              k1 + ue_cell_cfg.cell_cfg_common.ntn_cs_koffset,
                              expert_cfg.max_nof_dl_harq_retxs,
                              uci.harq_bit_idx)
               .value();
    srsran_assert(h_dl.has_value(), "Failed to allocate DL HARQ");
  } else {
    // It is a retx.
    bool result = h_dl->new_retx(pdsch_alloc.slot, k1 + ue_cell_cfg.cell_cfg_common.ntn_cs_koffset, uci.harq_bit_idx);
    srsran_assert(result, "Harq is in invalid state");
  }

  // Create PDSCH PDU.
  auto& msg = pdsch_alloc.result.dl.ue_grants.emplace_back();

  // Create a DL grant builder.
  return dl_grant_info{&user, params, h_dl.value(), pdcch, &msg, uci, pending_bytes};
}

void ue_cell_grid_allocator::set_pdsch_params(dl_grant_info&                        grant,
                                              vrb_interval                          vrbs,
                                              std::pair<crb_interval, crb_interval> crbs,
                                              bool                                  enable_interleaving)
{
  // Derive remaining parameters from \c dl_grant_params.
  ue&                                          u                  = ues[grant.user->ue_index()];
  ue_cell&                                     ue_cc              = *u.find_cell(cell_alloc.cell_index());
  const ue_cell_configuration&                 ue_cell_cfg        = ue_cc.cfg();
  const search_space_info&                     ss_info            = ue_cell_cfg.search_space(grant.cfg.ss_id);
  uint8_t                                      pdsch_td_res_index = grant.cfg.pdsch_td_res_index;
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg       = ss_info.pdsch_time_domain_list[pdsch_td_res_index];
  const subcarrier_spacing                     scs                = ss_info.bwp->dl_common->value().generic_params.scs;
  const cell_configuration&                    cell_cfg           = ue_cell_cfg.cell_cfg_common;
  const bool                                   is_retx            = grant.h_dl.nof_retxs() != 0;
  const unsigned                               nof_layers         = grant.cfg.recommended_ri;
  const sch_mcs_index                          mcs                = grant.cfg.recommended_mcs;
  const auto&                                  pdsch_cfg = ss_info.get_pdsch_config(pdsch_td_res_index, nof_layers);
  const unsigned                               k1        = grant.uci_alloc.k1;

  // Fetch PDCCH and PDSCH resource grid allocators.

  cell_slot_resource_allocator& pdcch_alloc = cell_alloc[0];
  cell_slot_resource_allocator& pdsch_alloc = cell_alloc[pdsch_td_cfg.k0];

  srsran_sanity_check(not(pdsch_alloc.dl_res_grid.collides(scs, pdsch_td_cfg.symbols, crbs.first) or
                          pdsch_alloc.dl_res_grid.collides(scs, pdsch_td_cfg.symbols, crbs.second)),
                      "Invalid calculation of PDSCH RBs. Used CRBs={:i}. Allocated CRBs={}{}.",
                      pdsch_alloc.dl_res_grid.used_crbs(scs, {0, cell_cfg.nof_dl_prbs}, pdsch_td_cfg.symbols),
                      crbs.first,
                      crbs.second);

  sch_mcs_tbs mcs_tbs_info;
  if (is_retx) {
    // It is a reTx.
    mcs_tbs_info = {grant.h_dl.get_grant_params().mcs, grant.h_dl.get_grant_params().tbs_bytes};
  } else {
    // It is a newTx.
    auto mcs_or_error = calculate_dl_mcs_tbs(pdsch_alloc, ss_info, pdsch_td_res_index, crbs, mcs, nof_layers);
    if (not mcs_or_error.has_value()) {
      // We give up on more reTxs of this HARQ.
      grant.h_dl.cancel_retxs();

      logger.warning("ue={} rnti={}: Failed to derive MCS for PUSCH. Cause: no MCS such that code rate <= 0.95 with "
                     "provided configuration",
                     fmt::underlying(u.ue_index),
                     u.crnti);
    }
    mcs_tbs_info = mcs_or_error.value_or(sch_mcs_tbs{sch_mcs_index{0}, 0});
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, crbs.first});
  if (not crbs.second.empty()) {
    pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, crbs.second});
  }

  // Compute TPC for PUCCH.
  uint8_t tpc = ue_cc.get_pucch_power_controller().compute_tpc_command(pdsch_alloc.slot + k1 +
                                                                       ue_cell_cfg.cell_cfg_common.ntn_cs_koffset);

  // Fill DL PDCCH DCI PDU.
  // Number of possible Downlink Assignment Indexes {0, ..., 3} as per TS38.213 Section 9.1.3.
  static constexpr unsigned DAI_MOD = 4U;
  uint8_t                   rv      = ue_cc.get_pdsch_rv(grant.h_dl.nof_retxs());
  // For allocation on PUSCH, we use a PUCCH resource indicator set to 0, as it will get ignored by the UE.
  const unsigned pucch_res_indicator = grant.uci_alloc.pucch_res_indicator.value_or(0);
  switch (ss_info.get_dl_dci_format()) {
    case dci_dl_format::f1_0: {
      build_dci_f1_0_c_rnti(grant.pdcch->dci,
                            ss_info,
                            cell_cfg.dl_cfg_common.init_dl_bwp,
                            vrbs,
                            pdsch_td_res_index,
                            k1,
                            pucch_res_indicator,
                            grant.uci_alloc.harq_bit_idx % DAI_MOD,
                            mcs_tbs_info.mcs,
                            rv,
                            grant.h_dl);
    } break;
    case dci_dl_format::f1_1:
      build_dci_f1_1_c_rnti(grant.pdcch->dci,
                            ue_cell_cfg,
                            ss_info.cfg->get_id(),
                            vrbs,
                            pdsch_td_res_index,
                            k1,
                            pucch_res_indicator,
                            grant.uci_alloc.harq_bit_idx % DAI_MOD,
                            mcs_tbs_info.mcs,
                            rv,
                            grant.h_dl,
                            nof_layers,
                            tpc,
                            enable_interleaving);
      break;
    default:
      report_fatal_error("Unsupported RNTI type for PDSCH allocation");
  }

  // Fill PDSCH PDU.
  dl_msg_alloc& msg            = *grant.pdsch;
  msg.context.ue_index         = u.ue_index;
  msg.context.k1               = k1;
  msg.context.ss_id            = grant.cfg.ss_id;
  msg.context.nof_retxs        = grant.h_dl.nof_retxs();
  msg.context.buffer_occupancy = 0; // We fill this value later, after the TB is built.
  if (not is_retx and ue_cc.link_adaptation_controller().is_dl_olla_enabled()) {
    msg.context.olla_offset = ue_cc.link_adaptation_controller().dl_cqi_offset();
  }
  switch (grant.pdcch->dci.type) {
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      build_pdsch_f1_0_c_rnti(msg.pdsch_cfg,
                              pdsch_cfg,
                              mcs_tbs_info.tbs,
                              u.crnti,
                              cell_cfg,
                              ss_info,
                              grant.pdcch->dci.c_rnti_f1_0,
                              vrbs,
                              not is_retx);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_1:
      build_pdsch_f1_1_c_rnti(msg.pdsch_cfg,
                              pdsch_cfg,
                              mcs_tbs_info,
                              u.crnti,
                              ue_cell_cfg,
                              grant.cfg.ss_id,
                              grant.pdcch->dci.c_rnti_f1_1,
                              vrbs,
                              not is_retx,
                              ue_cc.channel_state_manager());
      break;
    default:
      report_fatal_error("Unsupported PDCCH DL DCI format");
  }

  // Save set PDCCH and PDSCH PDU parameters in HARQ process.
  dl_harq_alloc_context pdsch_sched_ctx;
  pdsch_sched_ctx.dci_cfg_type = grant.pdcch->dci.type;
  if (not is_retx) {
    pdsch_sched_ctx.olla_mcs =
        ue_cc.link_adaptation_controller().calculate_dl_mcs(msg.pdsch_cfg.codewords[0].mcs_table);
    pdsch_sched_ctx.slice_id = grant.user->ran_slice_id();
  }
  pdsch_sched_ctx.cqi = ue_cc.channel_state_manager().get_wideband_cqi();

  if (not is_retx) {
    // Set MAC logical channels to schedule in this PDU if it is a newtx.
    u.build_dl_transport_block_info(
        msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes, grant.user->ran_slice_id());

    // Update context with buffer occupancy after the TB is built.
    msg.context.buffer_occupancy = u.pending_dl_newtx_bytes();
  }

  // Save PDSCH parameters in DL HARQ.
  grant.h_dl.save_grant_params(pdsch_sched_ctx, msg);

  // Update DRX state given the new allocation.
  u.drx_controller().on_new_dl_pdcch_alloc(pdcch_alloc.slot);
}

expected<vrb_interval, dl_alloc_failure_cause>
ue_cell_grid_allocator::allocate_dl_grant(const ue_retx_dl_grant_request& request)
{
  // Select PDCCH searchSpace and PDSCH time-domain resource config.
  auto sched_ctxt = sched_helper::get_retx_dl_sched_context(
      request.user, cell_alloc[0].slot, request.pdsch_slot, request.interleaving_enabled, request.h_dl);
  if (not sched_ctxt) {
    return make_unexpected(dl_alloc_failure_cause::other);
  }

  // Select DL CRBs.
  const vrb_interval vrbs = sched_helper::compute_retx_dl_vrbs(sched_ctxt.value(), request.used_dl_vrbs);
  if (vrbs.empty()) {
    return make_unexpected(dl_alloc_failure_cause::pdcch_alloc_failed);
  }

  // Allocate PDCCH, PDSCH and UCI PDUs.
  auto grant = setup_dl_grant_builder(request.user, sched_ctxt.value(), request.h_dl, 0);
  if (not grant.has_value()) {
    return make_unexpected(grant.error());
  }

  // Compute the corresponding CRBs.
  static constexpr search_space_id      ue_ded_ss_id = to_search_space_id(2);
  const auto&                           ss_info      = request.user.get_cc().cfg().search_space(ue_ded_ss_id);
  std::pair<crb_interval, crb_interval> crbs;
  if (request.interleaving_enabled) {
    const auto prbs = ss_info.interleaved_mapping.value().vrb_to_prb(vrbs);
    crbs            = {prb_to_crb(ss_info.dl_crb_lims, prbs.first), prb_to_crb(ss_info.dl_crb_lims, prbs.second)};
  } else {
    crbs = {prb_to_crb(ss_info.dl_crb_lims, vrbs.convert_to<prb_interval>()), {}};
  }

  // Set PDSCH parameters.
  set_pdsch_params(grant.value(), vrbs, crbs, request.interleaving_enabled);
  return vrbs;
}

expected<ue_cell_grid_allocator::ul_newtx_grant_builder, alloc_status>
ue_cell_grid_allocator::allocate_ul_grant(const ue_newtx_ul_grant_request& request)
{
  unsigned pending_uci_harq_bits =
      uci_alloc.get_scheduled_pdsch_counter_in_ue_uci(request.pusch_slot, request.user.crnti());

  // Select PDCCH searchSpace and PUSCH time-domain resource config.
  auto sched_ctxt = sched_helper::get_newtx_ul_sched_context(
      request.user, cell_alloc[0].slot, request.pusch_slot, pending_uci_harq_bits, request.pending_bytes);
  if (not sched_ctxt.has_value()) {
    // No valid parameters were found for this UE.
    return make_unexpected(alloc_status::skip_ue);
  }

  // Setup a UL grant.
  auto result = setup_ul_grant_builder(request.user, sched_ctxt.value(), std::nullopt, request.pending_bytes);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }

  // Add UL grant to list of pending grants and create a newTx DL grant builder.
  ul_grants.push_back(*result);
  return ul_newtx_grant_builder{*this, (unsigned)ul_grants.size() - 1};
}

expected<vrb_interval, alloc_status> ue_cell_grid_allocator::allocate_ul_grant(const ue_retx_ul_grant_request& request)
{
  unsigned pending_uci_harq_bits =
      uci_alloc.get_scheduled_pdsch_counter_in_ue_uci(request.pusch_slot, request.user.crnti());

  // Select PDCCH searchSpace and PUSCH time-domain resource config.
  auto sched_ctxt = sched_helper::get_retx_ul_sched_context(
      request.user, cell_alloc[0].slot, request.pusch_slot, pending_uci_harq_bits, request.h_ul);
  if (not sched_ctxt) {
    return make_unexpected(alloc_status::skip_ue);
  }

  // Select UL CRBs.
  vrb_interval vrbs = sched_helper::compute_retx_ul_vrbs(sched_ctxt.value(), request.used_ul_vrbs);
  if (vrbs.empty()) {
    return make_unexpected(alloc_status::skip_ue);
  }

  // Allocate PDCCH, PUSCH PDUs.
  auto grant = setup_ul_grant_builder(request.user, sched_ctxt.value(), request.h_ul, 0);
  if (not grant.has_value()) {
    return make_unexpected(grant.error());
  }

  // Set PUSCH parameters.
  set_pusch_params(grant.value(), vrbs);

  return vrbs;
}

expected<ue_cell_grid_allocator::ul_grant_info, alloc_status>
ue_cell_grid_allocator::setup_ul_grant_builder(const slice_ue&                       user,
                                               const sched_helper::ul_sched_context& params,
                                               std::optional<ul_harq_process_handle> h_ul,
                                               unsigned                              pending_bytes)
{
  // Derive remaining parameters from \c ul_grant_params.
  ue&                                          u                  = ues[user.ue_index()];
  ue_cell&                                     ue_cc              = *u.find_cell(cell_alloc.cell_index());
  const ue_cell_configuration&                 ue_cell_cfg        = ue_cc.cfg();
  const search_space_info&                     ss_info            = ue_cell_cfg.search_space(params.ss_id);
  const search_space_configuration&            ss_cfg             = *ss_info.cfg;
  const uint8_t                                pusch_td_res_index = params.pusch_td_res_index;
  const pusch_time_domain_resource_allocation& pusch_td_cfg       = ss_info.pusch_time_domain_list[pusch_td_res_index];
  const bool                                   is_retx            = h_ul.has_value();

  // Fetch PDCCH and PUSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = cell_alloc[0];
  const unsigned                final_k2    = pusch_td_cfg.k2 + cell_alloc.cfg.ntn_cs_koffset;
  cell_slot_resource_allocator& pusch_alloc = cell_alloc[final_k2];
  srsran_sanity_check(not pdcch_alloc.result.dl.ul_pdcchs.full(), "Maximum number of UL PDCCH grants per slot reached");
  srsran_sanity_check(not pusch_alloc.result.ul.puschs.full(), "No PUSCH space available in scheduler output list");

  // [Implementation-defined] We skip allocation of PUSCH if there is already a PUCCH grant scheduled using common
  // PUCCH resources.
  if (uci_alloc.has_uci_harq_on_common_pucch_res(u.crnti, pusch_alloc.slot)) {
    logger.debug("ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: UE has PUCCH grant using common PUCCH "
                 "resources scheduled",
                 fmt::underlying(u.ue_index),
                 u.crnti,
                 pusch_alloc.slot);
    return make_unexpected(alloc_status::skip_ue);
  }

  // Allocate PDCCH position.
  const aggregation_level aggr_lvl =
      ue_cc.get_aggregation_level(ue_cc.link_adaptation_controller().get_effective_cqi(), ss_info, false);
  pdcch_ul_information* pdcch =
      pdcch_sched.alloc_ul_pdcch_ue(pdcch_alloc, u.crnti, ue_cell_cfg, ss_cfg.get_id(), aggr_lvl);
  if (pdcch == nullptr) {
    logger.info(
        "ue={} rnti={}: Failed to allocate PUSCH. Cause: No space in PDCCH.", fmt::underlying(u.ue_index), u.crnti);
    // Note: (Implementation-defined) Assuming all UEs share the same CORESET, if there are no more CCEs left in the
    // CORESET, stop attempting to allocate new PDCCHs in the slot.
    unsigned nof_cces_left = ss_info.coreset->get_nof_cces();
    for (const auto& dl_pdcch : pdcch_alloc.result.dl.dl_pdcchs) {
      nof_cces_left -= std::min(nof_cces_left, to_nof_cces(dl_pdcch.ctx.cces.aggr_lvl));
    }
    for (const auto& ul_pdcch : pdcch_alloc.result.dl.ul_pdcchs) {
      nof_cces_left -= std::min(nof_cces_left, to_nof_cces(ul_pdcch.ctx.cces.aggr_lvl));
    }
    return make_unexpected(nof_cces_left == 0 ? alloc_status::skip_slot : alloc_status::skip_ue);
  }

  // Allocate UE UL HARQ.
  if (not is_retx) {
    // It is a new tx.
    h_ul = ue_cc.harqs.alloc_ul_harq(pusch_alloc.slot, expert_cfg.max_nof_ul_harq_retxs).value();
    srsran_assert(h_ul.has_value(), "Failed to allocate UL HARQ");
  } else {
    // It is a retx.
    bool result = h_ul->new_retx(pusch_alloc.slot);
    srsran_assert(result, "Failed to allocate HARQ retx");
  }

  // Create PUSCH PDU.
  auto& msg = pusch_alloc.result.ul.puschs.emplace_back();

  // Create a UL grant builder.
  return ul_grant_info{&user, params, h_ul.value(), pdcch, &msg, pending_bytes};
}

void ue_cell_grid_allocator::set_pusch_params(ul_grant_info& grant, const vrb_interval& vrbs)
{
  srsran_assert(not vrbs.empty(), "Invalid set of PUSCH VRBs");

  ue&      u     = ues[grant.user->ue_index()];
  ue_cell& ue_cc = *u.find_cell(cell_alloc.cell_index());

  const ue_cell_configuration& ue_cell_cfg = ue_cc.cfg();
  const cell_configuration&    cell_cfg    = ue_cell_cfg.cell_cfg_common;

  // Derive remaining parameters from \c ul_grant_params.
  const search_space_info&                     ss_info            = ue_cell_cfg.search_space(grant.cfg.ss_id);
  const search_space_configuration&            ss_cfg             = *ss_info.cfg;
  const bwp_uplink_common&                     bwp_ul_cmn         = *ss_info.bwp->ul_common.value();
  const subcarrier_spacing                     scs                = bwp_ul_cmn.generic_params.scs;
  const dci_ul_rnti_config_type                dci_type           = ss_info.get_ul_dci_format() == dci_ul_format::f0_0
                                                                        ? dci_ul_rnti_config_type::c_rnti_f0_0
                                                                        : dci_ul_rnti_config_type::c_rnti_f0_1;
  uint8_t                                      pusch_td_res_index = grant.cfg.pusch_td_res_index;
  const pusch_time_domain_resource_allocation& pusch_td_cfg       = ss_info.pusch_time_domain_list[pusch_td_res_index];
  const pusch_config_params&                   pusch_cfg          = grant.cfg.pusch_cfg;
  const bool                                   is_retx            = grant.h_ul.nof_retxs() != 0;

  // Fetch PDCCH and PUSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = cell_alloc[0];
  const unsigned                final_k2    = pusch_td_cfg.k2 + cell_cfg.ntn_cs_koffset;
  cell_slot_resource_allocator& pusch_alloc = cell_alloc[final_k2];

  // Compute exact MCS and TBS for this transmission.
  expected<sch_mcs_tbs, compute_ul_mcs_tbs_error> mcs_tbs_info;
  // TODO: find TS reference for -> Since, PUSCH always uses non interleaved mapping, prbs = vrbs.
  const auto crbs = prb_to_crb(ss_info.ul_crb_lims, vrbs.convert_to<prb_interval>());
  if (not is_retx) {
    // If it's a new Tx, compute the MCS and TBS from SNR, payload size, and available RBs.

    // Note: Even if at this point we can determine if the RBs intersect the DC location (via
    // dc_offset_helper::is_contained), we take the conservative approach and assume that the DC location is always
    // contained in the RBs. This may slightly reduce the MCS beyond what's needed to satisfy the effective code rate
    // limits. We do this to simplify the search for available RBs during HARQ retxs.
    const bool contains_dc = true;
    mcs_tbs_info =
        compute_ul_mcs_tbs(pusch_cfg, ue_cc.active_bwp(), grant.cfg.recommended_mcs, vrbs.length(), contains_dc);

    // If there is not MCS-TBS info, it means no MCS exists such that the effective code rate is <= 0.95.
    if (not mcs_tbs_info.has_value()) {
      // We cannot cancel this allocation at this point. So, we will proceed as normal, but set an invalid coderate.
      mcs_tbs_info.emplace();
      mcs_tbs_info->mcs = 0;
      mcs_tbs_info->tbs = compute_ul_tbs_unsafe(pusch_cfg, grant.cfg.recommended_mcs, vrbs.length());

      logger.warning(
          "ue={} rnti={}: Failed to allocate PUSCH. Cause: {} with this "
          "configuration: mcs={} vrbs={} symbols={} nof_oh={} tb-sc-field={} layers={} pi2bpsk={} "
          "harq_bits={} csi1_bits={} csi2_bits={} mcs_table_idx={} dmrs_A_pos={} is_dmrs_type2={} dmrs_add_pos_idx={}",
          fmt::underlying(u.ue_index),
          u.crnti,
          to_string(mcs_tbs_info.error()),
          grant.cfg.recommended_mcs,
          vrbs,
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
          ue_cell_cfg.init_bwp().ul_ded->pusch_cfg->pusch_mapping_type_a_dmrs.value().is_dmrs_type2 ? "yes" : "no",
          static_cast<unsigned>(
              ue_cell_cfg.init_bwp().ul_ded->pusch_cfg->pusch_mapping_type_a_dmrs.value().additional_positions));
    }

  } else {
    // If it's a reTx, fetch the MCS, TBS and number of layers from the previous transmission.
    const auto& prev_params = grant.h_ul.get_grant_params();
    mcs_tbs_info.emplace(sch_mcs_tbs{.mcs = prev_params.mcs, .tbs = prev_params.tbs_bytes});
    srsran_sanity_check(prev_params.mcs_table == pusch_cfg.mcs_table, "MCS table cannot change across HARQ reTxs");
  }

  srsran_sanity_check(not pusch_alloc.ul_res_grid.collides(scs, pusch_td_cfg.symbols, crbs),
                      "Invalid calculation of PUSCH RBs. Used CRBs={:i}. Allocated CRBs={}.",
                      pusch_alloc.ul_res_grid.used_crbs(scs, {0, cell_cfg.nof_ul_prbs}, pusch_td_cfg.symbols),
                      crbs);

  // Mark resources as occupied in the ResourceGrid.
  pusch_alloc.ul_res_grid.fill(grant_info{scs, pusch_td_cfg.symbols, crbs});

  // Compute total DAI. See TS 38.213, 9.1.3.2.
  // Note: We assume k2 <= k1, which means that all the HARQ bits are set at this point for this UL slot and UE.
  // Total DAI provides total number of transmissions at the end of each interval (slot in a cell). Values range from
  // 1 to 4. If a UE is not provided PDSCH-CodeBlockGroupTransmission and the UE is scheduled for a PUSCH transmission
  // by DCI format 0_1 with DAI field value V_T_DAI_UL = 4 and the UE has not received any PDCCH within the monitoring
  // occasions for PDCCH with DCI format 1_0 or DCI format 1_1 for scheduling PDSCH receptions or SPS PDSCH
  // release on any serving cell c and the UE does not have HARQ-ACK information in response to a SPS PDSCH
  // reception to multiplex in the PUSCH, the UE does not multiplex HARQ-ACK information in the PUSCH transmission.
  // NOTE: DAI is encoded as per left most column in Table 9.1.3-2 of TS 38.213.
  unsigned dai = 3;
  if (dci_type == dci_ul_rnti_config_type::c_rnti_f0_1) {
    unsigned total_harq_ack_in_uci = pusch_cfg.nof_harq_ack_bits;
    if (total_harq_ack_in_uci != 0) {
      // See TS 38.213, Table 9.1.3-2. dai value below maps to the leftmost column in the table.
      dai = ((total_harq_ack_in_uci - 1) % 4);
    }
  }

  // Compute TPC command before computing the nof_prbs adaptation based on PHR; this is because, when the TPC gets
  // computed, the channel state manager will update close-loop power control adjustment.
  const uint8_t tpc_command = ue_cc.get_pusch_power_controller().compute_tpc_command(
      pdcch_alloc.slot + pusch_td_cfg.k2 + cell_cfg.ntn_cs_koffset);

  // Fill UL PDCCH DCI.
  uint8_t rv = ue_cc.get_pusch_rv(grant.h_ul.nof_retxs());
  switch (dci_type) {
    case dci_ul_rnti_config_type::c_rnti_f0_0:
      build_dci_f0_0_c_rnti(grant.pdcch->dci,
                            ss_info,
                            cell_cfg.ul_cfg_common.init_ul_bwp,
                            vrbs,
                            grant.cfg.pusch_td_res_index,
                            mcs_tbs_info.value().mcs,
                            rv,
                            grant.h_ul,
                            tpc_command);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_1:
      build_dci_f0_1_c_rnti(grant.pdcch->dci,
                            ue_cell_cfg,
                            grant.cfg.ss_id,
                            vrbs,
                            grant.cfg.pusch_td_res_index,
                            mcs_tbs_info.value().mcs,
                            rv,
                            grant.h_ul,
                            dai,
                            pusch_cfg.nof_layers,
                            ue_cc.channel_state_manager().get_recommended_pusch_tpmi(pusch_cfg.nof_layers),
                            tpc_command);
      break;
    default:
      report_fatal_error("Unsupported PDCCH UL DCI format");
  }

  // Fill PUSCH PDU.
  ul_sched_info& msg     = *grant.pusch;
  msg.context.ue_index   = u.ue_index;
  msg.context.ss_id      = grant.cfg.ss_id;
  msg.context.k2         = final_k2;
  msg.context.nof_retxs  = grant.h_ul.nof_retxs();
  msg.context.nof_oh_prb = pusch_cfg.nof_oh_prb;
  if (not is_retx and ue_cc.link_adaptation_controller().is_ul_olla_enabled()) {
    msg.context.olla_offset = ue_cc.link_adaptation_controller().ul_snr_offset_db();
  }
  switch (grant.pdcch->dci.type) {
    case dci_ul_rnti_config_type::tc_rnti_f0_0:
      build_pusch_f0_0_tc_rnti(msg.pusch_cfg,
                               pusch_cfg,
                               mcs_tbs_info.value().tbs,
                               u.crnti,
                               cell_cfg,
                               grant.pdcch->dci.tc_rnti_f0_0,
                               vrbs,
                               not is_retx);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_0:
      build_pusch_f0_0_c_rnti(msg.pusch_cfg,
                              u.crnti,
                              pusch_cfg,
                              mcs_tbs_info.value().tbs,
                              cell_cfg,
                              bwp_ul_cmn,
                              grant.pdcch->dci.c_rnti_f0_0,
                              vrbs,
                              not is_retx);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_1:
      build_pusch_f0_1_c_rnti(msg.pusch_cfg,
                              u.crnti,
                              pusch_cfg,
                              mcs_tbs_info.value(),
                              ue_cell_cfg,
                              ss_cfg.get_id(),
                              grant.pdcch->dci.c_rnti_f0_1,
                              vrbs,
                              not is_retx);
      break;
    default:
      report_fatal_error("Unsupported PDCCH UL DCI format");
  }

  // Check if there is any UCI grant allocated on the PUCCH that can be moved to the PUSCH.
  uci_alloc.multiplex_uci_on_pusch(msg, pusch_alloc, ue_cell_cfg, u.crnti);

  // Save set PDCCH and PUSCH PDU parameters in HARQ process.
  ul_harq_alloc_context pusch_sched_ctx;
  pusch_sched_ctx.dci_cfg_type = grant.pdcch->dci.type;
  if (not is_retx) {
    pusch_sched_ctx.olla_mcs =
        ue_cc.link_adaptation_controller().calculate_ul_mcs(pusch_cfg.mcs_table, pusch_cfg.use_transform_precoder);
    pusch_sched_ctx.slice_id = grant.user->ran_slice_id();
  }

  grant.h_ul.save_grant_params(pusch_sched_ctx, msg.pusch_cfg);

  // Register UL allocations for this slot.
  u.handle_ul_transport_block_info(grant.h_ul.get_grant_params().tbs_bytes);

  // Update DRX state given the new allocation.
  u.drx_controller().on_new_ul_pdcch_alloc(pdcch_alloc.slot, pusch_alloc.slot);

  // Update the number of PRBs used in the PUSCH allocation.
  ue_cc.get_pusch_power_controller().update_pusch_pw_ctrl_state(pusch_alloc.slot, vrbs.length());
}

void ue_cell_grid_allocator::post_process_results()
{
  auto& slot_alloc = cell_alloc[0];

  // Update the PUCCH power control data.
  post_process_pucch_pw_ctrl_results(slot_alloc.slot);

  dl_grants.clear();
  ul_grants.clear();
}

void ue_cell_grid_allocator::post_process_pucch_pw_ctrl_results(slot_point slot)
{
  if (not cell_alloc.cfg.is_ul_enabled(slot)) {
    return;
  }

  auto& slot_alloc = cell_alloc[slot];

  // Spans through the PUCCH grant list and update the HARQ-ACK PUCCH grant counter for the corresponding RNTI and HARQ
  // process id.
  for (const auto& pucch : slot_alloc.result.ul.pucchs) {
    ue* user = ues.find_by_rnti(pucch.crnti);
    // This is to handle the case of a UE that gets removed after the PUCCH gets allocated and before this PUCCH is
    // expected to be sent.
    if (user == nullptr) {
      logger.warning(
          "rnti={}: No user with such RNTI found in the ue scheduler database. Skipping PUCCH power control update",
          pucch.crnti,
          slot_alloc.slot);
      continue;
    }

    pucch_uci_bits pucch_uci_bits;
    // pi_2_bpsk, additional_dmrs and intraslot_freq_hopping are only used for PUCCH format 3 and 4.
    bool pi_2_bpsk              = false;
    bool additional_dmrs        = false;
    bool intraslot_freq_hopping = false;

    pucch_uci_bits.harq_ack_nof_bits  = pucch.uci_bits.harq_ack_nof_bits;
    pucch_uci_bits.sr_bits            = pucch.uci_bits.sr_bits;
    pucch_uci_bits.csi_part1_nof_bits = pucch.uci_bits.csi_part1_nof_bits;
    switch (pucch.format()) {
      case pucch_format::FORMAT_3: {
        const auto& format_3   = std::get<pucch_format_3>(pucch.format_params);
        pi_2_bpsk              = format_3.pi_2_bpsk;
        additional_dmrs        = format_3.additional_dmrs;
        intraslot_freq_hopping = not pucch.resources.second_hop_prbs.empty();
      } break;
      case pucch_format::FORMAT_4: {
        const auto& format_4   = std::get<pucch_format_4>(pucch.format_params);
        pi_2_bpsk              = format_4.pi_2_bpsk;
        additional_dmrs        = format_4.additional_dmrs;
        intraslot_freq_hopping = not pucch.resources.second_hop_prbs.empty();
      } break;
      default:
        break;
    }

    user->get_pcell().get_pucch_power_controller().update_pucch_pw_ctrl_state(slot_alloc.slot,
                                                                              pucch.format(),
                                                                              pucch.resources.prbs.length(),
                                                                              pucch.resources.symbols.length(),
                                                                              pucch_uci_bits,
                                                                              intraslot_freq_hopping,
                                                                              pi_2_bpsk,
                                                                              additional_dmrs);
  }
}

void ue_cell_grid_allocator::dl_newtx_grant_builder::set_pdsch_params(vrb_interval                          alloc_vrbs,
                                                                      std::pair<crb_interval, crb_interval> alloc_crbs,
                                                                      bool enable_interleaving)
{
  // Transfer the PDSCH parameters to the parent DL grant.
  parent->set_pdsch_params(parent->dl_grants[grant_index], alloc_vrbs, alloc_crbs, enable_interleaving);

  // Set PDSCH parameters and set parent as nullptr to avoid further modifications.
  parent = nullptr;
}

void ue_cell_grid_allocator::ul_newtx_grant_builder::set_pusch_params(const vrb_interval& alloc_vrbs)
{
  // Transfer the PUSCH parameters to the parent UL grant.
  parent->set_pusch_params(parent->ul_grants[grant_index], alloc_vrbs);

  // Set PUSCH parameters and set parent as nullptr to avoid further modifications.
  parent = nullptr;
}
