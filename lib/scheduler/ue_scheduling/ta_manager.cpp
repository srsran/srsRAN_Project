/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ta_manager.h"

using namespace srsran;

ta_manager::ta_manager(const scheduler_ue_expert_config& expert_cfg_,
                       subcarrier_spacing                ul_scs_,
                       dl_logical_channel_manager*       dl_lc_ch_mgr_) :
  ul_scs(ul_scs_), dl_lc_ch_mgr(dl_lc_ch_mgr_), expert_cfg(expert_cfg_), state(state_t::idle)
{
  // Pre-allocate size for vector.
  for (auto& tag_meas : tag_n_ta_diff_measurements) {
    tag_meas.resize(expert_cfg.ta_measurement_window);
  }
}

void ta_manager::handle_ul_n_ta_update_indication(uint8_t tag_id, int64_t n_ta_diff_, float ul_sinr)
{
  // [Implementation-defined] N_TA update (N_TA_new - N_TA_old value in T_C units) measurements are considered only if
  // the UL SINR reported in the corresponding indication message is higher than the threshold.
  // NOTE: From the testing with COTS UE its observed that N_TA update measurements with UL SINR less than 10 dB were
  // majorly outliers.
  if (state == state_t::measure and ul_sinr > expert_cfg.ta_update_measurement_ul_sinr_threshold) {
    tag_n_ta_diff_measurements[tag_id].push_back(n_ta_diff_);
  }
}

void ta_manager::slot_indication(slot_point current_sl)
{
  // Update the measurement start time.
  if (state == state_t::idle) {
    meas_start_time = current_sl;
    state           = state_t::measure;
  }

  // Early return if measurement interval is short.
  if (abs(current_sl - meas_start_time) < expert_cfg.ta_measurement_window) {
    return;
  }

  for (uint8_t tag_id = 0; tag_id < tag_n_ta_diff_measurements.size(); ++tag_id) {
    if (tag_n_ta_diff_measurements[tag_id].empty()) {
      continue;
    }

    // Send Timing Advance command only if the offset is greater than threshold.
    // The new Timing Advance Command is a value ranging from [0,...,63] as per TS 38.213, clause 4.2. Hence, we
    // need to subtract a value of 31 (as per equation in the same clause) to get the change in Timing Advance Command.
    const unsigned new_t_a = compute_new_t_a(compute_avg_n_ta_difference(tag_id));
    if (abs((int)new_t_a - 31) > expert_cfg.ta_cmd_offset_threshold) {
      // Send Timing Advance Command to UE.
      dl_lc_ch_mgr->handle_mac_ce_indication(
          {.ce_lcid = lcid_dl_sch_t::TA_CMD, .ce_payload = ta_cmd_ce_payload{.tag_id = tag_id, .ta_cmd = new_t_a}});
    }

    // Reset stored measurements.
    reset_measurements(tag_id);
  }
}

int64_t ta_manager::compute_avg_n_ta_difference(uint8_t tag_id)
{
  int64_t avg_n_ta_difference = 0;
  for (const auto& meas : tag_n_ta_diff_measurements[tag_id]) {
    avg_n_ta_difference += meas;
  }
  return avg_n_ta_difference / static_cast<int64_t>(tag_n_ta_diff_measurements.size());
}

unsigned ta_manager::compute_new_t_a(int64_t n_ta_diff)
{
  return static_cast<int>(std::round(static_cast<float>(n_ta_diff * pow2(to_numerology_value(ul_scs))) /
                                     static_cast<float>(16U * 64))) +
         31;
}

void ta_manager::reset_measurements(uint8_t tag_id)
{
  state = state_t::idle;
  tag_n_ta_diff_measurements[tag_id].clear();
}
