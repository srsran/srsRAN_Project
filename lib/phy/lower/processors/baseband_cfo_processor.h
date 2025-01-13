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
#include "srsran/adt/concurrent_queue.h"
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
  /// \param time   Time at which the new CFO value is used.
  /// \param cfo_Hz New CFO value in Hertz.
  bool schedule_cfo_command(time_point time, float cfo_Hz) override
  {
    cfo_command command{time, cfo_Hz};
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
    const cfo_command& command = *cfo_command_queue.begin();

    // Get the current time.
    auto now = std::chrono::system_clock::now();

    // If the time for the command has come...
    if (now >= command.first) {
      // Update the current normalized CFO.
      current_cfo = command.second / srate.to_Hz<float>();

      // Pop the current command.
      cfo_command_queue.pop();
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
  using cfo_command = std::pair<time_point, float>;
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
};

} // namespace srsran
