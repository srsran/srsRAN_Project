/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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
#include <condition_variable>
#include <mutex>

namespace srsran {

/// Finite state machine for the ZeroMQ-based radio receive channel.
class radio_zmq_rx_channel_fsm
{
  /// Describes the possible states.
  enum class states {
    /// Indicates it failed to initialize.
    UNINITIALIZED = 0,
    /// Indicates it has been successfully initialized and it is waiting for request.
    SEND_REQUEST,
    /// Indicates it has been successfully initialized and it is waiting for data to transmit.
    RECEIVE_DATA,
    /// Indicates it is notify_stop.
    STOPPED
  };

  /// Indicates the current state.
  states state = states::UNINITIALIZED;
  /// Protects the state and transitions.
  mutable std::mutex mutex;
  /// State waiting timer.
  radio_zmq_timer timer;

  /// Same as is_running().
  bool is_running_unprotected() const { return state == states::SEND_REQUEST || state == states::RECEIVE_DATA; }

public:
  /// Notifies that the receive stream has been initialized successfully.
  void init_successful()
  {
    std::scoped_lock lock(mutex);
    state = states::SEND_REQUEST;
    timer.start();
  }

  /// Notifies that the stream detected an error it cannot recover from.
  void on_error()
  {
    std::scoped_lock lock(mutex);
    state = states::STOPPED;
  }

  /// Notifies that a request was received.
  void request_sent()
  {
    std::scoped_lock lock(mutex);
    if (state == states::SEND_REQUEST) {
      state = states::RECEIVE_DATA;
      timer.start();
    }
  }

  /// Notifies that a block of data was sent.
  void data_received()
  {
    std::scoped_lock lock(mutex);
    if (state == states::RECEIVE_DATA) {
      state = states::SEND_REQUEST;
      timer.start();
    }
  }

  /// Notifies a stop was signaled and wait for asynchronous executor to stop.
  void stop()
  {
    std::scoped_lock lock(mutex);
    if (is_running_unprotected()) {
      state = states::STOPPED;
    }
  }

  /// Notifies the asynchronous task has notify_stop.
  void async_task_stopped()
  {
    std::scoped_lock lock(mutex);
    state = states::STOPPED;
  }

  /// Indicates whether the current state is operational.
  bool is_running() const
  {
    std::scoped_lock lock(mutex);
    return is_running_unprotected();
  }

  /// Indicates whether it has a pending request.
  bool has_pending_response() const
  {
    std::scoped_lock lock(mutex);
    return state == states::RECEIVE_DATA;
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
    std::scoped_lock lock(mutex);

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
