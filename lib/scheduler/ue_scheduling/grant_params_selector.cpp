/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "grant_params_selector.h"
#include "../slicing/slice_ue_repository.h"
#include "../support/dmrs_helpers.h"
#include "../support/prbs_calculator.h"
#include "../ue_context/ue_cell.h"
#include "ue_pdsch_alloc_param_candidate_searcher.h"

using namespace srsran;
using namespace sched_helper;

std::optional<mcs_prbs_selection>
sched_helper::compute_newtx_required_mcs_and_prbs(const pdsch_config_params& pdsch_cfg,
                                                  const ue_cell&             ue_cc,
                                                  unsigned                   pending_bytes,
                                                  unsigned                   max_rbs)
{
  const ue_cell_configuration& ue_cell_cfg = ue_cc.cfg();
  const cell_configuration&    cell_cfg    = ue_cell_cfg.cell_cfg_common;

  std::optional<sch_mcs_index> mcs = ue_cc.link_adaptation_controller().calculate_dl_mcs(pdsch_cfg.mcs_table);
  if (not mcs.has_value()) {
    // Return a grant with no PRBs if the MCS is invalid (CQI is either 0, for UE out of range, or > 15).
    return std::nullopt;
  }
  sch_mcs_description mcs_config = pdsch_mcs_get_config(pdsch_cfg.mcs_table, mcs.value());

  // Apply max RB grant size limits.
  const bwp_downlink_common& bwp_dl_cmn = *ue_cell_cfg.bwp(ue_cc.active_bwp_id()).dl_common.value();
  max_rbs                               = std::min({bwp_dl_cmn.generic_params.crbs.length(),
                                                    cell_cfg.expert_cfg.ue.pdsch_nof_rbs.stop(),
                                                    ue_cell_cfg.rrm_cfg().pdsch_grant_size_limits.stop(),
                                                    max_rbs});

  sch_prbs_tbs prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                                  pdsch_cfg.symbols.length(),
                                                                  calculate_nof_dmrs_per_rb(pdsch_cfg.dmrs),
                                                                  pdsch_cfg.nof_oh_prb,
                                                                  mcs_config,
                                                                  pdsch_cfg.nof_layers},
                                       max_rbs);
  srsran_sanity_check(prbs_tbs.nof_prbs <= max_rbs, "Error in RB computation");
  if (prbs_tbs.nof_prbs == 0) {
    return std::nullopt;
  }

  // Apply min RB grant size limits (max was applied before).
  prbs_tbs.nof_prbs = std::max({prbs_tbs.nof_prbs,
                                cell_cfg.expert_cfg.ue.pdsch_nof_rbs.start(),
                                ue_cell_cfg.rrm_cfg().pdsch_grant_size_limits.start()});

  // [Implementation-defined] In case of partial slots and nof. PRBs allocated equals to 1 probability of KO is
  // high due to code not being able to cope with interference. So the solution is to increase the PRB allocation
  // to greater than 1 PRB.
  if (prbs_tbs.nof_prbs == 1 and pdsch_cfg.symbols.length() < NOF_OFDM_SYM_PER_SLOT_NORMAL_CP) {
    prbs_tbs.nof_prbs = 2;
  }

  return mcs_prbs_selection{mcs.value(), prbs_tbs.nof_prbs};
}

static pusch_config_params compute_pusch_config_params(const ue_cell&                               ue_cc,
                                                       dci_ul_rnti_config_type                      dci_type,
                                                       unsigned                                     nof_layers,
                                                       const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                       unsigned                                     uci_bits,
                                                       bool                                         is_csi_report_slot)
{
  const ue_cell_configuration& ue_cell_cfg = ue_cc.cfg();
  const cell_configuration&    cell_cfg    = ue_cc.cfg().cell_cfg_common;
  const bwp_uplink_common&     bwp_ul_cmn  = *ue_cell_cfg.bwp(ue_cc.active_bwp_id()).ul_common.value();

  pusch_config_params pusch_cfg;
  switch (dci_type) {
    case dci_ul_rnti_config_type::tc_rnti_f0_0:
      pusch_cfg = get_pusch_config_f0_0_tc_rnti(cell_cfg, pusch_td_cfg);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_0:
      pusch_cfg =
          get_pusch_config_f0_0_c_rnti(cell_cfg, &ue_cell_cfg, bwp_ul_cmn, pusch_td_cfg, uci_bits, is_csi_report_slot);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_1:
      pusch_cfg = get_pusch_config_f0_1_c_rnti(ue_cell_cfg, pusch_td_cfg, nof_layers, uci_bits, is_csi_report_slot);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI UL format");
  }

  return pusch_cfg;
}

