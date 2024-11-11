/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
  /// Period of the DRX cycle. See "drx-LongCycleStartOffset" in TS 38.331.
  std::chrono::milliseconds long_cycle;
  /// Start Offset of the DRX long cycle. See "drx-LongCycleStartOffset" in TS 38.331. Values {0,...,long_cycle - 1}.
  std::chrono::milliseconds long_start_offset;
  /// Duration at the beginning of a DRX cycle.
  std::chrono::milliseconds on_duration_timer;
  /// Duration of the UE inactivity timer.
  std::chrono::milliseconds inactivity_timer;
};

namespace drx_helper {

/// Valid drx-LongCycle values as per TS 38.331, "drx-LongCycleStartOffset".
span<const std::chrono::milliseconds> valid_long_cycle_values();

/// Valid drx-onDurationTimer values as per TS 38.331, "drx-LongCycleStartOffset".
span<const std::chrono::milliseconds> valid_on_duration_timer_values();

} // namespace drx_helper

} // namespace srsran
