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
  ret.ue_index      = request.ue_index;
  ret.crnti         = request.crnti;
  ret.pcell_index   = request.cell_index;
  ret.serv_cell_cfg = request.serv_cell_cfg;

  return ret;
}
