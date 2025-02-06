/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_cell_group_config_pool.h"
#include "srsran/scheduler/scheduler_configurator.h"

using namespace srsran;

ue_creation_params du_cell_group_config_pool::add_ue(const sched_ue_creation_request_message& cfg_req)
{
  // Create logical channel config.
  auto lc_ch_list = lc_ch_pool.create(cfg_req.cfg.lc_config_list.has_value() ? cfg_req.cfg.lc_config_list.value()
                                                                             : std::vector<logical_channel_config>{});

  return ue_creation_params{cfg_req.cfg, lc_ch_list};
}

ue_reconfig_params du_cell_group_config_pool::reconf_ue(const sched_ue_reconfiguration_message& cfg_req)
{
  std::optional<logical_channel_config_list_ptr> lc_ch_list;
  if (cfg_req.cfg.lc_config_list.has_value()) {
    lc_ch_list = lc_ch_pool.create(cfg_req.cfg.lc_config_list.value());
  }

  return ue_reconfig_params{cfg_req.cfg, lc_ch_list};
}
