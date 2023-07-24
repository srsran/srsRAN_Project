/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ue_channel_state_manager.h"

using namespace srsran;

ue_channel_state_manager::ue_channel_state_manager(const scheduler_ue_expert_config& expert_cfg,
                                                   unsigned                          nof_dl_ports_) :
  nof_dl_ports(nof_dl_ports_), pusch_snr_db(expert_cfg.initial_ul_sinr), wideband_cqi(expert_cfg.initial_cqi)
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

bool ue_channel_state_manager::handle_csi_report(const csi_report_data& csi_report)
{
  latest_csi_report = csi_report;

  // Set wideband CQI.
  if (csi_report.first_tb_wideband_cqi.has_value()) {
    wideband_cqi = csi_report.first_tb_wideband_cqi.value();
  }

  // Update recommended number of layers based on RI.
  if (csi_report.ri.has_value()) {
    if (csi_report.ri.value() > nof_dl_ports) {
      return false;
    }
    recommended_dl_layers = csi_report.ri.value().to_uint();
  }

  if (csi_report.pmi.has_value()) {
    if (nof_dl_ports <= 1) {
      return false;
    }
    const unsigned table_idx        = nof_layers_to_index(recommended_dl_layers);
    recommended_prg_info[table_idx] = csi_report.pmi.value();
  }

  return true;
}
