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

#include "ta_manager.h"
#include <numeric>

using namespace srsran;

ta_manager::ta_manager(const scheduler_ue_expert_config& expert_cfg_,
                       subcarrier_spacing                ul_scs_,
                       time_alignment_group::id_t        pcell_tag_id,
                       dl_logical_channel_manager*       dl_lc_ch_mgr_) :
  ul_scs(ul_scs_),
  dl_lc_ch_mgr(dl_lc_ch_mgr_),
  expert_cfg(expert_cfg_),
  logger(srslog::fetch_basic_logger("SCHED")),
  state(state_t::idle)
{
  if (expert_cfg.ta_cmd_offset_threshold < 0) {
    state = state_t::disabled;
    return;
  }

  update_tags(std::array<time_alignment_group::id_t, 1>{pcell_tag_id});
}

void ta_manager::handle_ul_n_ta_update_indication(time_alignment_group::id_t tag_id, int64_t n_ta_diff_, float ul_sinr)
{
  // [Implementation-defined] N_TA update (N_TA_new - N_TA_old value in T_C units) measurements are considered only if
  // the UL SINR reported in the corresponding indication message is higher than the threshold.
  // NOTE: From the testing with COTS UE its observed that N_TA update measurements with UL SINR less than 10 dB were
  // majorly outliers.
  if (state == state_t::measure and ul_sinr > expert_cfg.ta_update_measurement_ul_sinr_threshold) {
    // Note: Linear search is faster than binary for very small arrays.
    auto it = std::find_if(
        n_ta_reports.begin(), n_ta_reports.end(), [tag_id](const auto& meas) { return meas.tag_id == tag_id; });
    if (it != n_ta_reports.end() and it->tag_id == tag_id) {
      n_ta_reports[tag_id.value()].samples.emplace_back(n_ta_diff_);
    } else {
      logger.warning("Discarding TA report. Cause: TAG Id {} is not configured", tag_id.value());
    }
  }
}

void ta_manager::update_tags(span<const time_alignment_group::id_t> tag_ids)
{
  n_ta_reports.resize(tag_ids.size());
  for (unsigned i = 0, e = n_ta_reports.size(); i != e; ++i) {
    n_ta_reports[i].tag_id = tag_ids[i];
    n_ta_reports[i].samples.clear();
    n_ta_reports[i].samples.reserve(expert_cfg.ta_measurement_slot_period);
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
    return;
  }

  if (state == state_t::prohibit) {
    if ((current_sl - prohibit_start_time) > static_cast<int>(expert_cfg.ta_measurement_slot_prohibit_period)) {
      meas_start_time = current_sl;
      state           = state_t::measure;
    }
    return;
  }

  // Early return if measurement interval is short.
  if ((current_sl - meas_start_time) < static_cast<int>(expert_cfg.ta_measurement_slot_period)) {
    return;
  }

  bool ta_cmd_sent = false;
  for (unsigned tag_idx = 0; tag_idx != n_ta_reports.size(); ++tag_idx) {
    if (n_ta_reports[tag_idx].samples.empty()) {
      continue;
    }
    const time_alignment_group::id_t tag_id = n_ta_reports[tag_idx].tag_id;

    // Send Timing Advance command only if the offset is equal to or greater than the threshold.
    // The new Timing Advance Command is a value ranging from [0,...,63] as per TS 38.213, clause 4.2. Hence, we
    // need to subtract a value of 31 (as per equation in the same clause) to get the change in Timing Advance Command.
    const unsigned new_t_a = compute_new_t_a(compute_avg_n_ta_difference(tag_idx));
    if (abs(static_cast<int>(new_t_a) - ta_cmd_offset_zero) >= expert_cfg.ta_cmd_offset_threshold) {
      // Send Timing Advance Command to UE.
      if (dl_lc_ch_mgr->handle_mac_ce_indication(
              {.ce_lcid    = lcid_dl_sch_t::TA_CMD,
               .ce_payload = ta_cmd_ce_payload{.tag_id = tag_id, .ta_cmd = new_t_a}})) {
        ta_cmd_sent = true;
      } else {
        // Early return if queueing the TA CMD indication failed. Will try again at the next slot indication.
        logger.warning("Dropped TA command, queue is full.");
        return;
      }
    }

    // Reset stored measurements.
    reset_measurements(tag_idx);
  }

  if (ta_cmd_sent and expert_cfg.ta_measurement_slot_prohibit_period > 0) {
    // Set TA manager state to prohibit state.
    state               = state_t::prohibit;
    prohibit_start_time = current_sl;
  } else {
    // Set TA manager state to idle.
    state = state_t::idle;
  }
}

int64_t ta_manager::compute_avg_n_ta_difference(unsigned tag_idx)
{
  // Adjust this threshold as needed.
  static const double num_std_deviations = 1.75;

  span<const int64_t> samples = n_ta_reports[tag_idx].samples;
  if (samples.size() == 1) {
    return samples.front();
  }
  if (samples.size() == 2) {
    return (samples[0] + samples[1]) / 2;
  }

  // Compute mean.
  const double sum  = std::accumulate(samples.begin(), samples.end(), 0.0);
  const double mean = sum / static_cast<double>(samples.size());

  // Compute standard deviation.
  const double sample_variance =
      std::accumulate(samples.begin(),
                      samples.end(),
                      0.0,
                      [mean](double acc, int64_t samp) { return acc + std::pow(samp - mean, 2); }) /
      (samples.size() - 1);
  const double sample_std_dev = std::sqrt(sample_variance);

  int64_t  sum_n_ta_difference = 0;
  unsigned count               = 0;
  for (const int64_t meas : samples) {
    // Filter out outliers.
    if (std::abs(static_cast<double>(meas) - mean) <= num_std_deviations * sample_std_dev) {
      sum_n_ta_difference += meas;
      ++count;
    }
  }

  return sum_n_ta_difference / static_cast<int64_t>(count);
}

unsigned ta_manager::compute_new_t_a(int64_t n_ta_diff)
{
  return static_cast<unsigned>(
      std::round(static_cast<float>(n_ta_diff * pow2(to_numerology_value(ul_scs))) / static_cast<float>(16U * 64) +
                 static_cast<float>(ta_cmd_offset_zero) - expert_cfg.ta_target));
}

void ta_manager::reset_measurements(unsigned tag_idx)
{
  n_ta_reports[tag_idx].samples.clear();
}