pusch_config_params
sched_helper::compute_newtx_pusch_config_params(const ue_cell&                               ue_cc,
                                                dci_ul_rnti_config_type                      dci_type,
                                                const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                unsigned                                     uci_bits,
                                                bool                                         is_csi_report_slot)
{
  return compute_pusch_config_params(
      ue_cc, dci_type, ue_cc.channel_state_manager().get_nof_ul_layers(), pusch_td_cfg, uci_bits, is_csi_report_slot);
}

pusch_config_params
sched_helper::compute_retx_pusch_config_params(const ue_cell&                               ue_cc,
                                               const ul_harq_process_handle&                h_ul,
                                               const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                               unsigned                                     uci_bits,
                                               bool                                         is_csi_report_slot)
{
  return compute_pusch_config_params(ue_cc,
                                     h_ul.get_grant_params().dci_cfg_type,
                                     h_ul.get_grant_params().nof_layers,
                                     pusch_td_cfg,
                                     uci_bits,
                                     is_csi_report_slot);
}

mcs_prbs_selection sched_helper::compute_newtx_required_mcs_and_prbs(const pusch_config_params& pusch_cfg,
                                                                     const ue_cell&             ue_cc,
                                                                     unsigned                   pending_bytes)
{
  const ue_cell_configuration& ue_cell_cfg = ue_cc.cfg();
  const cell_configuration&    cell_cfg    = ue_cc.cfg().cell_cfg_common;
  const bwp_uplink_common&     bwp_ul_cmn  = *ue_cell_cfg.bwp(ue_cc.active_bwp_id()).ul_common.value();

  sch_mcs_index       mcs = ue_cc.link_adaptation_controller().calculate_ul_mcs(pusch_cfg.mcs_table);
  sch_mcs_description mcs_config =
      pusch_mcs_get_config(pusch_cfg.mcs_table, mcs, pusch_cfg.use_transform_precoder, false);

  const auto nof_symbols = pusch_cfg.symbols.length();

  sch_prbs_tbs prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                                  nof_symbols,
                                                                  calculate_nof_dmrs_per_rb(pusch_cfg.dmrs),
                                                                  pusch_cfg.nof_oh_prb,
                                                                  mcs_config,
                                                                  pusch_cfg.nof_layers});

  // Apply grant size limits specified in the config.
  unsigned nof_prbs = std::min(prbs_tbs.nof_prbs, bwp_ul_cmn.generic_params.crbs.length());
  nof_prbs          = std::max(std::min(nof_prbs, cell_cfg.expert_cfg.ue.pusch_nof_rbs.stop()),
                      cell_cfg.expert_cfg.ue.pusch_nof_rbs.start());
  nof_prbs          = std::max(std::min(nof_prbs, ue_cell_cfg.rrm_cfg().pusch_grant_size_limits.stop()),
                      ue_cell_cfg.rrm_cfg().pusch_grant_size_limits.start());

  return mcs_prbs_selection{mcs, nof_prbs};
}

