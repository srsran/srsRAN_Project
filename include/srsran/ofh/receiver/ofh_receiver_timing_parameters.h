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

#include <chrono>

namespace srsran {
namespace ofh {

/// \brief Structure storing the reception window timing parameters.
struct du_rx_window_timing_parameters {
  /// Offset from the current OTA symbol to the end of UL User-Plane reception window.
  std::chrono::microseconds Ta4_max;
  /// Offset from the current OTA symbol to the start of UL User-Plane reception window.
  std::chrono::microseconds Ta4_min;
};

} // namespace ofh
} // namespace srsran
