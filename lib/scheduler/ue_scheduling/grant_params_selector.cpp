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
#include "../support/dmrs_helpers.h"
#include "../support/prbs_calculator.h"
#include "../ue_context/ue_cell.h"

using namespace srsran;
using namespace sched_helper;

static pdsch_config_params compute_pdsch_config_params(const ue_cell&                               ue_cc,
                                                       dci_dl_rnti_config_type                      dci_type,
                                                       unsigned                                     nof_layers,
                                                       const pdsch_time_domain_resource_allocation& pdsch_td_cfg)
{
  const ue_cell_configuration& ue_cell_cfg = ue_cc.cfg();
  const cell_configuration&    cell_cfg    = ue_cell_cfg.cell_cfg_common;

  pdsch_config_params pdsch_cfg;
  switch (dci_type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0:
      pdsch_cfg = get_pdsch_config_f1_0_tc_rnti(cell_cfg, pdsch_td_cfg);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      pdsch_cfg = get_pdsch_config_f1_0_c_rnti(cell_cfg, &ue_cell_cfg, pdsch_td_cfg);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_1:
      pdsch_cfg = get_pdsch_config_f1_1_c_rnti(ue_cell_cfg, pdsch_td_cfg, nof_layers);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI DL format");
  }

  return pdsch_cfg;
}

pdsch_config_params
sched_helper::compute_newtx_pdsch_config_params(const ue_cell&                               ue_cc,
                                                dci_dl_rnti_config_type                      dci_type,
                                                const pdsch_time_domain_resource_allocation& pdsch_td_cfg)
{
  return compute_pdsch_config_params(ue_cc, dci_type, ue_cc.channel_state_manager().get_nof_dl_layers(), pdsch_td_cfg);
}

pdsch_config_params
sched_helper::compute_retx_pdsch_config_params(const ue_cell&                               ue_cc,
                                               const dl_harq_process_handle&                h_dl,
                                               const pdsch_time_domain_resource_allocation& pdsch_td_cfg)
{
  return compute_pdsch_config_params(
      ue_cc, h_dl.get_grant_params().dci_cfg_type, h_dl.get_grant_params().nof_layers, pdsch_td_cfg);
}

std::optional<mcs_prbs_selection>
sched_helper::compute_newtx_required_mcs_and_prbs(const pdsch_config_params& pdsch_cfg,
                                                  const ue_cell&             ue_cc,
                                                  unsigned                   pending_bytes)
{
  const ue_cell_configuration& ue_cfg   = ue_cc.cfg();
  const cell_configuration&    cell_cfg = ue_cfg.cell_cfg_common;

  std::optional<sch_mcs_index> mcs = ue_cc.link_adaptation_controller().calculate_dl_mcs(pdsch_cfg.mcs_table);
  if (not mcs.has_value()) {
    // Return a grant with no PRBs if the MCS is invalid (CQI is either 0, for UE out of range, or > 15).
    return std::nullopt;
  }
  sch_mcs_description mcs_config = pdsch_mcs_get_config(pdsch_cfg.mcs_table, mcs.value());

  // Estimate max RBs per grant.
  const bwp_downlink_common& bwp_dl_cmn = *ue_cfg.bwp(ue_cc.active_bwp_id()).dl_common;
  unsigned                   max_prbs   = std::min({bwp_dl_cmn.generic_params.crbs.length(),
                                                    cell_cfg.expert_cfg.ue.pdsch_nof_rbs.stop(),
                                                    ue_cfg.rrm_cfg().pdsch_grant_size_limits.stop()});

  sch_prbs_tbs prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                                  pdsch_cfg.symbols.length(),
                                                                  calculate_nof_dmrs_per_rb(pdsch_cfg.dmrs),
                                                                  pdsch_cfg.nof_oh_prb,
                                                                  mcs_config,
                                                                  pdsch_cfg.nof_layers},
                                       max_prbs);
  srsran_sanity_check(prbs_tbs.nof_prbs <= max_prbs, "Error in RB computation");
  if (prbs_tbs.nof_prbs == 0) {
    return std::nullopt;
  }

  // Apply grant size limits specified in the config.
  prbs_tbs.nof_prbs = std::max({prbs_tbs.nof_prbs,
                                cell_cfg.expert_cfg.ue.pdsch_nof_rbs.start(),
                                ue_cfg.rrm_cfg().pdsch_grant_size_limits.start()});

  return mcs_prbs_selection{mcs.value(), prbs_tbs.nof_prbs};
}
