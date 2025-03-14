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
#include "../support/csi_report_helpers.h"
#include "../support/dci_builder.h"
#include "../support/mcs_calculator.h"
#include "../support/sched_result_helpers.h"
#include "../ue_context/ue_drx_controller.h"
#include "grant_params_selector.h"
#include "ue_pdsch_alloc_param_candidate_searcher.h"
#include "ue_pusch_alloc_param_candidate_searcher.h"

#include "srsran/ran/transform_precoding/transform_precoding_helpers.h"
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
}

ue_cell_grid_allocator::dl_grant_params
ue_cell_grid_allocator::get_dl_grant_params(const common_ue_dl_grant_request& sched_params)
{
  const slice_ue& slice_ue = *sched_params.user;
  srsran_sanity_check(ues.contains(slice_ue.ue_index()), "Invalid UE candidate index");
  const bool     is_retx = sched_params.h_dl.has_value();
  ue&            u       = ues[slice_ue.ue_index()];
  const ue_cell& ue_cc   = slice_ue.get_cc();
  srsran_sanity_check(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(), "Invalid UE state");

  const ue_cell_configuration&                 ue_cell_cfg = ue_cc.cfg();
  const cell_configuration&                    cell_cfg    = ue_cell_cfg.cell_cfg_common;
  const std::optional<dl_harq_process_handle>& h_dl        = sched_params.h_dl;

  // Fetch PDCCH resource grid allocator.
  cell_slot_resource_allocator& pdcch_alloc = cell_alloc[0];
  srsran_sanity_check(not pdcch_alloc.result.dl.dl_pdcchs.full(), "No space available for PDCCH");
  srsran_sanity_check(ue_cc.is_pdcch_enabled(pdcch_alloc.slot), "DL is not active in the PDCCH slot");

  // Create PDSCH param candidate search object.
  ue_pdsch_alloc_param_candidate_searcher candidates{
      u, cell_alloc.cell_index(), h_dl, pdcch_alloc.slot, sched_params.pdsch_slot};
  if (candidates.is_empty()) {
    // The conditions for a new PDSCH allocation for this UE were not met (e.g. lack of available SearchSpaces).
    return {alloc_status::skip_ue};
  }

  // Iterate through allocation parameter candidates.
  for (const ue_pdsch_alloc_param_candidate_searcher::candidate& param_candidate : candidates) {
    const pdsch_time_domain_resource_allocation& pdsch_td_cfg = param_candidate.pdsch_td_res();
    const search_space_info&                     ss_info      = param_candidate.ss();
    const dci_dl_rnti_config_type                dci_type     = param_candidate.dci_dl_rnti_cfg_type();

    // Fetch PDSCH resource grid allocator.
    cell_slot_resource_allocator& pdsch_alloc = cell_alloc[pdsch_td_cfg.k0];

    // Verify only one PDSCH exists for the same RNTI in the same slot, and that the PDSCHs are in the same order as
    // PDCCHs.
    // [TS 38.214, 5.1] "For any HARQ process ID(s) in a given scheduled cell, the UE is not expected to receive a
    // PDSCH that overlaps in time with another PDSCH".
    // [TS 38.214, 5.1] "For any two HARQ process IDs in a given scheduled cell, if the UE is scheduled to start
    // receiving a first PDSCH starting in symbol j by a PDCCH ending in symbol i, the UE is not expected to be
    // scheduled to receive a PDSCH starting earlier than the end of the first PDSCH with a PDCCH that ends later
    // than symbol i.".
    slot_point last_pdsch_slot = ue_cc.harqs.last_pdsch_slot();
    if (last_pdsch_slot.valid() and pdsch_alloc.slot <= last_pdsch_slot) {
      // Try next k0 candidate.
      continue;
    }

    srsran_sanity_check(pdsch_alloc.result.dl.bc.sibs.size() + pdsch_alloc.result.dl.paging_grants.size() +
                                pdsch_alloc.result.dl.rar_grants.size() + pdsch_alloc.result.dl.ue_grants.size() <
                            expert_cfg.max_pdschs_per_slot,
                        "Max number of PDSCHs per slot was reached");
    srsran_sanity_check(not pdsch_alloc.result.dl.ue_grants.full(), "No space available in scheduler PDSCH outputs");
    srsran_sanity_check(ue_cc.is_pdsch_enabled(pdsch_alloc.slot), "DL is not active in the PDSCH slot");

    // Apply RB allocation limits.
    const unsigned start_rb = std::max(expert_cfg.pdsch_crb_limits.start(), ss_info.dl_crb_lims.start());
    const unsigned end_rb   = std::min(expert_cfg.pdsch_crb_limits.stop(), ss_info.dl_crb_lims.stop());
    if (start_rb >= end_rb) {
      // Invalid RB allocation range.
      return {alloc_status::skip_slot};
    }
    const crb_interval dl_crb_lims = {start_rb, end_rb};

    // Estimate required MCS/#PRBs.
    const pdsch_config_params*       pdsch_cfg = nullptr;
    sched_helper::mcs_prbs_selection mcs_prbs;
    if (is_retx) {
      pdsch_cfg = &ss_info.get_pdsch_config(param_candidate.pdsch_td_res_index(), h_dl->get_grant_params().nof_layers);
      mcs_prbs =
          sched_helper::mcs_prbs_selection{h_dl->get_grant_params().mcs, h_dl->get_grant_params().rbs.type1().length()};
    } else {
      pdsch_cfg   = &ss_info.get_pdsch_config(param_candidate.pdsch_td_res_index(),
                                            ue_cc.channel_state_manager().get_nof_dl_layers());
      auto result = sched_helper::compute_newtx_required_mcs_and_prbs(
          *pdsch_cfg, ue_cc, sched_params.recommended_nof_bytes.value());
      if (not result.has_value()) {
        return {alloc_status::skip_ue};
      }
      // [Implementation-defined] In case of partial slots and nof. PRBs allocated equals to 1 probability of KO is
      // high due to code not being able to cope with interference. So the solution is to increase the PRB allocation
      // to greater than 1 PRB.
      if (result.value().nof_prbs == 1 and not cell_cfg.is_fully_dl_enabled(pdsch_alloc.slot)) {
        result.value().nof_prbs = 2;
      }
      mcs_prbs = result.value();
    }

    // Generate result.
    dl_grant_params result;
    result.status               = alloc_status::success;
    result.ss_id                = ss_info.cfg->get_id();
    result.pdsch_td_res_index   = param_candidate.pdsch_td_res_index();
    result.crb_lims             = dl_crb_lims;
    result.dci_type             = dci_type;
    result.pdsch_cfg            = pdsch_cfg;
    result.recommended_mcs_prbs = mcs_prbs;
    return result;
  }

  return {alloc_status::skip_ue};
}

