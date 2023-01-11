/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/mac/mac_configuration_helpers.h"

using namespace srsgnb;

sched_ue_creation_request_message
srsgnb::make_scheduler_ue_creation_request(const mac_ue_create_request_message& request)
{
  sched_ue_creation_request_message ret{};
  ret.ue_index = request.ue_index;
  ret.crnti    = request.crnti;

  ret.lc_config_list.resize(request.bearers.size());
  for (unsigned i = 0; i != request.bearers.size(); ++i) {
    ret.lc_config_list[i] = request.bearers[i].lc_config;
  }

  ret.sched_request_config_list = request.mac_cell_group_cfg.scheduling_request_config;

  ret.cells.resize(1);
  serving_cell_ue_configuration_request& pcell = ret.cells[0];
  pcell.cell_index                             = request.cell_index;
  pcell.serv_cell_cfg                          = request.serv_cell_cfg;

  return ret;
}

sched_ue_reconfiguration_message
srsgnb::make_scheduler_ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& request)
{
  sched_ue_reconfiguration_message ret{};
  ret.ue_index    = request.ue_index;
  ret.crnti       = request.crnti;
  ret.pcell_index = request.pcell_index;

  ret.lc_config_list.resize(request.bearers.size());
  for (unsigned i = 0; i != request.bearers.size(); ++i) {
    ret.lc_config_list[i] = request.bearers[i].lc_config;
  }

  ret.sched_request_config_list = request.mac_cell_group_cfg.scheduling_request_config;

  ret.cells.resize(1);
  serving_cell_ue_configuration_request& pcell = ret.cells[0];
  pcell.cell_index                             = request.pcell_index;
  pcell.serv_cell_cfg                          = request.serv_cell_cfg;

  return ret;
}
