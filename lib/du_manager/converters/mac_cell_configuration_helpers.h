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

  mac_cell_group_config&       mcg_cfg = msg.mac_cell_group_cfg;
  scheduling_request_to_addmod sr_0{.sr_id = scheduling_request_id::SR_ID_MIN, .max_tx = sr_max_tx::n64};
  mcg_cfg.scheduling_request_config.emplace_back(sr_0);

  mcg_cfg.bsr_cfg.emplace();
  mcg_cfg.bsr_cfg.value().periodic_timer = periodic_bsr_timer::sf10;
  mcg_cfg.bsr_cfg.value().retx_timer     = retx_bsr_timer::sf80;

  mcg_cfg.tag_config.emplace_back(tag{.tag_id = uint_to_tag_id(0), .ta_timer = time_alignment_timer::infinity});

  mcg_cfg.phr_cfg.emplace();
  mcg_cfg.phr_cfg.value().periodic_timer         = phr_periodic_timer::sf10;
  mcg_cfg.phr_cfg.value().prohibit_timer         = phr_prohibit_timer::sf10;
  mcg_cfg.phr_cfg.value().power_factor_change    = phr_tx_power_factor_change::db1;
  mcg_cfg.phr_cfg.value().multiple_phr           = false;
  mcg_cfg.phr_cfg.value().dummy                  = false;
  mcg_cfg.phr_cfg.value().phr_type_to_other_cell = false;
  mcg_cfg.phr_cfg.value().phr_mode               = phr_mode_other_cg::real;

  mcg_cfg.skip_uplink_tx_dynamic = false;

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

  /// Msg4 parameters.
  /// Note: A MCS index of 7 can handle Msg4 of size 458 bytes.
  sched_req.max_msg4_mcs_index = 7;

  /// SIB1 parameters.
  sched_req.coreset0          = du_cfg.coreset0_idx;
  sched_req.searchspace0      = du_cfg.searchspace0_idx;
  sched_req.sib1_mcs          = 5;
  sched_req.sib1_rv           = 0;
  sched_req.sib1_dci_aggr_lev = aggregation_level::n4;
  sched_req.sib1_retx_period  = sib1_rtx_periodicity::ms160;
  sched_req.sib1_payload_size = sib1_payload_size;

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
