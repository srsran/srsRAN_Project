/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/du_manager/converters/scheduler_configuration_helpers.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/mac/mac_configuration_helpers.h"
#include "srsgnb/scheduler/config/logical_channel_config_factory.h"
#include "srsgnb/scheduler/config/sched_cell_config_helpers.h"
#include "srsgnb/scheduler/config/serving_cell_config.h"
#include "srsgnb/scheduler/config/serving_cell_config_factory.h"
#include "srsgnb/scheduler/mac_scheduler.h"

namespace srsgnb {
namespace test_helpers {

inline sched_cell_configuration_request_message
make_default_sched_cell_configuration_request(const cell_config_builder_params& params = {})
{
  sched_cell_configuration_request_message sched_req{};
  sched_req.cell_index     = to_du_cell_index(0);
  sched_req.pci            = params.pci;
  sched_req.scs_common     = params.scs_common;
  sched_req.dl_carrier     = config_helpers::make_default_carrier_configuration(params);
  sched_req.ul_carrier     = config_helpers::make_default_carrier_configuration(params);
  sched_req.dl_cfg_common  = config_helpers::make_default_dl_config_common(params);
  sched_req.ul_cfg_common  = config_helpers::make_default_ul_config_common(params);
  sched_req.ssb_config     = config_helpers::make_default_ssb_config(params);
  sched_req.dmrs_typeA_pos = dmrs_typeA_position::pos2;
  if (not band_helper::is_paired_spectrum(sched_req.dl_carrier.band)) {
    sched_req.tdd_ul_dl_cfg_common = config_helpers::make_default_tdd_ul_dl_config_common(params);
  }

  sched_req.nof_beams     = 1;
  sched_req.nof_layers    = 1;
  sched_req.nof_ant_ports = 1;

  // SIB1 parameters.
  sched_req.coreset0          = params.coreset0_index;
  sched_req.searchspace0      = 0U;
  sched_req.sib1_payload_size = 101; // Random size.

  sched_req.pucch_guardbands =
      config_helpers::build_pucch_guardbands_list(config_helpers::make_default_ue_uplink_config(params));

  if (params.csi_rs_enabled) {
    sched_req.csi_meas_cfg = config_helpers::make_default_csi_meas_config(params);
  }

  return sched_req;
}

inline cell_config_dedicated create_test_initial_ue_spcell_cell_config(const cell_config_builder_params& params = {})
{
  cell_config_dedicated cfg;
  cfg.serv_cell_idx = to_serv_cell_index(0);
  cfg.serv_cell_cfg = config_helpers::create_default_initial_ue_serving_cell_config(params);
  return cfg;
}

inline sched_ue_creation_request_message
create_default_sched_ue_creation_request(const cell_config_builder_params& params = {})
{
  sched_ue_creation_request_message msg{};

  msg.ue_index = to_du_ue_index(0);
  msg.crnti    = to_rnti(0x4601);

  scheduling_request_to_addmod sr_0{.sr_id = scheduling_request_id::SR_ID_MIN, .max_tx = sr_max_tx::n64};
  msg.cfg.sched_request_config_list.push_back(sr_0);

  msg.cfg.cells.push_back(create_test_initial_ue_spcell_cell_config(params));

  msg.cfg.lc_config_list.resize(2);
  msg.cfg.lc_config_list[0] = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB0);
  msg.cfg.lc_config_list[1] = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB1);

  return msg;
}

inline rach_indication_message generate_rach_ind_msg(slot_point prach_slot_rx, rnti_t temp_crnti, unsigned rapid = 0)
{
  rach_indication_message msg{};
  msg.cell_index = to_du_cell_index(0);
  msg.slot_rx    = prach_slot_rx;
  msg.occasions.emplace_back();
  msg.occasions.back().frequency_index = 0;
  msg.occasions.back().start_symbol    = 0;
  msg.occasions.back().preambles.emplace_back();
  msg.occasions.back().preambles.back().preamble_id  = rapid;
  msg.occasions.back().preambles.back().tc_rnti      = temp_crnti;
  msg.occasions.back().preambles.back().time_advance = phy_time_unit::from_seconds(0);
  return msg;
}

} // namespace test_helpers
} // namespace srsgnb
