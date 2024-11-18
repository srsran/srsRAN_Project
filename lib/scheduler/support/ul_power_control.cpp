/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ul_power_control.h"
#include "../config/ue_configuration.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

ul_power_control::ul_power_control(const ue_cell_configuration&    ue_cell_cfg_,
                                   const ue_channel_state_manager& ch_state_manager) :
  ue_cell_cfg(ue_cell_cfg_),
  channel_state_manager(ch_state_manager),
  pusch_sinr_target_dB(ue_cell_cfg_.cell_cfg_common.expert_cfg.ue.target_pusch_sinr),
  ref_path_loss_for_target_sinr(ue_cell_cfg_.cell_cfg_common.expert_cfg.ue.path_loss_for_target_pusch_sinr),
  tpc_adjust_prohibit_time_sl([this]() -> unsigned {
    return tpc_adjust_prohibit_time_ms << to_numerology_value(
               ue_cell_cfg.cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params.scs);
  }()),
  logger(srslog::fetch_basic_logger("SCHED"))
{
  // Initialize PUSCH PRB grid.
  std::fill(pusch_pw_ctrl_grid.begin(), pusch_pw_ctrl_grid.end(), pusch_pw_ctrl_data{slot_point{}, 0});

  // Dummy casts only needed to prevent Clang from complaining about unused variables.
  static_cast<void>(channel_state_manager);
  static_cast<void>(ref_path_loss_for_target_sinr);
  static_cast<void>(pusch_sinr_target_dB);
}

SRSRAN_WEAK_SYMB void ul_power_control::handle_phr(const cell_ph_report& phr)
{
  latest_phr.emplace(ue_phr_report{phr, std::nullopt});
}

void ul_power_control::save_pusch_nof_prbs(slot_point slot_rx, unsigned nof_prbs)
{
  pusch_pw_ctrl_grid[slot_rx.to_uint()] = {slot_rx, nof_prbs};
}

SRSRAN_WEAK_SYMB unsigned ul_power_control::adapt_pusch_prbs_to_phr(unsigned nof_prbs) const
{
  // Dummy function. This feature is only available in the SRSRAN 5G Enterprise version.
  return nof_prbs;
}
