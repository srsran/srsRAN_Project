/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ul_power_controller.h"
#include "../config/ue_configuration.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

ul_power_controller::ul_power_controller(const ue_cell_configuration&    ue_cell_cfg,
                                         const ue_channel_state_manager& ch_state_manager) :
  rnti(ue_cell_cfg.crnti),
  cl_pw_control_enabled(ue_cell_cfg.cell_cfg_common.expert_cfg.ue.ul_power_ctrl.enable_pusch_cl_pw_control),
  p0_nominal_pusch(
      ue_cell_cfg.cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().p0_nominal_with_grant.to_int()),
  channel_state_manager(ch_state_manager),
  pusch_sinr_target_dB(ue_cell_cfg.cell_cfg_common.expert_cfg.ue.ul_power_ctrl.target_pusch_sinr),
  ref_path_loss_for_target_sinr(
      ue_cell_cfg.cell_cfg_common.expert_cfg.ue.ul_power_ctrl.path_loss_for_target_pusch_sinr),
  tpc_adjust_prohibit_time_sl([&ue_cell_cfg]() -> unsigned {
    return tpc_adjust_prohibit_time_ms << to_numerology_value(
               ue_cell_cfg.cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params.scs);
  }()),
  logger(srslog::fetch_basic_logger("SCHED"))
{
  // Save the PUSCH power control configuration.
  reconfigure(ue_cell_cfg);

  // Initialize PUSCH PRB grid.
  static constexpr unsigned init_nof_prbs     = 0U;
  static constexpr int      init_f_pw_control = 0;
  std::fill(pusch_pw_ctrl_grid.begin(),
            pusch_pw_ctrl_grid.end(),
            pusch_pw_ctrl_data{slot_point{}, init_nof_prbs, init_f_pw_control});

  // Dummy casts only needed to prevent Clang from complaining about unused variables.
  static_cast<void>(rnti);
  static_cast<void>(cl_pw_control_enabled);
  static_cast<void>(channel_state_manager);
  static_cast<void>(ref_path_loss_for_target_sinr);
  static_cast<void>(pusch_sinr_target_dB);
}

void ul_power_controller::reconfigure(const ue_cell_configuration& ue_cell_cfg)
{
  if (ue_cell_cfg.cfg_dedicated().ul_config.has_value() and
      ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.has_value() and
      ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().pusch_pwr_ctrl.has_value()) {
    pusch_pwr_ctrl.emplace(
        ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().pusch_pwr_ctrl.value());
  }
}

SRSRAN_WEAK_SYMB void ul_power_controller::handle_phr(const cell_ph_report& phr, slot_point slot_rx)
{
  if (not pusch_pwr_ctrl.has_value()) {
    return;
  }
  latest_phr.emplace(ue_phr_report{phr, std::nullopt});
}

void ul_power_controller::update_pusch_pw_ctrl_state(slot_point slot_rx, unsigned nof_prbs)
{
  const int latest_f_cl_pw_control =
      latest_pusch_pw_control.has_value() ? latest_pusch_pw_control.value().f_cl_pw_control : 0;
  const unsigned grid_idx      = slot_rx.to_uint();
  pusch_pw_ctrl_grid[grid_idx] = {slot_rx, nof_prbs, latest_f_cl_pw_control};
}

SRSRAN_WEAK_SYMB unsigned ul_power_controller::adapt_pusch_prbs_to_phr(unsigned nof_prbs) const
{
  // Dummy function. This feature is only available in the SRSRAN 5G Enterprise version.
  return nof_prbs;
}

SRSRAN_WEAK_SYMB uint8_t ul_power_controller::compute_tpc_command(slot_point pusch_slot)
{
  // Dummy function. This feature is only available in the SRSRAN 5G Enterprise version.
  static constexpr uint8_t default_tpc = 1;
  return default_tpc;
}
