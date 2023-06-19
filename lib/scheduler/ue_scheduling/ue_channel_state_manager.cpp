/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_channel_state_manager.h"

using namespace srsran;

/// Find the number of DL ports for a given UE serving cell configuration.
static unsigned compute_nof_dl_ports(const serving_cell_config& serv_cell_cfg)
{
  if (not serv_cell_cfg.csi_meas_cfg.has_value()) {
    return 1;
  }

  unsigned max_ports = 1;
  for (const auto& nzp : serv_cell_cfg.csi_meas_cfg->nzp_csi_rs_res_list) {
    if (max_ports < nzp.res_mapping.nof_ports) {
      max_ports = nzp.res_mapping.nof_ports;
    }
  }
  return max_ports;
}

ue_channel_state_manager::ue_channel_state_manager(const serving_cell_config&        ue_serv_cell,
                                                   const scheduler_ue_expert_config& expert_cfg) :
  nof_dl_ports(compute_nof_dl_ports(ue_serv_cell)),
  pusch_snr_db(expert_cfg.initial_ul_sinr),
  wideband_cqi(expert_cfg.initial_cqi)
{
  // Set initial precoding value when no CSI has yet been received.
  if (nof_dl_ports == 2) {
    recommended_prg_info.resize(2, pdsch_precoding_info::prg_info{pdsch_precoding_info::prg_info::two_antenna_port{0}});
  } else if (nof_dl_ports == 4) {
    recommended_prg_info.resize(3,
                                pdsch_precoding_info::prg_info{
                                    pdsch_precoding_info::prg_info::typeI_single_panel_4ports_mode1{0, 0, nullopt, 0}});
  }
}

void ue_channel_state_manager::handle_csi_report(const csi_report_data& csi_report)
{
  latest_csi_report = csi_report;

  // Set wideband CQI.
  if (csi_report.first_tb_wideband_cqi.has_value()) {
    wideband_cqi = csi_report.first_tb_wideband_cqi.value();
  }

  // Update recommended number of layers based on RI.
  if (csi_report.ri.has_value()) {
    recommended_dl_layers = csi_report.ri.value().to_uint();
    srsran_sanity_check(recommended_dl_layers < nof_dl_ports, "Invalid RI reported");
  }

  if (csi_report.pmi.has_value()) {
    const unsigned table_idx        = nof_layers_to_index(recommended_dl_layers);
    recommended_prg_info[table_idx] = csi_report.pmi.value();
  }
}
