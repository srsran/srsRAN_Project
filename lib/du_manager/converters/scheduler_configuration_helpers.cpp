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

#include "scheduler_configuration_helpers.h"
#include "../du_ue/du_ue.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/scheduler/config/logical_channel_config_factory.h"
#include "srsran/scheduler/config/sched_cell_config_helpers.h"

using namespace srsran;
using namespace srs_du;

/// Derives Scheduler Cell Configuration from DU Cell Configuration.
sched_cell_configuration_request_message srsran::srs_du::make_sched_cell_config_req(du_cell_index_t       cell_index,
                                                                                    const du_cell_config& du_cfg,
                                                                                    unsigned sib1_payload_size)
{
  sched_cell_configuration_request_message sched_req{};
  sched_req.cell_index           = cell_index;
  sched_req.cell_group_index     = (du_cell_group_index_t)0;
  sched_req.pci                  = du_cfg.pci;
  sched_req.dl_carrier           = du_cfg.dl_carrier;
  sched_req.ul_carrier           = du_cfg.ul_carrier;
  sched_req.dl_cfg_common        = du_cfg.dl_cfg_common;
  sched_req.ul_cfg_common        = du_cfg.ul_cfg_common;
  sched_req.scs_common           = du_cfg.scs_common;
  sched_req.ssb_config           = du_cfg.ssb_cfg;
  sched_req.dmrs_typeA_pos       = du_cfg.dmrs_typeA_pos;
  sched_req.tdd_ul_dl_cfg_common = du_cfg.tdd_ul_dl_cfg_common;

  sched_req.nof_beams = 1;

  /// SIB1 parameters.
  sched_req.coreset0          = du_cfg.coreset0_idx;
  sched_req.searchspace0      = du_cfg.searchspace0_idx;
  sched_req.sib1_payload_size = sib1_payload_size;

  sched_req.pucch_guardbands = config_helpers::build_pucch_guardbands_list(
      du_cfg.pucch_cfg, du_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length());

  sched_req.zp_csi_rs_list = du_cfg.ue_ded_serv_cell_cfg.init_dl_bwp.pdsch_cfg->zp_csi_rs_res_list;

  if (du_cfg.ue_ded_serv_cell_cfg.csi_meas_cfg.has_value()) {
    sched_req.nzp_csi_rs_res_list = du_cfg.ue_ded_serv_cell_cfg.csi_meas_cfg->nzp_csi_rs_res_list;
  }

  return sched_req;
}

sched_ue_config_request srsran::srs_du::create_scheduler_ue_config_request(const du_ue& ue_ctx)
{
  sched_ue_config_request sched_cfg;

  sched_cfg.cells.resize(1);
  sched_cfg.cells[0]                  = ue_ctx.resources->cells[0];
  sched_cfg.sched_request_config_list = ue_ctx.resources->mcg_cfg.scheduling_request_config;
  // Add SRB and DRB logical channels.
  for (const du_ue_srb& bearer : ue_ctx.bearers.srbs()) {
    sched_cfg.lc_config_list.emplace_back(config_helpers::create_default_logical_channel_config(bearer.lcid()));
    auto& sched_lc_ch = sched_cfg.lc_config_list.back();
    sched_lc_ch.sr_id.emplace(sched_cfg.sched_request_config_list.back().sr_id);
  }
  for (const auto& bearer : ue_ctx.bearers.drbs()) {
    sched_cfg.lc_config_list.emplace_back(config_helpers::create_default_logical_channel_config(bearer.second->lcid));
    auto& sched_lc_ch = sched_cfg.lc_config_list.back();
    sched_lc_ch.sr_id.emplace(sched_cfg.sched_request_config_list.back().sr_id);
  }

  return sched_cfg;
}
