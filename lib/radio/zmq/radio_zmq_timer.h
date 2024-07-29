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

/// Timer for the ZeroMQ-based radio channel.
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