ue_cell_grid_allocator::ul_grant_params
ue_cell_grid_allocator::get_ul_grant_params(const common_ue_ul_grant_request& sched_params)
{
  static constexpr unsigned pdcch_delay_in_slots = 0;
  const slice_ue&           slice_ue             = *sched_params.user;
  srsran_sanity_check(ues.contains(slice_ue.ue_index()), "Invalid UE candidate index");
  const bool is_retx = sched_params.h_ul.has_value();

  ue&            u     = ues[slice_ue.ue_index()];
  const ue_cell& ue_cc = sched_params.user->get_cc();
  srsran_sanity_check(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(), "Invalid UE candidate");

  const ue_cell_configuration&                 ue_cell_cfg = ue_cc.cfg();
  const cell_configuration&                    cell_cfg    = ue_cell_cfg.cell_cfg_common;
  const std::optional<ul_harq_process_handle>& h_ul        = sched_params.h_ul;

  // Fetch PDCCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = cell_alloc[pdcch_delay_in_slots];
  srsran_sanity_check(ue_cc.is_pdcch_enabled(pdcch_alloc.slot), "DL is not active in the PDCCH slot");
  srsran_sanity_check(not pdcch_alloc.result.dl.ul_pdcchs.full(), "Maximum number of UL PDCCH grants per slot reached");

  // Create PUSCH param candidate search object.
  ue_pusch_alloc_param_candidate_searcher candidates{
      u, cell_alloc.cell_index(), h_ul, pdcch_alloc.slot, sched_params.pusch_slot};
  if (candidates.is_empty()) {
    // The conditions for a new PUSCH allocation for this UE were not met (e.g. lack of available SearchSpaces).
    return {alloc_status::skip_ue};
  }

  // Iterate through allocation parameter candidates.
  for (const ue_pusch_alloc_param_candidate_searcher::candidate& param_candidate : candidates) {
    const pusch_time_domain_resource_allocation& pusch_td_cfg = param_candidate.pusch_td_res();
    const search_space_info&                     ss_info      = param_candidate.ss();
    const dci_ul_rnti_config_type                dci_type     = param_candidate.dci_ul_rnti_cfg_type();
    const unsigned                               final_k2     = pusch_td_cfg.k2 + cell_cfg.ntn_cs_koffset;

    // Fetch PUSCH resource grid allocators.
    cell_slot_resource_allocator& pusch_alloc = cell_alloc[pdcch_delay_in_slots + final_k2];

    // Verify that the order of PUSCHs for the same UE matches the order of PDCCHs and that there is at most one PUSCH
    // per slot.
    // [TS 38.214, 6.1] "For any HARQ process ID(s) in a given scheduled cell, the UE is not expected to transmit a
    // PUSCH that overlaps in time with another PUSCH".
    // [TS 38.214, 6.1] "For any two HARQ process IDs in a given scheduled cell, if the UE is scheduled to start a first
    // PUSCH transmission starting in symbol j by a PDCCH ending in symbol i, the UE is not expected to be scheduled to
    // transmit a PUSCH starting earlier than the end of the first PUSCH by a PDCCH that ends later than symbol i".
    slot_point last_pusch_slot = ue_cc.harqs.last_pusch_slot();
    if (last_pusch_slot.valid() and pusch_alloc.slot <= last_pusch_slot) {
      return {alloc_status::skip_ue};
    }

    // Check if there is space for more PUSCH PDUs.
    srsran_sanity_check(not pusch_alloc.result.ul.puschs.full(), "No PUSCH space available in scheduler output list");
    srsran_sanity_check(cell_cfg.is_ul_enabled(pusch_alloc.slot), "UL is not active in the PUSCH slot");

    // When checking the number of remaining grants for PUSCH, take into account that the PUCCH grants for this UE will
    // be removed when multiplexing the UCI on PUSCH.
    unsigned pusch_pdu_rem_space = get_space_left_for_pusch_pdus(pusch_alloc.result, u.crnti, expert_cfg);
    if (pusch_pdu_rem_space == 0) {
      if (pusch_alloc.result.ul.puschs.size() >= expert_cfg.max_puschs_per_slot) {
        logger.info(
            "ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: Max number of PUSCHs per slot {} was reached.",
            fmt::underlying(u.ue_index),
            u.crnti,
            pusch_alloc.slot,
            expert_cfg.max_puschs_per_slot);
      } else {
        logger.info("ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: Max number of UL grants per slot {} "
                    "was reached.",
                    fmt::underlying(u.ue_index),
                    u.crnti,
                    pusch_alloc.slot,
                    expert_cfg.max_puschs_per_slot);
      }
      return {alloc_status::skip_slot};
    }

    // [Implementation-defined] We skip allocation of PUSCH if there is already a PUCCH grant scheduled using common
    // PUCCH resources.
    if (uci_alloc.has_uci_harq_on_common_pucch_res(u.crnti, pusch_alloc.slot)) {
      logger.debug("ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: UE has PUCCH grant using common PUCCH "
                   "resources scheduled",
                   fmt::underlying(u.ue_index),
                   u.crnti,
                   pusch_alloc.slot);
      return {alloc_status::skip_ue};
    }

    const unsigned start_ul_symbols =
        NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.get_nof_ul_symbol_per_slot(pusch_alloc.slot);
    // If it is a retx, we need to ensure we use a time_domain_resource with the same number of symbols as used for
    // the first transmission.
    const bool sym_length_match_prev_grant_for_retx =
        is_retx ? pusch_td_cfg.symbols.length() == h_ul->get_grant_params().nof_symbols : true;
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
                   fmt::underlying(u.ue_index),
                   u.crnti,
                   pusch_alloc.slot,
                   start_rb,
                   end_rb);
      return {alloc_status::skip_slot};
    }
    const crb_interval ul_crb_lims = {start_rb, end_rb};

    // Compute the MCS and the number of PRBs, depending on the pending bytes to transmit.
    pusch_config_params              pusch_cfg;
    sched_helper::mcs_prbs_selection mcs_prbs;
    // Note: We assume k2 <= k1, which means that all the HARQ bits are set at this point for this UL slot and UE.
    const unsigned nof_harq_ack_bits = uci_alloc.get_scheduled_pdsch_counter_in_ue_uci(pusch_alloc, u.crnti);
    const bool     is_csi_report_slot =
        u.get_pcell().cfg().csi_meas_cfg() != nullptr and
        csi_helper::is_csi_reporting_slot(*u.get_pcell().cfg().csi_meas_cfg(), pusch_alloc.slot);
    if (is_retx) {
      pusch_cfg = sched_helper::compute_retx_pusch_config_params(
          ue_cc, *h_ul, pusch_td_cfg, nof_harq_ack_bits, is_csi_report_slot);
      mcs_prbs =
          sched_helper::mcs_prbs_selection{h_ul->get_grant_params().mcs, h_ul->get_grant_params().rbs.type1().length()};
    } else {
      pusch_cfg = sched_helper::compute_newtx_pusch_config_params(
          ue_cc, dci_type, pusch_td_cfg, nof_harq_ack_bits, is_csi_report_slot);
      mcs_prbs = sched_helper::compute_newtx_required_mcs_and_prbs(
          pusch_cfg, ue_cc, sched_params.recommended_nof_bytes.value());
    }

    // Generate result.
    ul_grant_params result;
    result.status               = alloc_status::success;
    result.ss_id                = ss_info.cfg->get_id();
    result.pusch_td_res_index   = param_candidate.pusch_td_res_index();
    result.crb_lims             = ul_crb_lims;
    result.dci_type             = dci_type;
    result.pusch_cfg            = pusch_cfg;
    result.recommended_mcs_prbs = mcs_prbs;
    return result;
  }

  return {alloc_status::skip_ue};
}

