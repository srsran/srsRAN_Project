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

#include "srsran/ran/slot_point.h"
#include <chrono>

namespace srsran {

/// Describes the context of the current timing boundary.
struct lower_phy_timing_context {
  /// Slot context.
  slot_point slot;
  /// Time point of the current context.
  std::chrono::time_point<std::chrono::system_clock> time_point;
};

} // namespace srsran
