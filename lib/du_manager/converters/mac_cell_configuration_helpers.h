/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../du_manager_config.h"
#include "f1c_configuration_helpers.h"
#include "srsgnb/asn1/asn1_utils.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/mac/cell_configuration.h"
#include "srsgnb/mac/config/mac_cell_group_config_factory.h"
#include "srsgnb/mac/mac_ue_configurator.h"
#include "srsgnb/ran/tdd_ul_dl_config.h"

// TODO: This file is temporary. Eventually we will receive cell configurations from the DU config file.

namespace srsgnb {

namespace test_helpers {

inline mac_ue_create_request_message make_default_ue_creation_request()
{
  mac_ue_create_request_message msg{};

  msg.ue_index   = to_du_ue_index(0);
  msg.crnti      = to_rnti(0x4601);
  msg.cell_index = to_du_cell_index(0);

  msg.serv_cell_cfg.emplace(config_helpers::make_default_initial_ue_serving_cell_config());

  msg.mac_cell_group_cfg = config_helpers::make_initial_mac_cell_group_config();

  physical_cell_group_config& pcg_cfg = msg.phy_cell_group_cfg;
  pcg_cfg.p_nr_fr1                    = 10;
  pcg_cfg.pdsch_harq_codebook         = pdsch_harq_ack_codebook::dynamic;

  return msg;
}

} // namespace test_helpers

/// Derives Scheduler Cell Configuration from DU Cell Configuration.
inline sched_cell_configuration_request_message
make_sched_cell_config_req(du_cell_index_t cell_index, const du_cell_config& du_cfg, unsigned sib1_payload_size)
{
  sched_cell_configuration_request_message sched_req{};
  sched_req.cell_index     = cell_index;
  sched_req.pci            = du_cfg.pci;
  sched_req.dl_carrier     = du_cfg.dl_carrier;
  sched_req.ul_carrier     = du_cfg.ul_carrier;
  sched_req.dl_cfg_common  = du_cfg.dl_cfg_common;
  sched_req.ul_cfg_common  = du_cfg.ul_cfg_common;
  sched_req.scs_common     = du_cfg.scs_common;
  sched_req.ssb_config     = du_cfg.ssb_cfg;
  sched_req.dmrs_typeA_pos = du_cfg.dmrs_typeA_pos;

  sched_req.nof_beams     = 1;
  sched_req.nof_ant_ports = 1;
  sched_req.nof_ant_ports = 1;

  /// SIB1 parameters.
  sched_req.coreset0          = du_cfg.coreset0_idx;
  sched_req.searchspace0      = du_cfg.searchspace0_idx;
  sched_req.sib1_payload_size = sib1_payload_size;

  sched_req.pucch_guardbands = du_cfg.pucch_guardbands;

  return sched_req;
}

/// Derives MAC Cell Configuration from DU Cell Configuration.
inline mac_cell_creation_request make_mac_cell_config(du_cell_index_t cell_index, const du_cell_config& du_cfg)
{
  mac_cell_creation_request mac_cfg{};
  mac_cfg.cell_index       = cell_index;
  mac_cfg.pci              = du_cfg.pci;
  mac_cfg.scs_common       = du_cfg.scs_common;
  mac_cfg.ssb_cfg          = du_cfg.ssb_cfg;
  mac_cfg.dl_carrier       = du_cfg.dl_carrier;
  mac_cfg.ul_carrier       = du_cfg.ul_carrier;
  mac_cfg.cell_barred      = du_cfg.cell_barred;
  mac_cfg.intra_freq_resel = du_cfg.intra_freq_resel;
  mac_cfg.bcch_dl_sch_payload.append(srs_du::make_asn1_rrc_cell_bcch_dl_sch_msg(du_cfg));
  mac_cfg.sched_req =
      make_sched_cell_config_req(cell_index, du_cfg, static_cast<unsigned>(mac_cfg.bcch_dl_sch_payload.length()));
  return mac_cfg;
}

} // namespace srsgnb