expected<pdcch_dl_information*, alloc_status> ue_cell_grid_allocator::alloc_dl_pdcch(const ue_cell&           ue_cc,
                                                                                     const search_space_info& ss_info)
{
  const rnti_t crnti = ue_cc.rnti();

  const aggregation_level aggr_lvl =
      ue_cc.get_aggregation_level(ue_cc.link_adaptation_controller().get_effective_cqi(), ss_info, true);

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

expected<uci_allocation, alloc_status>
ue_cell_grid_allocator::alloc_uci(ue_cell& ue_cc, const search_space_info& ss_info, uint8_t pdsch_td_res_index)
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
    return make_unexpected(alloc_status::skip_ue);
  }

  return uci.value();
}

dl_alloc_result ue_cell_grid_allocator::allocate_newtx_dl_grant(const ue_dl_newtx_grant_request& request)
{
  return allocate_dl_grant(common_ue_dl_grant_request{
      request.pdsch_slot, &request.user, std::nullopt, request.pending_bytes, request.max_nof_rbs});
}

dl_alloc_result ue_cell_grid_allocator::allocate_retx_dl_grant(const ue_dl_retx_grant_request& request)
{
  return allocate_dl_grant(
      common_ue_dl_grant_request{request.pdsch_slot, &request.user, request.h_dl, std::nullopt, std::nullopt});
}

