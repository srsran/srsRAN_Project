/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/support/units.h"

namespace srsran {

/// \brief Maximum number of SI messages that can be scheduled as per TS 38.331, "maxSI-Message".
const size_t MAX_SI_MESSAGES = 32;

/// Scheduling parameters of the SI message.
struct si_message_scheduling_config {
  /// SI message payload size in bytes.
  units::bytes msg_len;
  /// Periodicity of the SI-message in radio frames. Values: {8, 16, 32, 64, 128, 256, 512}.
  unsigned period_radio_frames;
};

/// \brief Configuration of the SI message scheduling.
///
/// This struct will be handled by the MAC scheduler to determine the required PDCCH and PDSCH grants for SI.
struct si_scheduling_config {
  /// List of SI-messages to schedule.
  std::vector<si_message_scheduling_config> si_messages;
  /// \brief The length of the SI scheduling window, in slots.
  ///
  /// It is always shorter or equal to the period of the SI message. Values: {5, 10, 20, 40, 80, 160, 320, 640, 1280}.
  unsigned si_window_len_slots;
};

} // namespace srsran
