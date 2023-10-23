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
