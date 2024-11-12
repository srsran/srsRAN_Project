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
