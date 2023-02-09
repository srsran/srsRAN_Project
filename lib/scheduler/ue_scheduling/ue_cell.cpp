/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_cell.h"
#include "../support/dmrs_helpers.h"
#include "../support/mcs_calculator.h"
#include "../support/prbs_calculator.h"
#include "../ue_scheduling/ue_sch_pdu_builder.h"
#include "srsgnb/scheduler/scheduler_feedback_handler.h"

using namespace srsgnb;

/// Number of UL HARQs reserved per UE (Implementation-defined)
constexpr unsigned NOF_UL_HARQS = 16;

ue_cell::ue_cell(du_ue_index_t                     ue_index_,
                 rnti_t                            crnti_val,
                 const scheduler_ue_expert_config& expert_cfg_,
                 const cell_configuration&         cell_cfg_common_,
                 const serving_cell_config&        ue_serv_cell) :
  ue_index(ue_index_),
  cell_index(ue_serv_cell.cell_index),
  harqs(crnti_val, (unsigned)ue_serv_cell.pdsch_serv_cell_cfg->nof_harq_proc, NOF_UL_HARQS),
  crnti_(crnti_val),
  expert_cfg(expert_cfg_),
  ue_cfg(cell_cfg_common_, ue_serv_cell)
{
  if (not expert_cfg.fixed_ul_mcs.has_value()) {
    update_pusch_snr(expert_cfg.initial_ul_sinr);
  }
}

void ue_cell::handle_reconfiguration_request(const serving_cell_config& new_ue_cell_cfg)
{
  ue_cfg.reconfigure(new_ue_cell_cfg);
}

unsigned ue_cell::required_dl_prbs(unsigned time_resource, unsigned pending_bytes) const
{
  static const unsigned     nof_layers = 1;
  const cell_configuration& cell_cfg   = cfg().cell_cfg_common;

  sch_mcs_index       mcs        = expert_cfg.fixed_dl_mcs.value(); // TODO: Support dynamic MCS.
  sch_mcs_description mcs_config = pdsch_mcs_get_config(cfg().cfg_dedicated().init_dl_bwp.pdsch_cfg->mcs_table, mcs);

  dmrs_information dmrs_info = make_dmrs_info_common(
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common, time_resource, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);
  // According to TS 38.214, Section 5.1.3.2, nof_oh_prb is set equal to xOverhead, when set; else nof_oh_prb = 0.
  // NOTE: x_overhead::not_set is mapped to 0.
  unsigned     nof_oh_prb = cfg().cfg_dedicated().pdsch_serv_cell_cfg.has_value()
                                ? static_cast<unsigned>(cfg().cfg_dedicated().pdsch_serv_cell_cfg.value().x_ov_head)
                                : static_cast<unsigned>(x_overhead::not_set);
  sch_prbs_tbs prbs_tbs   = get_nof_prbs(prbs_calculator_sch_config{
      pending_bytes,
      (unsigned)cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[time_resource].symbols.length(),
      calculate_nof_dmrs_per_rb(dmrs_info),
      nof_oh_prb,
      mcs_config,
      nof_layers});
  return prbs_tbs.nof_prbs;
}

grant_prbs_mcs
ue_cell::required_ul_prbs(unsigned time_resource, unsigned pending_bytes, dci_ul_rnti_config_type type) const
{
  const cell_configuration& cell_cfg = cfg().cell_cfg_common;

  const bwp_uplink_common& bwp_ul_cmn = ue_cfg.ul_bwp_common(active_bwp_id());

  pusch_config_params pusch_cfg;
  switch (type) {
    case dci_ul_rnti_config_type::tc_rnti_f0_0:
      pusch_cfg = get_pusch_config_f0_0_tc_rnti(cell_cfg, time_resource);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_0:
      pusch_cfg = get_pusch_config_f0_0_c_rnti(cell_cfg, ue_cfg, bwp_ul_cmn, time_resource);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI UL format");
  }

  double        ul_snr{ue_metrics.pusch_snr_db};
  sch_mcs_index mcs = expert_cfg.fixed_ul_mcs.has_value() ? expert_cfg.fixed_ul_mcs.value() : map_snr_to_mcs_ul(ul_snr);

  sch_mcs_description mcs_config = pusch_mcs_get_config(pusch_cfg.mcs_table, mcs, false);

  const unsigned nof_symbols = static_cast<unsigned>(
      cfg()
          .cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[time_resource]
          .symbols.length());

  sch_prbs_tbs prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                                  nof_symbols,
                                                                  calculate_nof_dmrs_per_rb(pusch_cfg.dmrs),
                                                                  pusch_cfg.nof_oh_prb,
                                                                  mcs_config,
                                                                  pusch_cfg.nof_layers});

  return grant_prbs_mcs{mcs, std::min(prbs_tbs.nof_prbs, bwp_ul_cmn.generic_params.crbs.length())};
}

int ue_cell::handle_crc_pdu(slot_point pusch_slot, const ul_crc_pdu_indication& crc_pdu)
{
  // Update UL HARQ state.
  int tbs = harqs.ul_crc_info(crc_pdu.harq_id, crc_pdu.tb_crc_success, pusch_slot);
  if (tbs >= 0) {
    // HARQ with matching ID and UCI slot was found.

    // Update PUSCH KO count metrics.
    ue_metrics.consecutive_pusch_kos = (crc_pdu.tb_crc_success) ? 0 : ue_metrics.consecutive_pusch_kos + 1;

    // Update PUSCH SNR reported from PHY.
    update_pusch_snr(crc_pdu.ul_sinr_metric);
  }

  return tbs;
}
