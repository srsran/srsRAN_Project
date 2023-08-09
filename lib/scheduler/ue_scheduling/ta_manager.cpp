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

ta_manager::ta_manager(subcarrier_spacing ul_scs_) : ul_scs(ul_scs_), state(state_t::idle) {}

void ta_manager::handle_ul_n_ta_update_indication(slot_point rx_slot_, int64_t n_ta_diff_)
{
  if (state == state_t::measure and not measurements.full()) {
    measurements.push_back({.rx_slot = rx_slot_, .n_ta_diff = n_ta_diff_});
  }
}

void ta_manager::slot_indication(slot_point current_sl)
{
  // Update the measurement start time.
  if (state == state_t::idle) {
    meas_start_time = current_sl;
    state           = state_t::measure;
  }

  // Early return if there are not enough measurements/empty or measurement interval is short.
  if (measurements.empty() or (abs(current_sl - meas_start_time) < MEAS_TIME_INTERVAL_IN_SLOTS)) {
    return;
  }

  // Early return if change in N_TA is less than threshold.
  if (compute_avg_n_ta_difference() < N_TA_OFFSET_THRESHOLD) {
    return;
  }

  // TODO: Send Timing Advance command and put state in prohibit.

  reset_measurements();
}

int64_t ta_manager::compute_avg_n_ta_difference()
{
  int64_t avg_n_ta_difference = 0;
  for (const auto& meas : measurements) {
    avg_n_ta_difference += meas.n_ta_diff;
  }
  return avg_n_ta_difference / static_cast<int64_t>(measurements.size());
}

unsigned ta_manager::compute_new_t_a(int64_t n_ta_diff)
{
  return static_cast<int>(std::round(static_cast<float>(n_ta_diff * pow2(to_numerology_value(ul_scs))) /
                                     static_cast<float>(16U * 64))) +
         31;
}

void ta_manager::reset_measurements()
{
  state = state_t::idle;
  measurements.clear();
}
