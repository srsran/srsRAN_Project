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

#include "lib/du_manager/converters/mac_cell_configuration_helpers.h"
#include "srsgnb/mac/mac_configuration_helpers.h"
#include "srsgnb/scheduler/config/logical_channel_config_factory.h"
#include "srsgnb/scheduler/config/serving_cell_config.h"
#include "srsgnb/scheduler/mac_scheduler.h"

namespace srsgnb {

inline sched_cell_configuration_request_message make_default_sched_cell_configuration_request()
{
  du_cell_config default_du_cell_cfg = config_helpers::make_default_du_cell_config();
  /// Random size of a SIB1 payload in bytes.
  const unsigned sib1_payload_size = 101;
  return make_sched_cell_config_req(to_du_cell_index(0), default_du_cell_cfg, sib1_payload_size);
}

// Helper function to create a sched_cell_configuration_request_message that allows a configuration with either 15kHz or
// 30kHz SCS. By default, it creates a bandwidth of 20MHz.
inline sched_cell_configuration_request_message
make_default_sched_cell_configuration_request_scs(subcarrier_spacing scs, bool tdd_mode = false)
{
  sched_cell_configuration_request_message msg{make_default_sched_cell_configuration_request()};
  msg.ssb_config.scs                               = scs;
  msg.scs_common                                   = scs;
  msg.ul_cfg_common.init_ul_bwp.generic_params.scs = scs;
  msg.dl_cfg_common.init_dl_bwp.generic_params.scs = scs;
  // Change Carrier parameters when SCS is 15kHz.
  if (scs == subcarrier_spacing::kHz15) {
    // Band n5 for FDD, band n41 for TDD.
    msg.dl_carrier.arfcn = tdd_mode ? 499200 : 530000;
    msg.ul_carrier.arfcn = tdd_mode ? 499200 : band_helper::get_ul_arfcn_from_dl_arfcn(msg.ul_carrier.arfcn);
    msg.dl_cfg_common.freq_info_dl.scs_carrier_list.front().carrier_bandwidth = 106;
    msg.dl_cfg_common.init_dl_bwp.generic_params.crbs =
        crb_interval{0, msg.dl_cfg_common.freq_info_dl.scs_carrier_list.front().carrier_bandwidth};
    msg.ul_cfg_common.freq_info_ul.scs_carrier_list.front().carrier_bandwidth = 106;
    msg.ul_cfg_common.init_ul_bwp.generic_params.crbs =
        crb_interval{0, msg.ul_cfg_common.freq_info_ul.scs_carrier_list.front().carrier_bandwidth};
  }
  // Change Carrier parameters when SCS is 30kHz.
  else if (scs == subcarrier_spacing::kHz30) {
    // Band n5 for FDD, band n77 or n78 for TDD.
    msg.dl_carrier.arfcn = tdd_mode ? 630000 : 176000;
    msg.ul_carrier.arfcn = tdd_mode ? 630000 : band_helper::get_ul_arfcn_from_dl_arfcn(msg.ul_carrier.arfcn);
    msg.dl_cfg_common.freq_info_dl.scs_carrier_list.emplace_back(
        scs_specific_carrier{0, subcarrier_spacing::kHz30, 52});
    msg.dl_cfg_common.init_dl_bwp.generic_params.crbs = {
        0, msg.dl_cfg_common.freq_info_dl.scs_carrier_list[1].carrier_bandwidth};
    msg.ul_cfg_common.freq_info_ul.scs_carrier_list.emplace_back(
        scs_specific_carrier{0, subcarrier_spacing::kHz30, 52});
    msg.ul_cfg_common.init_ul_bwp.generic_params.crbs = {
        0, msg.ul_cfg_common.freq_info_ul.scs_carrier_list[1].carrier_bandwidth};
  }
  msg.dl_carrier.carrier_bw_mhz = 20;
  msg.dl_carrier.nof_ant        = 1;
  msg.ul_carrier.carrier_bw_mhz = 20;
  msg.ul_carrier.nof_ant        = 1;

  if (tdd_mode) {
    msg.tdd_ul_dl_cfg_common.emplace(config_helpers::make_default_tdd_ul_dl_config_common());
  }

  return msg;
}

inline sched_ue_creation_request_message make_default_sched_ue_creation_request()
{
  sched_ue_creation_request_message msg{};

  msg.ue_index = to_du_ue_index(0);
  msg.crnti    = to_rnti(0x4601);

  scheduling_request_to_addmod sr_0{.sr_id = scheduling_request_id::SR_ID_MIN, .max_tx = sr_max_tx::n64};
  msg.cfg.sched_request_config_list.push_back(sr_0);

  msg.cfg.cells.resize(1);
  msg.cfg.cells[0].cell_index    = to_du_cell_index(0);
  msg.cfg.cells[0].serv_cell_cfg = config_helpers::make_default_initial_ue_serving_cell_config();

  msg.cfg.lc_config_list.resize(2);
  msg.cfg.lc_config_list[0] = config_helpers::make_default_logical_channel_config(lcid_t::LCID_SRB0);
  msg.cfg.lc_config_list[1] = config_helpers::make_default_logical_channel_config(lcid_t::LCID_SRB1);

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

} // namespace srsgnb
