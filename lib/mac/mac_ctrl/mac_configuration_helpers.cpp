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

sched_cell_configuration_request_message
srsgnb::make_scheduler_cell_configuration_request(const mac_cell_creation_request& request)
{
  sched_cell_configuration_request_message ret{};

  ret.cell_index    = request.cell_index;
  ret.nof_beams     = 1;
  ret.nof_layers    = 1;
  ret.nof_ant_ports = 1;
  ret.pci           = request.pci;

  ret.scs_common = request.scs_common;
  ret.ssb_config = request.ssb_cfg;

  ret.dl_carrier = request.dl_carrier;
  ret.ul_carrier = request.ul_carrier;

  ret.dl_cfg_common = request.dl_cfg_common;
  ret.ul_cfg_common = request.ul_cfg_common;

  return ret;
}
