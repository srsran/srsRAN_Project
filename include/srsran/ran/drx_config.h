/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  bool operator==(const drx_config& other) const
  {
    return long_cycle == other.long_cycle && long_start_offset == other.long_start_offset &&
           on_duration_timer == other.on_duration_timer && inactivity_timer == other.inactivity_timer;
  }
  bool operator!=(const drx_config& other) const { return not(*this == other); }
};

namespace drx_helper {

/// Valid LongCycle values as per TS 38.331, "drx-LongCycleStartOffset".
span<const std::chrono::milliseconds> valid_long_cycle_values();

/// Valid onDurationTimer values as per TS 38.331, "drx-LongCycleStartOffset".
span<const std::chrono::milliseconds> valid_on_duration_timer_values();

/// Valid InactivityTimer values as per TS 38.331, "drx-InactivityTyimer".
span<const std::chrono::milliseconds> valid_inactivity_timer_values();

} // namespace drx_helper

} // namespace srsran
