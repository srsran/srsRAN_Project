/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "ta_manager.h"
#include <numeric>

using namespace srsran;

ta_manager::ta_manager(const scheduler_ue_expert_config& expert_cfg_,
                       subcarrier_spacing                ul_scs_,
                       dl_logical_channel_manager*       dl_lc_ch_mgr_) :
  ul_scs(ul_scs_), dl_lc_ch_mgr(dl_lc_ch_mgr_), expert_cfg(expert_cfg_), state(state_t::idle)
{
  if (expert_cfg.ta_cmd_offset_threshold < 0) {
    state = state_t::disabled;
  }
}

void ta_manager::handle_ul_n_ta_update_indication(uint8_t tag_id, int64_t n_ta_diff_, float ul_sinr)
{
  // [Implementation-defined] N_TA update (N_TA_new - N_TA_old value in T_C units) measurements are considered only if
  // the UL SINR reported in the corresponding indication message is higher than the threshold.
  // NOTE: From the testing with COTS UE its observed that N_TA update measurements with UL SINR less than 10 dB were
  // majorly outliers.
  if (state == state_t::measure and ul_sinr > expert_cfg.ta_update_measurement_ul_sinr_threshold) {
    tag_n_ta_diff_measurements[tag_id].emplace_back(n_ta_diff_);
  }
}

void ta_manager::slot_indication(slot_point current_sl)
{
  if (state == state_t::disabled) {
    return;
  }

  // Update the measurement start time.
  // NOTE: When the state is idle, it denotes the start of measurement window. And when the measurement time reaches the
  // threshold only then I change the state back to idle (until then the state will be in measure)
  if (state == state_t::idle) {
    meas_start_time = current_sl;
    state           = state_t::measure;
  }

  // Early return if measurement interval is short.
  if ((current_sl - meas_start_time) < (int)expert_cfg.ta_measurement_slot_period) {
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

  // Set TA manager state to idle.
  state = state_t::idle;
}

int64_t ta_manager::compute_avg_n_ta_difference(uint8_t tag_id)
{
  // Adjust this threshold as needed.
  static const double num_std_deviations = 2.0;

  const auto n_ta_diff_meas = tag_n_ta_diff_measurements[tag_id];

  // Compute mean.
  const double sum  = std::accumulate(n_ta_diff_meas.begin(), n_ta_diff_meas.end(), 0.0);
  const double mean = sum / static_cast<double>(n_ta_diff_meas.size());

  // Compute standard deviation.
  std::vector<double> diff(n_ta_diff_meas.size());
  std::transform(
      n_ta_diff_meas.begin(), n_ta_diff_meas.end(), diff.begin(), [mean](double value) { return value - mean; });
  const double sq_sum  = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
  const double std_dev = std::sqrt(sq_sum / static_cast<double>(n_ta_diff_meas.size()));

  int64_t avg_n_ta_difference = 0;
  for (const auto& meas : n_ta_diff_meas) {
    // Filter out outliers.
    if (std::abs((double)meas - mean) <= num_std_deviations * std_dev) {
      avg_n_ta_difference += meas;
    }
  }

  return avg_n_ta_difference / static_cast<int64_t>(n_ta_diff_meas.size());
}

unsigned ta_manager::compute_new_t_a(int64_t n_ta_diff)
{
  return static_cast<int>(std::round(static_cast<float>(n_ta_diff * pow2(to_numerology_value(ul_scs))) /
                                     static_cast<float>(16U * 64))) +
         31;
}

void ta_manager::reset_measurements(uint8_t tag_id)
{
  tag_n_ta_diff_measurements[tag_id].clear();
}
