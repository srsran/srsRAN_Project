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
#include "srsgnb/scheduler/config/logical_channel_config_factory.h"

using namespace srsgnb;
using namespace srs_du;

sched_ue_config_request srsgnb::srs_du::create_scheduler_ue_config_request(const du_ue& ue_ctx)
{
  sched_ue_config_request sched_cfg;

  sched_cfg.cells.resize(1);
  sched_cfg.cells[0]                  = ue_ctx.resources->cells[0];
  sched_cfg.sched_request_config_list = ue_ctx.resources->mcg_cfg.scheduling_request_config;
  // Add SRB and DRB logical channels.
  for (const du_ue_srb& bearer : ue_ctx.bearers.srbs()) {
    sched_cfg.lc_config_list.emplace_back(config_helpers::make_default_logical_channel_config(bearer.lcid()));
    auto& sched_lc_ch = sched_cfg.lc_config_list.back();
    sched_lc_ch.sr_id.emplace(sched_cfg.sched_request_config_list.back().sr_id);
  }
  for (const du_ue_drb& bearer : ue_ctx.bearers.drbs()) {
    sched_cfg.lc_config_list.emplace_back(config_helpers::make_default_logical_channel_config(bearer.lcid));
    auto& sched_lc_ch = sched_cfg.lc_config_list.back();
    sched_lc_ch.sr_id.emplace(sched_cfg.sched_request_config_list.back().sr_id);
  }

  return sched_cfg;
}
