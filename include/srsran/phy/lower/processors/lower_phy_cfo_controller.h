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

/// Lower physical layer carrier frequency processor interface.
class lower_phy_cfo_controller
{
public:
  /// Data type used for point in time.
  using time_point = std::chrono::system_clock::time_point;

  /// Default destructor.
  virtual ~lower_phy_cfo_controller() = default;

  /// \brief Schedules a CFO command.
  /// \param time   Time at which the CFO value is applied.
  /// \param cfo_Hz New CFO value in Hertz.
  virtual bool schedule_cfo_command(time_point time, float cfo_Hz) = 0;
};

} // namespace srsran
