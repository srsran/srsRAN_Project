/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "lib/du_manager/converters/scheduler_configuration_helpers.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/scheduler/config/csi_helper.h"
#include "srsran/scheduler/config/logical_channel_config_factory.h"
#include "srsran/scheduler/config/sched_cell_config_helpers.h"
#include "srsran/scheduler/config/serving_cell_config.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include "srsran/scheduler/mac_scheduler.h"

namespace srsran {
namespace test_helpers {

inline sched_cell_configuration_request_message
make_default_sched_cell_configuration_request(const cell_config_builder_params& params = {})
{
  sched_cell_configuration_request_message sched_req{};
  sched_req.cell_index     = to_du_cell_index(0);
  sched_req.pci            = params.pci;
  sched_req.scs_common     = params.scs_common;
  sched_req.dl_carrier     = config_helpers::make_default_dl_carrier_configuration(params);
  sched_req.ul_carrier     = config_helpers::make_default_ul_carrier_configuration(params);
  sched_req.dl_cfg_common  = config_helpers::make_default_dl_config_common(params);
  sched_req.ul_cfg_common  = config_helpers::make_default_ul_config_common(params);
  sched_req.ssb_config     = config_helpers::make_default_ssb_config(params);
  sched_req.dmrs_typeA_pos = dmrs_typeA_position::pos2;
  if (not band_helper::is_paired_spectrum(sched_req.dl_carrier.band)) {
    sched_req.tdd_ul_dl_cfg_common = config_helpers::make_default_tdd_ul_dl_config_common(params);
  }

  sched_req.nof_beams = 1;

  // SIB1 parameters.
  sched_req.coreset0          = params.coreset0_index;
  sched_req.searchspace0      = 0U;
  sched_req.sib1_payload_size = 101; // Random size.

  pucch_builder_params default_pucch_builder_params     = du_cell_config{}.pucch_cfg;
  default_pucch_builder_params.nof_ue_pucch_f1_res_harq = 3;
  default_pucch_builder_params.nof_ue_pucch_f2_res_harq = 6;
  default_pucch_builder_params.nof_sr_resources         = 2;

  sched_req.pucch_guardbands = config_helpers::build_pucch_guardbands_list(
      default_pucch_builder_params, sched_req.ul_cfg_common.init_ul_bwp.generic_params.crbs.length());

  if (params.csi_rs_enabled) {
    csi_helper::csi_builder_params csi_params{};
    csi_params.pci           = params.pci;
    csi_params.nof_rbs       = sched_req.dl_cfg_common.init_dl_bwp.generic_params.crbs.length();
    csi_params.nof_ports     = params.nof_dl_ports;
    csi_params.csi_rs_period = csi_helper::get_max_csi_rs_period(params.scs_common);

    sched_req.zp_csi_rs_list      = csi_helper::make_periodic_zp_csi_rs_resource_list(csi_params);
    sched_req.nzp_csi_rs_res_list = csi_helper::make_nzp_csi_rs_resource_list(csi_params);
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
create_default_sched_ue_creation_request(const cell_config_builder_params&    params      = {},
                                         const std::initializer_list<lcid_t>& lcid_to_cfg = {})
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
  for (lcid_t lcid : lcid_to_cfg) {
    if (lcid >= lcid_t::LCID_SRB2) {
      msg.cfg.lc_config_list.push_back(config_helpers::create_default_logical_channel_config(lcid));
    }
  }

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

inline uplink_config make_test_ue_uplink_config(const cell_config_builder_params& params)

{
  // > UL Config.
  uplink_config ul_config{};
  ul_config.init_ul_bwp.pucch_cfg.emplace();

  // >> PUCCH.
  auto& pucch_cfg = ul_config.init_ul_bwp.pucch_cfg.value();
  // PUCCH Resource Set ID 0.
  auto& pucch_res_set_0            = pucch_cfg.pucch_res_set.emplace_back();
  pucch_res_set_0.pucch_res_set_id = 0;
  pucch_res_set_0.pucch_res_id_list.emplace_back(0);
  pucch_res_set_0.pucch_res_id_list.emplace_back(1);
  pucch_res_set_0.pucch_res_id_list.emplace_back(2);

  auto& pucch_res_set_1            = pucch_cfg.pucch_res_set.emplace_back();
  pucch_res_set_1.pucch_res_set_id = 1;
  pucch_res_set_1.pucch_res_id_list.emplace_back(3);
  pucch_res_set_1.pucch_res_id_list.emplace_back(4);
  pucch_res_set_1.pucch_res_id_list.emplace_back(5);
  pucch_res_set_1.pucch_res_id_list.emplace_back(6);
  pucch_res_set_1.pucch_res_id_list.emplace_back(7);
  pucch_res_set_1.pucch_res_id_list.emplace_back(8);

  unsigned nof_rbs = band_helper::get_n_rbs_from_bw(
      params.channel_bw_mhz,
      params.scs_common,
      params.band.has_value() ? band_helper::get_freq_range(params.band.value()) : frequency_range::FR1);

  // PUCCH resource format 1, for HARQ-ACK.
  // >>> PUCCH resource 0.
  pucch_resource res_basic{.res_id = 0, .starting_prb = nof_rbs - 1, .format = pucch_format::FORMAT_1};

  res_basic.format_params.emplace<pucch_format_1_cfg>(
      pucch_format_1_cfg{.initial_cyclic_shift = 0, .nof_symbols = 14, .starting_sym_idx = 0, .time_domain_occ = 0});
  pucch_cfg.pucch_res_list.push_back(res_basic);
  // >>> PUCCH resource 1.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res1 = pucch_cfg.pucch_res_list.back();
  res1.res_id          = 1;
  res1.starting_prb    = 1;
  res1.second_hop_prb.emplace(nof_rbs - res1.starting_prb - 1);
  // >>> PUCCH resource 2.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res2 = pucch_cfg.pucch_res_list.back();
  res2.res_id          = 2;
  res2.second_hop_prb.emplace(1);
  res2.starting_prb = nof_rbs - res2.second_hop_prb.value() - 1;

  // PUCCH resource format 2, for HARQ-ACK and CSI.
  // >>> PUCCH resource 3.
  pucch_resource res_basic_f2{.starting_prb = 2, .format = pucch_format::FORMAT_2};
  res_basic_f2.res_id = 3;
  res_basic_f2.format_params.emplace<pucch_format_2_3_cfg>(
      pucch_format_2_3_cfg{.nof_prbs = 1, .nof_symbols = 2, .starting_sym_idx = 0});
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  // >>> PUCCH resource 4.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res4                                                   = pucch_cfg.pucch_res_list.back();
  res4.res_id                                                            = 4;
  variant_get<pucch_format_2_3_cfg>(res4.format_params).starting_sym_idx = 2;
  // >>> PUCCH resource 5.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res5                                                   = pucch_cfg.pucch_res_list.back();
  res5.res_id                                                            = 5;
  variant_get<pucch_format_2_3_cfg>(res5.format_params).starting_sym_idx = 4;
  // >>> PUCCH resource 6.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res6                                                   = pucch_cfg.pucch_res_list.back();
  res6.res_id                                                            = 6;
  variant_get<pucch_format_2_3_cfg>(res6.format_params).starting_sym_idx = 6;
  // >>> PUCCH resource 7.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res7                                                   = pucch_cfg.pucch_res_list.back();
  res7.res_id                                                            = 7;
  variant_get<pucch_format_2_3_cfg>(res7.format_params).starting_sym_idx = 8;
  // >>> PUCCH resource 8.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res8                                                   = pucch_cfg.pucch_res_list.back();
  res8.res_id                                                            = 8;
  variant_get<pucch_format_2_3_cfg>(res8.format_params).starting_sym_idx = 10;
  // >>> PUCCH resource 9.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res9                                                   = pucch_cfg.pucch_res_list.back();
  res9.res_id                                                            = 9;
  variant_get<pucch_format_2_3_cfg>(res9.format_params).starting_sym_idx = 12;

  // >>> PUCCH resource 10.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res10 = pucch_cfg.pucch_res_list.back();
  res10.res_id          = 10;
  res10.starting_prb    = 0;

  // >>> PUCCH resource 11.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res11 = pucch_cfg.pucch_res_list.back();
  res11.res_id          = 11;
  res11.starting_prb    = nof_rbs - 3;

  // TODO: add more PUCCH resources.

  // >>> SR Resource.
  const unsigned pucch_sr_res_id = pucch_cfg.pucch_res_list.size() - 1;
  pucch_cfg.sr_res_list.push_back(scheduling_request_resource_config{.sr_res_id    = 1,
                                                                     .sr_id        = uint_to_sched_req_id(0),
                                                                     .period       = sr_periodicity::sl_40,
                                                                     .offset       = 0,
                                                                     .pucch_res_id = pucch_sr_res_id});

  pucch_cfg.format_1_common_param.emplace();
  pucch_cfg.format_2_common_param.emplace(
      pucch_common_all_formats{.max_c_rate = max_pucch_code_rate::dot_25, .simultaneous_harq_ack_csi = true});

  // >>> dl-DataToUl-Ack
  // TS38.213, 9.1.2.1 - "If a UE is provided dl-DataToUL-ACK, the UE does not expect to be indicated by DCI format 1_0
  // a slot timing value for transmission of HARQ-ACK information that does not belong to the intersection of the set
  // of slot timing values {1, 2, 3, 4, 5, 6, 7, 8} and the set of slot timing values provided by dl-DataToUL-ACK for
  // the active DL BWP of a corresponding serving cell.
  // Inactive for format1_0."
  // Note2: Only k1 >= 4 supported.
  nr_band band = params.band.has_value() ? params.band.value() : band_helper::get_band_from_dl_arfcn(params.dl_arfcn);
  if (band_helper::get_duplex_mode(band) == duplex_mode::FDD) {
    pucch_cfg.dl_data_to_ul_ack = {SCHEDULER_MIN_K1};
  } else {
    // TDD
    pucch_cfg.dl_data_to_ul_ack =
        config_helpers::generate_k1_candidates(config_helpers::make_default_tdd_ul_dl_config_common(params));
  }

  // > PUSCH config.
  ul_config.init_ul_bwp.pusch_cfg.emplace(config_helpers::make_default_pusch_config());

  // > SRS config.
  ul_config.init_ul_bwp.srs_cfg.emplace(config_helpers::make_default_srs_config(params));

  return ul_config;
}

} // namespace test_helpers
} // namespace srsran
