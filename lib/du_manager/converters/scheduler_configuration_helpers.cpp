/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_configuration_helpers.h"
#include "../du_ue/du_ue.h"
#include "srsgnb/du/du_cell_config.h"
#include "srsgnb/scheduler/config/logical_channel_config_factory.h"
#include "srsgnb/scheduler/config/sched_cell_config_helpers.h"

using namespace srsgnb;
using namespace srs_du;

/// Derives Scheduler Cell Configuration from DU Cell Configuration.
sched_cell_configuration_request_message srsgnb::srs_du::make_sched_cell_config_req(du_cell_index_t       cell_index,
                                                                                    const du_cell_config& du_cfg,
                                                                                    unsigned sib1_payload_size)
{
  sched_cell_configuration_request_message sched_req{};
  sched_req.cell_index           = cell_index;
  sched_req.pci                  = du_cfg.pci;
  sched_req.dl_carrier           = du_cfg.dl_carrier;
  sched_req.ul_carrier           = du_cfg.ul_carrier;
  sched_req.dl_cfg_common        = du_cfg.dl_cfg_common;
  sched_req.ul_cfg_common        = du_cfg.ul_cfg_common;
  sched_req.scs_common           = du_cfg.scs_common;
  sched_req.ssb_config           = du_cfg.ssb_cfg;
  sched_req.dmrs_typeA_pos       = du_cfg.dmrs_typeA_pos;
  sched_req.tdd_ul_dl_cfg_common = du_cfg.tdd_ul_dl_cfg_common;

  sched_req.nof_beams     = 1;
  sched_req.nof_layers    = 1;
  sched_req.nof_ant_ports = 1;

  /// SIB1 parameters.
  sched_req.coreset0          = du_cfg.coreset0_idx;
  sched_req.searchspace0      = du_cfg.searchspace0_idx;
  sched_req.sib1_payload_size = sib1_payload_size;

  sched_req.pucch_guardbands = config_helpers::build_pucch_guardbands_list(*du_cfg.ue_ded_serv_cell_cfg.ul_config);

  sched_req.csi_meas_cfg = du_cfg.ue_ded_serv_cell_cfg.csi_meas_cfg;

  return sched_req;
}

sched_ue_config_request srsgnb::srs_du::create_scheduler_ue_config_request(const du_ue& ue_ctx)
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
  for (const du_ue_drb& bearer : ue_ctx.bearers.drbs()) {
    sched_cfg.lc_config_list.emplace_back(config_helpers::create_default_logical_channel_config(bearer.lcid));
    auto& sched_lc_ch = sched_cfg.lc_config_list.back();
    sched_lc_ch.sr_id.emplace(sched_cfg.sched_request_config_list.back().sr_id);
  }

  return sched_cfg;
}
