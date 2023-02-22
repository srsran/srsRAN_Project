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

#include <condition_variable>
#include <mutex>

namespace srsran {

/// Describes lower physical layer state FSM.
class lower_phy_state_fsm
{
private:
  /// Describes the possible states.
  enum class states {
    /// Indicates the lower physical layer has not been initialized.
    UNINITIALIZED,
    /// Indicates the lower physical layer has been successfully initialized and started the execution of asynchronous
    /// tasks.
    RUNNING,
    /// Indicates stop has been signaled and it is waiting for the execution of asynchronous tasks to finish.
    WAIT_STOP,
    /// Indicates the component was initialized successfully and no asynchronous task is performed.
    STOPPED
  };

  /// Indicates the current state.
  states state = states::UNINITIALIZED;
  /// Protects the concurrent access to the instance.
  mutable std::mutex mutex;
  /// Used to wait for certain state transitions.
  std::condition_variable cvar;

  /// Same than is_running().
  bool is_running_unprotected() const { return state == states::RUNNING; }

public:
  /// Notifies the successful component initialization.
  void on_successful_init()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (state == states::UNINITIALIZED) {
      state = states::RUNNING;
    }
  }

  /// Notifies the asynchronous task executor stopped.
  void on_async_executor_stop()
  {
    std::unique_lock<std::mutex> lock(mutex);
    state = states::STOPPED;
    cvar.notify_all();
  }

  /// Signals a stop and join.
  void stop_and_join()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (is_running_unprotected()) {
      state = states::WAIT_STOP;
    }

    while (state != states::UNINITIALIZED && state != states::STOPPED) {
      cvar.wait(lock);
    }
  }

  bool is_running() const
  {
    std::unique_lock<std::mutex> lock(mutex);
    return is_running_unprotected();
  }
};

} // namespace srsran
