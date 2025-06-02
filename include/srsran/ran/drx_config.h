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

#include "srsran/adt/span.h"
#include <chrono>

namespace srsran {

/// Configuration of a UE Discontinuous Reception (DRX) mode.
struct drx_config {
  /// Period of the DRX cycle. See "drx-LongCycleStartOffset" in TS 38.331. Values: {10, 20, 32, 40, 60, 64, 70, 80,
  /// 128, 160, 256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240}.
  std::chrono::milliseconds long_cycle;
  /// Start Offset of the DRX long cycle. See "drx-LongCycleStartOffset" in TS 38.331. Values {0,...,long_cycle - 1}.
  std::chrono::milliseconds long_start_offset;
  /// Duration at the beginning of a DRX cycle. Values: {1, 2, 3, 4, 5, 6, 8, 10, 20, 30, 40, 50, 60, 80, 100, 200, 300,
  /// 400, 500, 600, 800, 1000, 1200, 1600}.
  std::chrono::milliseconds on_duration_timer;
  /// Duration of the UE inactivity timer. Values: {0, 1, 2, 3, 4, 5, 6, 8, 10, 20, 30, 40, 50, 60, 80, 100, 200, 300,
  /// 500, 750, 1280, 1920, 2560}.
  std::chrono::milliseconds inactivity_timer;
  /// Duration of the Retransmission DL timer, in slots. See "drx-RetransmissionTimerDL" in TS 38.331. Values: {0, 1, 2,
  /// 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320}.
  unsigned retx_timer_dl;
  /// Duration of the Retransmission UL timer, in slots. See "drx-RetransmissionTimerUL" in TS 38.331. Values: {0, 1, 2,
  /// 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320}.
  unsigned retx_timer_ul;

  bool operator==(const drx_config& other) const
  {
    return long_cycle == other.long_cycle && long_start_offset == other.long_start_offset &&
           on_duration_timer == other.on_duration_timer && inactivity_timer == other.inactivity_timer &&
           retx_timer_dl == other.retx_timer_dl && retx_timer_ul == other.retx_timer_ul;
  }
  bool operator!=(const drx_config& other) const { return not(*this == other); }
};

namespace drx_helper {

/// Valid LongCycle values as per TS 38.331, "drx-LongCycleStartOffset".
span<const std::chrono::milliseconds> valid_long_cycle_values();

/// Valid onDurationTimer values as per TS 38.331, "drx-LongCycleStartOffset".
span<const std::chrono::milliseconds> valid_on_duration_timer_values();

/// Valid InactivityTimer values as per TS 38.331, "drx-InactivityTimer".
span<const std::chrono::milliseconds> valid_inactivity_timer_values();

/// Valid RetransmissionTimer values as per TS 38.331, "drx-RetransmissionTimerDL" and "drx-RetransmissionTimerUL".
span<const unsigned> valid_retx_timer_values();

} // namespace drx_helper

} // namespace srsran
