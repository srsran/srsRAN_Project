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

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/ring_buffer.h"
#include "srsran/adt/span.h"
#include "srsran/phy/lower/processors/lower_phy_cfo_controller.h"
#include "srsran/phy/lower/sampling_rate.h"
#include "srsran/srsvec/prod.h"
#include "srsran/support/math/math_utils.h"
#include <chrono>

namespace srsran {

/// \brief Baseband carrier frequency offset processor.
///
/// Applies the configured carrier frequency offset to baseband signals.
class baseband_cfo_processor : public lower_phy_cfo_controller
{
public:
  baseband_cfo_processor(sampling_rate srate_) : srate(srate_) {}

  /// \brief Notifies a new CFO command.
  /// \param time Time at which the new CFO value is used.
  /// \param cfo_Hz New CFO value in Hertz.
  /// \param cfo_drift_Hz_s New CFO drift value in Hertz per second.
  bool schedule_cfo_command(time_point time_, float cfo_Hz_, float cfo_drift_Hz_s_ = 0) override
  {
    cfo_command command{time_, cfo_Hz_, cfo_drift_Hz_s_};
    if (!cfo_command_queue.try_push(command)) {
      return false;
    }
    return true;
  }

  /// Reset sample offset and update the CFO if any command is queued.
  void next_cfo_command()
  {
    // Reset the sample offset.
    sample_offset = 0;

    // Skip if there are no commands.
    if (cfo_command_queue.empty()) {
      return;
    }

    // Get the reference of the next command.
    const cfo_command& command             = *cfo_command_queue.begin();
    auto [cfo_start_ts, cfo_Hz, cfo_drift] = command;

    // Get the current time.
    auto now = std::chrono::system_clock::now();

    // If the time for the command has come...
    if (now >= cfo_start_ts) {
      // Update the current normalized CFO.
      initial_cfo         = cfo_Hz / srate.to_Hz<float>();
      current_cfo         = initial_cfo;
      cfo_start_timestamp = cfo_start_ts;
      cfo_drift_hz_s      = cfo_drift;

      // Pop the current command.
      cfo_command_queue.pop();
    }

    if (std::isnormal(cfo_drift_hz_s)) {
      auto elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - cfo_start_timestamp).count();
      current_cfo          = initial_cfo + (cfo_drift_hz_s * elapsed_time_ms / 1e3) / srate.to_Hz<float>();
    }
  }

  /// Increments the CFO sample offset by a number of samples.
  void advance(unsigned nof_samples) { sample_offset += nof_samples; }

  /// Applies carrier frequency offset in-place to a baseband buffer.
  void process(span<cf_t> buffer) const
  {
    // Skip CFO process if the current CFO is zero, NaN or infinity.
    if (!std::isnormal(current_cfo)) {
      return;
    }

    // Calculate the initial phase of the block in radians.
    float initial_phase = TWOPI * current_cfo * static_cast<float>(sample_offset);

    // Apply CFO.
    srsvec::prod_cexp(buffer, buffer, current_cfo, initial_phase);
  }

private:
  /// CFO command data type.
  using cfo_command = std::tuple<time_point, float, float>;
  /// Maximum number of commands that can be enqueued.
  static constexpr unsigned max_nof_commands = 128;

  /// Baseband sampling rate
  sampling_rate srate;
  /// Queue of CFO commands.
  static_ring_buffer<cfo_command, max_nof_commands> cfo_command_queue;
  /// Current sample count for keeping the phase coherent between calls.
  unsigned sample_offset = 0;
  /// Current normalized CFO.
  float current_cfo = 0.0;
  /// Normalized CFO at the start time.
  float initial_cfo = 0.0;
  /// Current CFO start timestamp.
  time_point cfo_start_timestamp;
  /// Current CFO drift.
  float cfo_drift_hz_s = 0.0;
};

} // namespace srsran
