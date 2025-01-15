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

#include <chrono>
#include <cmath>

namespace srsran {
namespace ofh {

/// \brief Structure storing the reception window timing parameters expressed in a number of symbols.
struct rx_window_timing_parameters {
  /// Offset from the current OTA symbol to the first symbol at which UL User-Plane message can be received within its
  /// reception window. Must be calculated based on \c Ta4_min parameter.
  unsigned sym_start;
  /// Offset from the current OTA symbol to the last symbol at which UL User-Plane message can be received within its
  /// reception window. Must be calculated based on \c Ta4_max parameter.
  unsigned sym_end;
};

} // namespace ofh
} // namespace srsran
