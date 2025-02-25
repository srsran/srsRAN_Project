/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "scheduler_config_helper.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/scheduler/config/logical_channel_config_factory.h"
#include "srsran/scheduler/config/sched_cell_config_helpers.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"

using namespace srsran;

sched_cell_configuration_request_message srsran::sched_config_helper::make_default_sched_cell_configuration_request(
    const cell_config_builder_params& params_input)
{
  config_helpers::cell_config_builder_params_extended params{params_input};
  sched_cell_configuration_request_message            sched_req{};
  sched_req.cell_index           = to_du_cell_index(0);
  sched_req.pci                  = params.pci;
  sched_req.scs_common           = params.scs_common;
  sched_req.dl_carrier           = config_helpers::make_default_dl_carrier_configuration(params);
  sched_req.ul_carrier           = config_helpers::make_default_ul_carrier_configuration(params);
  sched_req.dl_cfg_common        = config_helpers::make_default_dl_config_common(params);
  sched_req.ul_cfg_common        = config_helpers::make_default_ul_config_common(params);
  sched_req.ssb_config           = config_helpers::make_default_ssb_config(params);
  sched_req.tdd_ul_dl_cfg_common = params.tdd_ul_dl_cfg_common;

  // The CORESET duration of 3 symbols is only permitted if dmrs-typeA-Position is set to 3. Refer TS 38.211, 7.3.2.2.
  const pdcch_type0_css_coreset_description coreset0_desc = pdcch_type0_css_coreset_get(
      *params.band, params.ssb_scs, params.scs_common, *params.coreset0_index, params.k_ssb->value());
  sched_req.dmrs_typeA_pos =
      coreset0_desc.nof_symb_coreset == 3U ? dmrs_typeA_position::pos3 : dmrs_typeA_position::pos2;

  sched_req.nof_beams = 1;

  // SIB1 parameters.
  sched_req.coreset0          = *params.coreset0_index;
  sched_req.searchspace0      = params.search_space0_index;
  sched_req.sib1_payload_size = units::bytes{101}; // Random size.

  pucch_builder_params default_pucch_builder_params;
  default_pucch_builder_params.nof_ue_pucch_f0_or_f1_res_harq       = 3;
  default_pucch_builder_params.nof_ue_pucch_f2_or_f3_or_f4_res_harq = 6;
  default_pucch_builder_params.nof_sr_resources                     = 2;
  default_pucch_builder_params.f0_or_f1_params.emplace<pucch_f1_params>();
  default_pucch_builder_params.f2_or_f3_or_f4_params.emplace<pucch_f2_params>();

  sched_req.pucch_guardbands = config_helpers::build_pucch_guardbands_list(
      default_pucch_builder_params, sched_req.ul_cfg_common.init_ul_bwp.generic_params.crbs.length());

  if (params.csi_rs_enabled) {
    csi_meas_config csi_meas      = config_helpers::make_csi_meas_config(params);
    pdsch_config    pdsch         = config_helpers::make_default_pdsch_config(params);
    sched_req.zp_csi_rs_list      = pdsch.zp_csi_rs_res_list;
    sched_req.nzp_csi_rs_res_list = csi_meas.nzp_csi_rs_res_list;
  }

  if (sched_req.tdd_ul_dl_cfg_common.has_value()) {
    sched_req.dl_data_to_ul_ack =
        config_helpers::generate_k1_candidates(*sched_req.tdd_ul_dl_cfg_common, params.min_k1);
  } else {
    sched_req.dl_data_to_ul_ack = {params.min_k1};
  }

  return sched_req;
}

cell_config_dedicated
sched_config_helper::create_test_initial_ue_spcell_cell_config(const cell_config_builder_params& params)
{
  cell_config_dedicated cfg;
  cfg.serv_cell_idx = to_serv_cell_index(0);
  cfg.serv_cell_cfg = config_helpers::create_default_initial_ue_serving_cell_config(params);
  return cfg;
}

sched_ue_creation_request_message
sched_config_helper::create_default_sched_ue_creation_request(const cell_config_builder_params& params,
                                                              span<const lcid_t>                lcid_to_cfg)
{
  sched_ue_creation_request_message msg{};

  msg.ue_index = to_du_ue_index(0);
  msg.crnti    = to_rnti(0x4601);

  scheduling_request_to_addmod sr_0{.sr_id = scheduling_request_id::SR_ID_MIN, .max_tx = sr_max_tx::n64};
  msg.cfg.sched_request_config_list.emplace();
  msg.cfg.sched_request_config_list->push_back(sr_0);

  msg.cfg.cells.emplace();
  msg.cfg.cells->push_back(create_test_initial_ue_spcell_cell_config(params));

  msg.cfg.lc_config_list.emplace();
  msg.cfg.lc_config_list->resize(2);
  (*msg.cfg.lc_config_list)[0] = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB0);
  (*msg.cfg.lc_config_list)[1] = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB1);
  for (lcid_t lcid : lcid_to_cfg) {
    if (lcid >= lcid_t::LCID_SRB2) {
      msg.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(lcid));
    }
  }

  return msg;
}

sched_ue_creation_request_message
sched_config_helper::create_default_sched_ue_creation_request(const cell_config_builder_params&    params,
                                                              const std::initializer_list<lcid_t>& lcid_to_cfg)
{
  return create_default_sched_ue_creation_request(params, span<const lcid_t>(lcid_to_cfg.begin(), lcid_to_cfg.end()));
}

sched_ue_creation_request_message
sched_config_helper::create_empty_spcell_cfg_sched_ue_creation_request(const cell_config_builder_params& params)
{
  sched_ue_creation_request_message msg{};

  msg.ue_index = to_du_ue_index(0);
  msg.crnti    = to_rnti(0x4601);

  cell_config_dedicated cfg;
  cfg.serv_cell_idx              = to_serv_cell_index(0);
  serving_cell_config& serv_cell = cfg.serv_cell_cfg;

  serv_cell.cell_index = to_du_cell_index(0);

  msg.cfg.cells.emplace();
  msg.cfg.cells->push_back(cfg);

  msg.cfg.lc_config_list.emplace();
  msg.cfg.lc_config_list->resize(1);
  (*msg.cfg.lc_config_list)[0] = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB0);

  return msg;
}
