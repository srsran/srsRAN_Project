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

#include <chrono>

namespace srsran {

/// Lower physical layer carrier frequency offset controller interface.
class lower_phy_cfo_controller
{
public:
  /// Data type used for point in time.
  using time_point = std::chrono::system_clock::time_point;

  /// Default destructor.
  virtual ~lower_phy_cfo_controller() = default;

  /// \brief Schedules a CFO command.
  /// \param time Time at which the CFO value is applied.
  /// \param cfo_Hz New CFO value in Hertz.
  /// \param cfo_drift_Hz_s New CFO drift value in Hertz per second.
  virtual bool schedule_cfo_command(time_point time, float cfo_Hz, float cfo_drift_Hz_s = 0) = 0;
};

} // namespace srsran