static std::optional<dl_grant_sched_params> compute_dl_grant_sched_params(const slice_ue&               u,
                                                                          slot_point                    pdcch_slot,
                                                                          slot_point                    pdsch_slot,
                                                                          const dl_harq_process_handle* h_dl,
                                                                          unsigned pending_bytes = 0,
                                                                          unsigned max_rbs       = MAX_NOF_PRBS)
{
  const ue_cell& ue_cc = u.get_cc();

  // Checks that should have been ensured at this point.
  srsran_sanity_check(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(), "Invalid UE state");
  srsran_sanity_check(ue_cc.is_pdcch_enabled(pdcch_slot), "DL is not active in the PDCCH slot");
  srsran_sanity_check(ue_cc.is_pdsch_enabled(pdsch_slot), "DL is not active in the PDSCH slot");

  // Verify only one PDSCH exists for the same RNTI in the same slot, and that the PDSCHs are in the same order as
  // PDCCHs.
  // [TS 38.214, 5.1] "For any HARQ process ID(s) in a given scheduled cell, the UE is not expected to receive a
  // PDSCH that overlaps in time with another PDSCH".
  // [TS 38.214, 5.1] "For any two HARQ process IDs in a given scheduled cell, if the UE is scheduled to start
  // receiving a first PDSCH starting in symbol j by a PDCCH ending in symbol i, the UE is not expected to be
  // scheduled to receive a PDSCH starting earlier than the end of the first PDSCH with a PDCCH that ends later
  // than symbol i.".
  slot_point last_pdsch_slot = ue_cc.harqs.last_pdsch_slot();
  if (last_pdsch_slot.valid() and pdsch_slot <= last_pdsch_slot) {
    return std::nullopt;
  }

  const ue_cell_configuration& ue_cell_cfg      = ue_cc.cfg();
  const cell_configuration&    cell_cfg         = ue_cell_cfg.cell_cfg_common;
  unsigned                     slot_nof_symbols = cell_cfg.get_nof_dl_symbol_per_slot(pdsch_slot);

  std::optional<dci_dl_rnti_config_type> preferred_dci_type;
  if (h_dl != nullptr) {
    // ReTx case.
    if (slot_nof_symbols < h_dl->get_grant_params().nof_symbols) {
      // Early exit if there are not enough symbols in the slot for the retransmission.
      return std::nullopt;
    }
    preferred_dci_type = h_dl->get_grant_params().dci_cfg_type;
  }

  auto ss_candidate_list = ue_cc.get_active_dl_search_spaces(pdcch_slot, preferred_dci_type);
  for (const search_space_info* ss_candidate : ss_candidate_list) {
    // Apply RB allocation limits.
    const unsigned start_rb =
        std::max(cell_cfg.expert_cfg.ue.pdsch_crb_limits.start(), ss_candidate->dl_crb_lims.start());
    const unsigned end_rb = std::min(cell_cfg.expert_cfg.ue.pdsch_crb_limits.stop(), ss_candidate->dl_crb_lims.stop());
    if (start_rb >= end_rb) {
      // Invalid RB allocation range.
      continue;
    }

    for (unsigned pdsch_td_index = 0, e = ss_candidate->pdsch_time_domain_list.size(); pdsch_td_index != e;
         ++pdsch_td_index) {
      const pdsch_time_domain_resource_allocation& pdsch_td_res = ss_candidate->pdsch_time_domain_list[pdsch_td_index];

      // Check that k0 matches the chosen PDSCH slot
      if (pdcch_slot + pdsch_td_res.k0 != pdsch_slot) {
        continue;
      }

      // If it is a retx, we need to ensure we use a time_domain_resource with the same number of symbols as used for
      // the first transmission.
      if (h_dl != nullptr and pdsch_td_res.symbols.length() != h_dl->get_grant_params().nof_symbols) {
        continue;
      }

      // Check whether PDSCH time domain resource fits in DL symbols of the slot.
      if (slot_nof_symbols < pdsch_td_res.symbols.length()) {
        continue;
      }

      // Check whether PDSCH time domain resource does not overlap with CORESET.
      if (pdsch_td_res.symbols.start() <
          ss_candidate->cfg->get_first_symbol_index() + ss_candidate->coreset->duration) {
        continue;
      }

      // Compute recommended number of layers, MCS and PRBs.
      unsigned      nof_layers;
      unsigned      nof_rbs;
      sch_mcs_index mcs;
      if (h_dl == nullptr) {
        // NewTx Case.
        nof_layers                           = ue_cc.channel_state_manager().get_nof_dl_layers();
        const pdsch_config_params& pdsch_cfg = ss_candidate->get_pdsch_config(pdsch_td_index, nof_layers);
        auto mcs_prbs_sel = compute_newtx_required_mcs_and_prbs(pdsch_cfg, ue_cc, pending_bytes, max_rbs);
        if (not mcs_prbs_sel.has_value()) {
          continue;
        }
        mcs     = mcs_prbs_sel.value().mcs;
        nof_rbs = mcs_prbs_sel.value().nof_prbs;
      } else {
        // ReTx Case.
        nof_layers = h_dl->get_grant_params().nof_layers;
        mcs        = h_dl->get_grant_params().mcs;
        nof_rbs    = h_dl->get_grant_params().rbs.type1().length();
      }

      // Successful selection of grant parameters.
      dl_grant_sched_params params;
      params.ss_id              = ss_candidate->cfg->get_id();
      params.pdsch_td_res_index = pdsch_td_index;
      params.mcs                = mcs;
      params.nof_rbs            = nof_rbs;
      params.nof_layers         = nof_layers;
      params.crb_lims           = {start_rb, end_rb};
      return params;
    }
  }

  return std::nullopt;
}

std::optional<dl_grant_sched_params>
sched_helper::compute_newtx_dl_grant_sched_params(const slice_ue&                u,
                                                  slot_point                     pdcch_slot,
                                                  slot_point                     pdsch_slot,
                                                  unsigned                       pending_bytes,
                                                  const std::optional<unsigned>& max_rbs)
{
  return compute_dl_grant_sched_params(
      u, pdcch_slot, pdsch_slot, nullptr, pending_bytes, max_rbs.value_or(MAX_NOF_PRBS));
}

std::optional<dl_grant_sched_params>
sched_helper::compute_retx_dl_grant_sched_params(const slice_ue&               u,
                                                 slot_point                    pdcch_slot,
                                                 slot_point                    pdsch_slot,
                                                 const dl_harq_process_handle& h_dl)
{
  return compute_dl_grant_sched_params(u, pdcch_slot, pdsch_slot, &h_dl);
}
