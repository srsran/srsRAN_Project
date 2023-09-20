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

#include "radio_zmq_timer.h"
#include "srsran/support/srsran_assert.h"
#include <condition_variable>
#include <mutex>

namespace srsran {
class radio_zmq_tx_channel_fsm
{
private:
  /// Describes the possible states.
  enum class states {
    /// Indicates it failed to initialize.
    UNINITIALIZED = 0,
    /// The channel has been successfully initialised and it is waiting to start.
    WAIT_START,
    /// The channel waits for a request.
    WAIT_REQUEST,
    /// The channel waits for data to transmit.
    WAIT_DATA,
    /// Waits for the asynchronous thread to notify a stop.
    WAIT_STOP,
    /// Indicates it is notify_stop.
    STOPPED
  };

  /// Indicates the current state.
  states state = states::UNINITIALIZED;
  /// Protects the state and transitions.
  mutable std::mutex mutex;
  /// Condition variable to wait for certain states.
  std::condition_variable cvar;
  /// State waiting timer.
  radio_zmq_timer timer;

  /// Same than is_running().
  bool is_running_unprotected() const { return state == states::WAIT_REQUEST || state == states::WAIT_DATA; }

public:
  /// Notifies that the transmit stream has been initialized successfully.
  void init_successful()
  {
    std::unique_lock<std::mutex> lock(mutex);
    srsran_assert(state == states::UNINITIALIZED, "Invalid state.");
    state = states::WAIT_START;
  }

  /// Notifies the asynchronous processing has started.
  void on_start()
  {
    std::unique_lock<std::mutex> lock(mutex);
    srsran_assert(state == states::WAIT_START, "Invalid state.");
    state = states::WAIT_REQUEST;
    timer.start();
  }

  /// Notifies that the stream detected an error it cannot recover from.
  void on_error()
  {
    std::unique_lock<std::mutex> lock(mutex);
    state = states::STOPPED;
    cvar.notify_all();
  }

  /// Notifies that a request was received.
  void request_received()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (state == states::WAIT_REQUEST) {
      state = states::WAIT_DATA;
      timer.start();
    }
  }

  /// Notifies a block of data was send.
  void data_sent()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (state == states::WAIT_DATA) {
      state = states::WAIT_REQUEST;
      timer.start();
    }
  }

  /// Notifies a stop was signaled and wait for asynchronous executor to stop.
  void stop()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (is_running_unprotected()) {
      state = states::WAIT_STOP;
    }
  }

  /// Waits for the asynchronous tasks notifies that has notify_stop.
  void wait_stop()
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (state != states::STOPPED) {
      cvar.wait(lock);
    }
  }

  /// Notifies the asynchronous task has notify_stop.
  void async_task_stopped()
  {
    std::unique_lock<std::mutex> lock(mutex);
    state = states::STOPPED;
    cvar.notify_all();
  }

  /// Gets if the channel has been successfully initialised.
  bool is_initiated() const
  {
    std::unique_lock<std::mutex> lock(mutex);
    return state > states::UNINITIALIZED;
  }

  /// Indicates whether the current state is operational.
  bool is_running() const
  {
    std::unique_lock<std::mutex> lock(mutex);
    return is_running_unprotected();
  }

  /// Indicates whether it has a pending request.
  bool has_pending_request() const
  {
    std::unique_lock<std::mutex> lock(mutex);
    return state == states::WAIT_DATA;
  }

  /// \brief Checks if the waiting time for request or data has expired.
  ///
  /// The request and data wait expires only if:
  /// - the current state is waiting for a request; or
  /// - the current state is waiting for data.
  ///
  /// The expiration occurs when a certain time had past since:
  /// - the last state transition from waiting data to waiting for request; or
  /// - last time an expiration was detected.
  bool has_wait_expired()
  {
    std::unique_lock<std::mutex> lock(mutex);

    // It can only be expired if it is currently running.
    if (!is_running_unprotected()) {
      return false;
    }

    // Check if the timer has not expired.
    if (!timer.is_expired()) {
      return false;
    }

    // Update timer with new time.
    timer.start();
    return true;
  }
};
} // namespace srsran