dl_alloc_result ue_cell_grid_allocator::allocate_dl_grant(const common_ue_dl_grant_request& grant)
{
  // Derive DL grant parameters and verify conditions for allocation.
  dl_grant_params grant_params = get_dl_grant_params(grant);
  if (grant_params.status != alloc_status::success) {
    return {grant_params.status};
  }

  // Derive remaining parameters from \c dl_grant_params.
  ue&                                          u           = ues[grant.user->ue_index()];
  ue_cell&                                     ue_cc       = *u.find_cell(cell_alloc.cell_index());
  const ue_cell_configuration&                 ue_cell_cfg = ue_cc.cfg();
  const search_space_info&                     ss_info     = ue_cell_cfg.search_space(grant_params.ss_id);
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg =
      ss_info.pdsch_time_domain_list[grant_params.pdsch_td_res_index];
  const subcarrier_spacing              scs      = ss_info.bwp->dl_common->value().generic_params.scs;
  const cell_configuration&             cell_cfg = ue_cell_cfg.cell_cfg_common;
  const bool                            is_retx  = grant.h_dl.has_value();
  std::optional<dl_harq_process_handle> h_dl;
  if (is_retx) {
    h_dl = ue_cc.harqs.dl_harq(grant.h_dl->id());
  }

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = cell_alloc[0];
  cell_slot_resource_allocator& pdsch_alloc = cell_alloc[pdsch_td_cfg.k0];

  const crb_bitmap used_crbs = pdsch_alloc.dl_res_grid.used_crbs(scs, grant_params.crb_lims, pdsch_td_cfg.symbols);
  if (used_crbs.all()) {
    return {alloc_status::skip_slot};
  }

  // Try to limit the grant PRBs.
  unsigned n_prbs = grant_params.recommended_mcs_prbs.nof_prbs;
  if (not is_retx) {
    // New Tx case.

    // Limit nof. RBs to allocate to maximum RBs provided in grant.
    if (grant.max_nof_rbs.has_value()) {
      n_prbs = std::min(n_prbs, grant.max_nof_rbs.value());
    }

    // Re-apply nof. PDSCH RBs to allocate limits.
    n_prbs = std::max(n_prbs, expert_cfg.pdsch_nof_rbs.start());
    n_prbs = std::min(n_prbs, expert_cfg.pdsch_nof_rbs.stop());
    n_prbs = std::max(n_prbs, ue_cell_cfg.rrm_cfg().pdsch_grant_size_limits.start());
    n_prbs = std::min(n_prbs, ue_cell_cfg.rrm_cfg().pdsch_grant_size_limits.stop());

    if (n_prbs == 0) {
      logger.debug("ue={} rnti={} PDSCH allocation skipped. Cause: UE's CQI=0 ", fmt::underlying(u.ue_index), u.crnti);
      return {alloc_status::skip_ue};
    }
  }

  crb_interval crbs = rb_helper::find_empty_interval_of_length(used_crbs, n_prbs, 0);
  if (crbs.empty()) {
    logger.debug("ue={} rnti={}: Failed to allocate PDSCH. Cause: Cause: No more RBs available at slot={}",
                 fmt::underlying(u.ue_index),
                 u.crnti,
                 pdsch_alloc.slot);
    return {alloc_status::skip_slot};
  }

  // In case of Retx, the #CRBs need to stay the same.
  if (is_retx and crbs.length() != h_dl->get_grant_params().rbs.type1().length()) {
    logger.debug(
        "ue={} rnti={}: Failed to allocate PDSCH. Cause: No more RBs available at slot={} for h_id={} retransmission",
        fmt::underlying(u.ue_index),
        u.crnti,
        pdsch_alloc.slot,
        fmt::underlying(h_dl->id()));
    return {alloc_status::skip_ue};
  }

  // Allocate PDCCH.
  auto pdcch_result = alloc_dl_pdcch(ue_cc, ss_info);
  if (not pdcch_result.has_value()) {
    return {pdcch_result.error()};
  }
  pdcch_dl_information* pdcch = pdcch_result.value();

  // Allocate UCI.
  auto uci_result = alloc_uci(ue_cc, ss_info, grant_params.pdsch_td_res_index);
  if (not uci_result.has_value()) {
    pdcch_sched.cancel_last_pdcch(pdcch_alloc);
    return {uci_result.error()};
  }
  uci_allocation& uci                     = uci_result.value();
  unsigned        k1                      = uci.k1;
  pdcch->ctx.context.harq_feedback_timing = k1;

  // [Implementation-defined] Check whether max. PUCCHs per slot or max. UL grants per slot is reached if PDSCH
  // allocation for current UE succeeds. If so, allocate remaining RBs to the current UE only if it's a new Tx.
  // NOTE: At this point UCI is already allocated hence '>' is used rather than '>='.
  cell_slot_resource_allocator& ul_alloc = cell_alloc[pdsch_td_cfg.k0 + k1];
  if (not is_retx and ((ul_alloc.result.ul.pucchs.size() > (expert_cfg.max_pucchs_per_slot - 1)) or
                       ((ul_alloc.result.ul.pucchs.size() + ul_alloc.result.ul.puschs.size()) >
                        (expert_cfg.max_ul_grants_per_slot - 1)))) {
    crbs = rb_helper::find_empty_interval_of_length(used_crbs, used_crbs.size(), 0);
  }

  // Verify there is no RB collision.
  srsran_assert(not pdsch_alloc.dl_res_grid.collides(scs, pdsch_td_cfg.symbols, crbs),
                "Invalid calculation of PDSCH RBs");

  // Reduce estimated MCS by 1 whenever CSI-RS is sent over a particular slot to account for the overhead of CSI-RS
  // REs.
  sch_mcs_index adjusted_mcs{grant_params.recommended_mcs_prbs.mcs.value()};
  if (not is_retx and not pdsch_alloc.result.dl.csi_rs.empty()) {
    // [Implementation-defined] The max MCS values below are set empirically and should avoid the effective code rate
    // to exceed 0.95 due to the overhead of CSI-RS REs.
    adjusted_mcs                = adjusted_mcs == 0 ? adjusted_mcs : adjusted_mcs - 1;
    uint8_t max_mcs_with_csi_rs = 28;
    if (grant_params.pdsch_cfg->mcs_table == pdsch_mcs_table::qam64) {
      max_mcs_with_csi_rs = 26U;
    } else if (grant_params.pdsch_cfg->mcs_table == pdsch_mcs_table::qam256) {
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

    mcs_tbs_info = compute_dl_mcs_tbs(*grant_params.pdsch_cfg, adjusted_mcs, crbs.length(), contains_dc);

    // If there is not MCS-TBS info, it means no MCS exists such that the effective code rate is <= 0.95.
    if (not mcs_tbs_info.has_value()) {
      logger.warning("ue={} rnti={}: Failed to allocate PDSCH. Cause: no MCS such that code rate <= 0.95.",
                     fmt::underlying(u.ue_index),
                     u.crnti);
      pdcch_sched.cancel_last_pdcch(pdcch_alloc);
      return {alloc_status::skip_ue};
    }
  } else {
    // It is a retx.
    mcs_tbs_info.emplace(sch_mcs_tbs{.mcs = h_dl->get_grant_params().mcs, .tbs = h_dl->get_grant_params().tbs_bytes});
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, crbs});

  // Allocate UE DL HARQ.
  bool is_new_data = not is_retx;
  if (is_new_data) {
    // It is a new tx.
    h_dl = ue_cc.harqs.alloc_dl_harq(pdsch_alloc.slot, k1, expert_cfg.max_nof_dl_harq_retxs, uci.harq_bit_idx).value();
  } else {
    // It is a retx.
    bool result = h_dl->new_retx(pdsch_alloc.slot, k1, uci.harq_bit_idx);
    srsran_assert(result, "Harq is in invalid state");
  }

  // Compute TPC for PUCCH.
  uint8_t tpc = ue_cc.get_pucch_power_controller().compute_tpc_command(pdsch_alloc.slot + k1);

  // Fill DL PDCCH DCI PDU.
  // Number of possible Downlink Assignment Indexes {0, ..., 3} as per TS38.213 Section 9.1.3.
  static constexpr unsigned DAI_MOD = 4U;
  uint8_t                   rv      = ue_cc.get_pdsch_rv(h_dl->nof_retxs());
  // For allocation on PUSCH, we use a PUCCH resource indicator set to 0, as it will get ignored by the UE.
  const unsigned pucch_res_indicator = uci.pucch_res_indicator.has_value() ? uci.pucch_res_indicator.value() : 0U;
  switch (grant_params.dci_type) {
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      build_dci_f1_0_c_rnti(pdcch->dci,
                            ss_info,
                            cell_cfg.dl_cfg_common.init_dl_bwp,
                            crbs,
                            grant_params.pdsch_td_res_index,
                            k1,
                            pucch_res_indicator,
                            uci.harq_bit_idx % DAI_MOD,
                            mcs_tbs_info.value().mcs,
                            rv,
                            *h_dl);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_1:
      build_dci_f1_1_c_rnti(pdcch->dci,
                            ue_cell_cfg,
                            grant_params.ss_id,
                            crb_to_prb(ss_info.dl_crb_lims, crbs),
                            grant_params.pdsch_td_res_index,
                            k1,
                            pucch_res_indicator,
                            uci.harq_bit_idx % DAI_MOD,
                            mcs_tbs_info.value().mcs,
                            rv,
                            *h_dl,
                            grant_params.pdsch_cfg->nof_layers,
                            tpc);
      break;
    default:
      report_fatal_error("Unsupported RNTI type for PDSCH allocation");
  }

  // Fill PDSCH PDU.
  dl_msg_alloc& msg            = pdsch_alloc.result.dl.ue_grants.emplace_back();
  msg.context.ue_index         = u.ue_index;
  msg.context.k1               = k1;
  msg.context.ss_id            = grant_params.ss_id;
  msg.context.nof_retxs        = h_dl->nof_retxs();
  msg.context.buffer_occupancy = 0; // We fill this value later, after the TB is built.
  if (is_new_data and ue_cc.link_adaptation_controller().is_dl_olla_enabled()) {
    msg.context.olla_offset = ue_cc.link_adaptation_controller().dl_cqi_offset();
  }
  switch (pdcch->dci.type) {
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      build_pdsch_f1_0_c_rnti(msg.pdsch_cfg,
                              *grant_params.pdsch_cfg,
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
                              *grant_params.pdsch_cfg,
                              mcs_tbs_info.value(),
                              u.crnti,
                              ue_cell_cfg,
                              grant_params.ss_id,
                              pdcch->dci.c_rnti_f1_1,
                              crbs,
                              h_dl->nof_retxs() == 0,
                              ue_cc.channel_state_manager());
      break;
    default:
      report_fatal_error("Unsupported PDCCH DL DCI format");
  }

  // Save set PDCCH and PDSCH PDU parameters in HARQ process.
  dl_harq_alloc_context pdsch_sched_ctx;
  pdsch_sched_ctx.dci_cfg_type = pdcch->dci.type;
  if (is_new_data) {
    pdsch_sched_ctx.olla_mcs =
        ue_cc.link_adaptation_controller().calculate_dl_mcs(msg.pdsch_cfg.codewords[0].mcs_table);
    pdsch_sched_ctx.slice_id = grant.user->ran_slice_id();
  }
  pdsch_sched_ctx.cqi = ue_cc.channel_state_manager().get_wideband_cqi();

  if (is_new_data) {
    // Set MAC logical channels to schedule in this PDU if it is a newtx.
    u.build_dl_transport_block_info(
        msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes, grant.user->ran_slice_id());

    // Update context with buffer occupancy after the TB is built.
    msg.context.buffer_occupancy = u.pending_dl_newtx_bytes();
  }

  h_dl->save_grant_params(pdsch_sched_ctx, msg);

  // Update DRX state given the new allocation.
  u.drx_controller().on_new_pdcch_alloc(pdcch_alloc.slot);

  return {alloc_status::success,
          h_dl->get_grant_params().tbs_bytes,
          crbs.length(),
          is_new_data ? msg.tb_list.back() : dl_msg_tb_info{}};
}

