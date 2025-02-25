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

#include "pucch_power_controller.h"
#include "../config/ue_configuration.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

pucch_power_controller::pucch_power_controller(const ue_cell_configuration& ue_cell_cfg) :
  rnti(ue_cell_cfg.crnti),
  cl_pw_control_enabled(ue_cell_cfg.cell_cfg_common.expert_cfg.ue.ul_power_ctrl.enable_pucch_cl_pw_control),
  pucch_f0_sinr_target_dB(ue_cell_cfg.cell_cfg_common.expert_cfg.ue.ul_power_ctrl.pucch_f0_sinr_target_dB),
  pucch_f2_sinr_target_dB(ue_cell_cfg.cell_cfg_common.expert_cfg.ue.ul_power_ctrl.pucch_f2_sinr_target_dB),
  pucch_f3_sinr_target_dB(ue_cell_cfg.cell_cfg_common.expert_cfg.ue.ul_power_ctrl.pucch_f3_sinr_target_dB),
  tpc_adjust_prohibit_time_sl([&ue_cell_cfg]() -> unsigned {
    return tpc_adjust_prohibit_time_ms << to_numerology_value(
               ue_cell_cfg.cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params.scs);
  }()),
  pucch_f0_f1_sinr_dB(alpha_ema_sinr),
  pucch_f2_f3_f4_sinr_dB(alpha_ema_sinr),
  logger(srslog::fetch_basic_logger("SCHED"))
{
  // Save the PUCCH power control configuration.
  reconfigure(ue_cell_cfg);

  // Initialize PUCCH PRB grid.
  std::fill(pucch_pw_ctrl_grid.begin(), pucch_pw_ctrl_grid.end(), pucch_grants_pw_ctrl{});

  // Dummy casts only needed to prevent Clang from complaining about unused variables.
  static_cast<void>(rnti);
  static_cast<void>(cl_pw_control_enabled);
  static_cast<void>(pucch_f0_sinr_target_dB);
  static_cast<void>(pucch_f2_sinr_target_dB);
  static_cast<void>(pucch_f3_sinr_target_dB);
}

void pucch_power_controller::reconfigure(const ue_cell_configuration& ue_cell_cfg)
{
  if (ue_cell_cfg.init_bwp().ul_ded.has_value() and ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.has_value()) {
    const auto& pucch_cfg = ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value();
    // Retrieve the Format of the resources in PUCCH set 0 and 1. NOTE: all resources are expected to be of the same
    // format.
    static constexpr size_t id_pucch_res_set_0 = 0U;
    unsigned pucch_res_set_0_id = pucch_cfg.pucch_res_set[id_pucch_res_set_0].pucch_res_id_list.front().ue_res_id;
    auto*    res_set_0          = std::find_if(
        pucch_cfg.pucch_res_list.begin(),
        pucch_cfg.pucch_res_list.end(),
        [pucch_res_set_0_id](const pucch_resource& res) { return res.res_id.ue_res_id == pucch_res_set_0_id; });
    srsran_assert(res_set_0 != pucch_cfg.pucch_res_list.end(),
                  "rnti={}: PUCCH resource id={} not found in PUCCH resource set 0",
                  rnti,
                  pucch_res_set_0_id);
    format_set_0                               = res_set_0->format;
    static constexpr size_t id_pucch_res_set_1 = 1U;
    unsigned pucch_res_set_1_id = pucch_cfg.pucch_res_set[id_pucch_res_set_1].pucch_res_id_list.front().ue_res_id;
    auto*    res_set_1          = std::find_if(
        pucch_cfg.pucch_res_list.begin(),
        pucch_cfg.pucch_res_list.end(),
        [pucch_res_set_1_id](const pucch_resource& res) { return res.res_id.ue_res_id == pucch_res_set_1_id; });
    srsran_assert(res_set_1 != pucch_cfg.pucch_res_list.end(),
                  "rnti={}: PUCCH resource id={} not found in PUCCH resource set 1",
                  rnti,
                  pucch_res_set_1_id);
    format_set_1 = res_set_1->format;
    if (ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().pucch_pw_control.has_value()) {
      pucch_pwr_ctrl.emplace(ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().pucch_pw_control.value());
    }
  }
}

#ifndef SRSRAN_HAS_ENTERPRISE

void pucch_power_controller::update_pucch_pw_ctrl_state(slot_point      slot,
                                                        pucch_format    format,
                                                        unsigned        nof_prbs,
                                                        unsigned        nof_symb,
                                                        pucch_uci_bits& uci_bits,
                                                        bool            intraslot_freq_hopping,
                                                        bool            pi_2_bpsk,
                                                        bool            additional_dmrs)
{
  // Dummy function. This feature is only available in the SRSRAN 5G Enterprise version.
}

void pucch_power_controller::update_pucch_sinr_f0_f1(slot_point slor_rx, float sinr_db)
{
  // Dummy function. This feature is only available in the SRSRAN 5G Enterprise version.
}

void pucch_power_controller::update_pucch_sinr_f2_f3_f4(slot_point slor_rx,
                                                        float      sinr_db,
                                                        bool       has_harq_bits,
                                                        bool       has_csi_bits)
{
  // Dummy function. This feature is only available in the SRSRAN 5G Enterprise version.
}

uint8_t pucch_power_controller::compute_tpc_command(slot_point pucch_slot)
{
  // Dummy function. This feature is only available in the SRSRAN 5G Enterprise version.
  static constexpr uint8_t default_tpc = 1;
  return default_tpc;
}

#endif // SRSRAN_HAS_ENTERPRISE
