/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__
#include <uhd/types/metadata.hpp>
#include <uhd/types/time_spec.hpp>
#pragma GCC diagnostic pop

namespace srsran {

class radio_uhd_tx_stream_fsm
{
private:
  /// Wait for end-of-burst acknowledgement timeout in seconds.
  static constexpr double WAIT_EOB_ACK_TIMEOUT_S = 0.01;

  /// Defines the Tx stream internal states.
  enum class states {
    /// Indicates the stream was not initialized successfully.
    UNINITIALIZED = 0,
    /// Indicates the stream is ready to start burst.
    START_BURST,
    /// Indicates the stream is transmitting a burst.
    IN_BURST,
    /// Indicates an end-of-burst must be transmitted and abort any transmission.
    END_OF_BURST,
    /// Indicates wait for end-of-burst acknowledgement.
    WAIT_END_OF_BURST,
    /// Signals a stop to the asynchronous thread.
    WAIT_STOP,
    /// Indicates the asynchronous thread is notify_stop.
    STOPPED
  };

  /// Indicates the current state.
  states state;

  /// Protects the class concurrent access.
  mutable std::mutex mutex;
  /// Condition variable to wait for certain states.
  std::condition_variable cvar;

  uhd::time_spec_t wait_eob_timeout = uhd::time_spec_t();

public:
  /// \brief Notifies that the transmit stream has been initialized successfully.
  void init_successful()
  {
    std::unique_lock<std::mutex> lock(mutex);
    state = states::START_BURST;
  }

  /// \brief Notifies a late or an underflow event.
  /// \remark Transitions state end of burst if it is in a burst.
  /// \param[in] time_spec Indicates the time the underflow event occurred.
  void async_event_late_underflow(const uhd::time_spec_t& time_spec)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (state == states::IN_BURST) {
      state            = states::END_OF_BURST;
      wait_eob_timeout = time_spec;
      wait_eob_timeout += WAIT_EOB_ACK_TIMEOUT_S;
    }
  }

  /// \brief Notifies an end-of-burst acknowledgement.
  /// \remark Transitions state to start burst if it is waiting for the end-of-burst.
  void async_event_end_of_burst_ack()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (state == states::WAIT_END_OF_BURST) {
      state = states::START_BURST;
    }
  }

  /// \brief Handles a new transmission.
  /// \param[out] metadata     Destination of the required metadata.
  /// \param[in]  time_spec    Transmission time of the first sample.
  /// \param[in]  is_empty     Empty buffer flag.
  /// \param[in]  tail_padding Tail padding flag, indicating the last transmission in the burst.
  /// \return True if the block shall be transmitted. False if the block shall be ignored.
  bool on_transmit(uhd::tx_metadata_t& metadata, uhd::time_spec_t& time_spec, bool is_empty, bool tail_padding)
  {
    std::unique_lock<std::mutex> lock(mutex);
    switch (state) {
      case states::WAIT_END_OF_BURST:
        // Do nothing if the wait for end-of-burst timeout has not expired.
        if (wait_eob_timeout.get_real_secs() >= time_spec.get_real_secs()) {
          return false;
        }
        // Otherwise go into start burst state and handle the state.
        state = states::START_BURST;
      case states::START_BURST:
        // Set start of burst flag and time spec.
        if (!is_empty) {
          metadata.has_time_spec  = true;
          metadata.start_of_burst = true;
          metadata.end_of_burst   = tail_padding;
          metadata.time_spec      = time_spec;

          // Transition to in-burst.
          if (!tail_padding) {
            state = states::IN_BURST;
          }

          return true;
        }
        return false;
      case states::IN_BURST:
        if (is_empty || tail_padding) {
          // Signal end of burst to UHD.
          metadata.end_of_burst = true;

          // Transition to start burst without waiting for the EOB ACK.
          state = states::START_BURST;
        }
        break;
      case states::END_OF_BURST:
        // Flag end-of-burst.
        metadata.end_of_burst = true;
        state                 = states::WAIT_END_OF_BURST;
        if (wait_eob_timeout == uhd::time_spec_t()) {
          wait_eob_timeout = time_spec;
          wait_eob_timeout += WAIT_EOB_ACK_TIMEOUT_S;
        }
        break;
      case states::UNINITIALIZED:
      case states::WAIT_STOP:
      case states::STOPPED:
        // Ignore transmission.
        return false;
    }

    // Transmission shall not be ignored.
    return true;
  }

  void stop(uhd::tx_metadata_t& metadata)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (state == states::IN_BURST) {
      metadata.end_of_burst = true;
    }
    state = states::WAIT_STOP;
  }

  bool is_stopping() const
  {
    std::unique_lock<std::mutex> lock(mutex);
    return state == states::WAIT_STOP;
  }

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
};

} // namespace srsran