ul_alloc_result ue_cell_grid_allocator::allocate_newtx_ul_grant(const ue_ul_newtx_grant_request& request)
{
  return allocate_ul_grant(common_ue_ul_grant_request{
      request.pusch_slot, &request.user, std::nullopt, request.pending_bytes, request.max_nof_rbs});
}

ul_alloc_result ue_cell_grid_allocator::allocate_retx_ul_grant(const ue_ul_retx_grant_request& request)
{
  return allocate_ul_grant(
      common_ue_ul_grant_request{request.pusch_slot, &request.user, request.h_ul, std::nullopt, std::nullopt});
}

ul_alloc_result ue_cell_grid_allocator::allocate_ul_grant(const common_ue_ul_grant_request& grant)
{
  ul_grant_params grant_params = get_ul_grant_params(grant);
  if (grant_params.status != alloc_status::success) {
    return {grant_params.status};
  }
  static constexpr unsigned pdcch_delay_in_slots = 0;
  ue&                       u                    = ues[grant.user->ue_index()];
  ue_cell&                  ue_cc                = *u.find_cell(cell_alloc.cell_index());

  const ue_cell_configuration&  ue_cell_cfg = ue_cc.cfg();
  const cell_configuration&     cell_cfg    = ue_cell_cfg.cell_cfg_common;
  cell_slot_resource_allocator& pdcch_alloc = cell_alloc[pdcch_delay_in_slots];

  // Derive remaining parameters from \c ul_grant_params.
  const search_space_info&                     ss_info     = ue_cell_cfg.search_space(grant_params.ss_id);
  const search_space_configuration&            ss_cfg      = *ss_info.cfg;
  const bwp_uplink_common&                     bwp_ul_cmn  = *ss_info.bwp->ul_common.value();
  const subcarrier_spacing                     scs         = bwp_ul_cmn.generic_params.scs;
  const dci_ul_rnti_config_type                dci_type    = grant_params.dci_type;
  const crb_interval&                          ul_crb_lims = grant_params.crb_lims;
  const pusch_time_domain_resource_allocation& pusch_td_cfg =
      ss_info.pusch_time_domain_list[grant_params.pusch_td_res_index];
  const pusch_config_params&            pusch_cfg = grant_params.pusch_cfg;
  const bool                            is_retx   = grant.h_ul.has_value();
  std::optional<ul_harq_process_handle> h_ul;
  if (is_retx) {
    h_ul = ue_cc.harqs.ul_harq(grant.h_ul->id());
  }

  // Fetch PUSCH resource grid allocators.
  const unsigned                final_k2    = pusch_td_cfg.k2 + cell_cfg.ntn_cs_koffset;
  cell_slot_resource_allocator& pusch_alloc = cell_alloc[pdcch_delay_in_slots + final_k2];

  const prb_bitmap used_crbs = pusch_alloc.ul_res_grid.used_crbs(scs, ul_crb_lims, pusch_td_cfg.symbols);
  if (used_crbs.all()) {
    return {alloc_status::skip_slot};
  }

  sched_helper::mcs_prbs_selection mcs_prbs = grant_params.recommended_mcs_prbs;
  if (not is_retx) {
    // newTx case.

    // Due to the pre-allocated UCI bits, MCS 0 and PRB 1 would not leave any space for the payload on the TBS, as
    // all the space would be taken by the UCI bits. As a result of this, the effective code rate would be 0 and the
    // allocation would fail and be postponed to the next slot.
    // [Implementation-defined] In our tests, we have seen that MCS 5 with 1 PRB can lead (depending on the
    // configuration) to a non-valid MCS-PRB allocation; therefore, we set 6 as minimum value for 1 PRB.
    // TODO: Remove this part and handle the problem with a loop that is general for any configuration.
    const sch_mcs_index min_mcs_for_1_prb{6U};
    const unsigned      min_allocable_prbs = 1U;
    if (mcs_prbs.mcs < min_mcs_for_1_prb and mcs_prbs.nof_prbs <= min_allocable_prbs) {
      ++mcs_prbs.nof_prbs;
    }

    // Limit nof. RBs to allocate to maximum RBs provided in grant.
    if (grant.max_nof_rbs.has_value()) {
      mcs_prbs.nof_prbs = std::min(mcs_prbs.nof_prbs, grant.max_nof_rbs.value());
    }

    // Re-apply nof. PUSCH RBs to allocate limits.
    mcs_prbs.nof_prbs = std::max(mcs_prbs.nof_prbs, expert_cfg.pusch_nof_rbs.start());
    mcs_prbs.nof_prbs = std::min(mcs_prbs.nof_prbs, expert_cfg.pusch_nof_rbs.stop());
    mcs_prbs.nof_prbs = std::max(mcs_prbs.nof_prbs, ue_cell_cfg.rrm_cfg().pusch_grant_size_limits.start());
    mcs_prbs.nof_prbs = std::min(mcs_prbs.nof_prbs, ue_cell_cfg.rrm_cfg().pusch_grant_size_limits.stop());

    // Ensure the number of PRB is valid if the transform precoder is used. The condition the PUSCH bandwidth with
    // transform precoder is defined in TS 38.211 Section 6.1.3. The number of PRB must be lower than or equal to
    // current number of PRB.
    if ((dci_type == dci_ul_rnti_config_type::c_rnti_f0_1)
            ? ue_cell_cfg.use_pusch_transform_precoding_dci_0_1()
            : ue_cell_cfg.cell_cfg_common.use_msg3_transform_precoder()) {
      mcs_prbs.nof_prbs =
          get_transform_precoding_nearest_lower_nof_prb_valid(mcs_prbs.nof_prbs).value_or(mcs_prbs.nof_prbs);
    }
  }

  // NOTE: This should never happen, but it's safe not to proceed if we get nof_prbs == 0.
  if (mcs_prbs.nof_prbs == 0) {
    logger.debug(
        "ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: MCS and PRBs computation resulted in no PRBs "
        "allocated to this UE",
        fmt::underlying(u.ue_index),
        u.crnti,
        pusch_alloc.slot);
    return {alloc_status::skip_ue};
  }

  crb_interval crbs = rb_helper::find_empty_interval_of_length(used_crbs, mcs_prbs.nof_prbs, 0);
  if (crbs.empty()) {
    logger.debug("ue={} rnti={}: Failed to allocate PUSCH. Cause: Cause: No more RBs available at slot={}",
                 fmt::underlying(u.ue_index),
                 u.crnti,
                 pusch_alloc.slot);
    return {alloc_status::skip_slot};
  }

  // In case of Retx, the #CRBs need to stay the same.
  if (is_retx and crbs.length() != h_ul->get_grant_params().rbs.type1().length()) {
    logger.debug(
        "ue={} rnti={}: Failed to allocate PUSCH. Cause: No more RBs available at slot={} for h_id={} retransmission",
        fmt::underlying(u.ue_index),
        u.crnti,
        pusch_alloc.slot,
        fmt::underlying(h_ul->id()));
    return {alloc_status::skip_ue};
  }

  // Compute TPC command before computing the nof_prbs adaptation based on PHR; this is because, when the TPC gets
  // computed, the channel state manager will update close-loop power control adjustment.
  static constexpr uint8_t default_tpc_command = 1U;
  const uint8_t            tpc_command         = dci_type != dci_ul_rnti_config_type::tc_rnti_f0_0
                                                     ? ue_cc.get_pusch_power_controller().compute_tpc_command(grant.pusch_slot)
                                                     : default_tpc_command;

  // If this is not a retx, then we need to adjust the number of PRBs to the PHR, to prevent the UE from reducing the
  // nominal TX power to meet the max TX power.
  if (not is_retx) {
    const unsigned nof_prbs_adjusted_to_phr = ue_cc.get_pusch_power_controller().adapt_pusch_prbs_to_phr(crbs.length());
    if (nof_prbs_adjusted_to_phr < crbs.length()) {
      crbs.resize(nof_prbs_adjusted_to_phr);
    }
  }

  // Verify there is no RB collision.
  if (pusch_alloc.ul_res_grid.collides(scs, pusch_td_cfg.symbols, crbs)) {
    logger.warning("ue={} rnti={}: Failed to allocate PUSCH in slot={}. Cause: Allocation collides with existing "
                   "PUSCH in RB grid [{}, {})",
                   fmt::underlying(u.ue_index),
                   u.crnti,
                   pusch_alloc.slot,
                   crbs.start(),
                   crbs.stop());
    return {alloc_status::invalid_params};
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
    return {nof_cces_left == 0 ? alloc_status::skip_slot : alloc_status::skip_ue};
  }

  // Compute MCS and TBS for this transmission.
  std::optional<sch_mcs_tbs> mcs_tbs_info;
  // If it's a new Tx, compute the MCS and TBS from SNR, payload size, and available RBs.
  if (not is_retx) {
    bool contains_dc =
        dc_offset_helper::is_contained(cell_cfg.expert_cfg.ue.initial_ul_dc_offset, cell_cfg.nof_ul_prbs, crbs);

    mcs_tbs_info = compute_ul_mcs_tbs(pusch_cfg, &ue_cell_cfg, mcs_prbs.mcs, crbs.length(), contains_dc);
  }
  // If it's a reTx, fetch the MCS, TBS and number of layers from the previous transmission.
  else {
    const auto& prev_params = h_ul->get_grant_params();
    mcs_tbs_info.emplace(sch_mcs_tbs{.mcs = prev_params.mcs, .tbs = prev_params.tbs_bytes});
    srsran_assert(prev_params.mcs_table == pusch_cfg.mcs_table, "MCS table cannot change across HARQ reTxs");
  }

  // If there is not MCS-TBS info, it means no MCS exists such that the effective code rate is <= 0.95.
  if (not mcs_tbs_info.has_value()) {
    logger.warning(
        "ue={} rnti={}: Failed to allocate PUSCH. Cause: no MCS such that code rate <= 0.95 with this "
        "configuration: mcs={} crbs={} symbols={} nof_oh={} tb-sc-field={} layers={} pi2bpsk={} "
        "harq_bits={} csi1_bits={} csi2_bits={} mcs_table_idx={} dmrs_A_pos={} is_dmrs_type2={} dmrs_add_pos_idx={}",
        fmt::underlying(u.ue_index),
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
        ue_cell_cfg.init_bwp().ul_ded->pusch_cfg->pusch_mapping_type_a_dmrs.value().is_dmrs_type2 ? "yes" : "no",
        static_cast<unsigned>(
            ue_cell_cfg.init_bwp().ul_ded->pusch_cfg->pusch_mapping_type_a_dmrs.value().additional_positions));
    pdcch_sched.cancel_last_pdcch(pdcch_alloc);
    return {alloc_status::invalid_params};
  }

  // Mark resources as occupied in the ResourceGrid.
  pusch_alloc.ul_res_grid.fill(grant_info{scs, pusch_td_cfg.symbols, crbs});

  // Allocate UE UL HARQ.
  bool is_new_data = not is_retx;
  if (is_new_data) {
    // It is a new tx.
    h_ul = ue_cc.harqs.alloc_ul_harq(pusch_alloc.slot, expert_cfg.max_nof_ul_harq_retxs);
    srsran_assert(h_ul.has_value(), "Failed to allocate HARQ");
  } else {
    // It is a retx.
    bool result = h_ul->new_retx(pusch_alloc.slot);
    srsran_assert(result, "Failed to allocate HARQ retx");
  }

  // Compute total DAI. See TS 38.213, 9.1.3.2.
  // Note: We assume k2 <= k1, which means that all the HARQ bits are set at this point for this UL slot and UE.
  const unsigned nof_harq_ack_bits = uci_alloc.get_scheduled_pdsch_counter_in_ue_uci(pusch_alloc, u.crnti);
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
  uint8_t rv = ue_cc.get_pusch_rv(h_ul->nof_retxs());
  switch (dci_type) {
    case dci_ul_rnti_config_type::tc_rnti_f0_0:
      build_dci_f0_0_tc_rnti(pdcch->dci,
                             *ue_cell_cfg.bwp(to_bwp_id(0)).dl_common.value(),
                             ue_cell_cfg.bwp(ue_cc.active_bwp_id()).ul_common->value().generic_params,
                             crbs,
                             grant_params.pusch_td_res_index,
                             mcs_tbs_info.value().mcs,
                             rv);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_0:
      build_dci_f0_0_c_rnti(pdcch->dci,
                            ss_info,
                            cell_cfg.ul_cfg_common.init_ul_bwp,
                            crbs,
                            grant_params.pusch_td_res_index,
                            mcs_tbs_info.value().mcs,
                            rv,
                            *h_ul,
                            tpc_command);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_1:
      build_dci_f0_1_c_rnti(pdcch->dci,
                            ue_cell_cfg,
                            ss_cfg.get_id(),
                            crbs,
                            grant_params.pusch_td_res_index,
                            mcs_tbs_info.value().mcs,
                            rv,
                            *h_ul,
                            dai,
                            pusch_cfg.nof_layers,
                            ue_cc.channel_state_manager().get_recommended_pusch_tpmi(pusch_cfg.nof_layers),
                            tpc_command);
      break;
    default:
      report_fatal_error("Unsupported PDCCH UL DCI format");
  }

  // Fill PUSCH.
  ul_sched_info& msg    = pusch_alloc.result.ul.puschs.emplace_back();
  msg.context.ue_index  = u.ue_index;
  msg.context.ss_id     = ss_cfg.get_id();
  msg.context.k2        = final_k2;
  msg.context.nof_retxs = h_ul->nof_retxs();
  if (is_new_data and ue_cc.link_adaptation_controller().is_ul_olla_enabled()) {
    msg.context.olla_offset = ue_cc.link_adaptation_controller().ul_snr_offset_db();
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
                              h_ul->nof_retxs() == 0);
      break;
    default:
      report_fatal_error("Unsupported PDCCH UL DCI format");
  }

  // Check if there is any UCI grant allocated on the PUCCH that can be moved to the PUSCH.
  uci_alloc.multiplex_uci_on_pusch(msg, pusch_alloc, ue_cell_cfg, u.crnti);

  // Save set PDCCH and PUSCH PDU parameters in HARQ process.
  ul_harq_alloc_context pusch_sched_ctx;
  pusch_sched_ctx.dci_cfg_type = pdcch->dci.type;
  if (is_new_data) {
    pusch_sched_ctx.olla_mcs = ue_cc.link_adaptation_controller().calculate_ul_mcs(msg.pusch_cfg.mcs_table);
    pusch_sched_ctx.slice_id = grant.user->ran_slice_id();
  }

  h_ul->save_grant_params(pusch_sched_ctx, msg.pusch_cfg);

  // Register UL allocations for this slot.
  u.handle_ul_transport_block_info(h_ul->get_grant_params().tbs_bytes);

  // Update DRX state given the new allocation.
  u.drx_controller().on_new_pdcch_alloc(pdcch_alloc.slot);

  // Update the number of PRBs used in the PUSCH allocation.
  ue_cc.get_pusch_power_controller().update_pusch_pw_ctrl_state(pusch_alloc.slot, crbs.length());

  return {alloc_status::success, h_ul->get_grant_params().tbs_bytes, crbs.length()};
}

void ue_cell_grid_allocator::post_process_results()
{
  auto& slot_alloc = cell_alloc[0];

  // Update the PUCCH power control data.
  post_process_pucch_pw_ctrl_results(slot_alloc.slot);
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
