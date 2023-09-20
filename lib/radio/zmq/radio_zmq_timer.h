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

class radio_zmq_timer
{
public:
  /// \brief Creates a timer.
  /// \param[in] start_now Set to true for starting immediately.
  radio_zmq_timer(bool start_now = false)
  {
    if (start_now) {
      start();
    }
  }

  /// Starts or restarts the expiration time from the current time.
  void start() { wait_expire_time = std::chrono::steady_clock::now() + wait_timeout; }

  /// Clears the current expiration time.
  void clear() { wait_expire_time = no_time; }

  /// Checks if the time has expired.
  bool is_expired() const
  {
    // No expiration time configured.
    if (wait_expire_time == std::chrono::time_point<std::chrono::steady_clock>{}) {
      return false;
    }

    // Get current time.
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

    // Check if the timer has been expired.
    if (now < wait_expire_time) {
      return false;
    }

    return true;
  }

private:
  /// No time value.
  const std::chrono::time_point<std::chrono::steady_clock> no_time{};

  /// Timeout value.
  const std::chrono::milliseconds wait_timeout{1000};

  /// Current expiration time.
  std::chrono::time_point<std::chrono::steady_clock> wait_expire_time = no_time;
};

} // namespace srsran