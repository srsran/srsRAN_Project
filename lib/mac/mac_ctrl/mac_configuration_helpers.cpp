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

  ret.dmrs_typeA_pos        = dmrs_typeA_position::pos2;
  ret.pdcch_config_sib1     = request.pdcch_config_sib1;
  ret.sib1_mcs              = request.sib1_mcs;
  ret.sib1_rv               = request.sib1_rv;
  ret.sib1_dci_aggr_lev     = request.sib1_dci_aggr_lev;
  ret.sib1_retx_periodicity = request.sib1_rxtx_periodicity;

  return ret;
}

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
