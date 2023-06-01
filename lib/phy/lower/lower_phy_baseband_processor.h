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
#include "srsran/adt/blocking_queue.h"
#include "srsran/gateways/baseband/baseband_gateway_receiver.h"
#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/phy/lower/lower_phy_controller.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_baseband.h"
#include "srsran/phy/lower/sampling_rate.h"
#include <condition_variable>
#include <mutex>

namespace srsran {

/// \brief Implements the lower physical layer baseband processing core.
///
/// This class interfaces and manages the baseband data flow between the baseband gateways and the processors. This
/// class is agnostic to the sampling rate and radio frame timing.
class lower_phy_baseband_processor : public lower_phy_controller
{
public:
  /// Collects the parameters necessary to initialize the baseband adaptor, as well as injected dependencies.
  struct configuration {
    /// Sampling rate.
    sampling_rate srate;
    /// \brief Receive task executor.
    ///
    /// Receives baseband samples from the \ref baseband_gateway_receiver, reserves baseband buffers and pushes
    /// tasks to the other executors.
    task_executor* rx_task_executor;
    /// \brief Transmit task executor.
    ///
    /// Transmits baseband samples and releases the downlink baseband processing buffer to the pool.
    task_executor* tx_task_executor;
    /// \brief Uplink task executor.
    ///
    /// Notifies uplink-related time boundaries, runs the baseband demodulation and notifies availability of data.
    task_executor* ul_task_executor;
    /// \brief Downlink task executor.
    ///
    /// Notifies downlink-related time boundaries and runs the baseband modulation.
    task_executor* dl_task_executor;
    /// Baseband receiver gateway.
    baseband_gateway_receiver* receiver;
    /// Baseband transmitter gateway.
    baseband_gateway_transmitter* transmitter;
    /// Uplink baseband processor.
    uplink_processor_baseband* ul_bb_proc;
    /// Downlink processor baseband.
    downlink_processor_baseband* dl_bb_proc;
    /// Number of transmit ports.
    unsigned nof_tx_ports;
    /// Number of receive ports.
    unsigned nof_rx_ports;
    /// Receive to transmit delay in samples.
    baseband_gateway_timestamp tx_time_offset;
    /// Maximum number of samples between the last received sample and the next sample to transmit time instants.
    baseband_gateway_timestamp rx_to_tx_max_delay;
    /// Transmit buffers size.
    unsigned tx_buffer_size;
    /// Number of transmit buffers of size \c nof_tx_buffers.
    unsigned nof_tx_buffers;
    /// Receive buffers size.
    unsigned rx_buffer_size;
    /// Number of receive buffers of size \c rx_buffer_size.
    unsigned nof_rx_buffers;
    /// System time-based throttling. See \ref lower_phy_configuration::system_time_throttling.
    float system_time_throttling;
  };

  /// Constructs a baseband adaptor.
  explicit lower_phy_baseband_processor(const configuration& config);

  // See interface for documentation.
  void start(baseband_gateway_timestamp init_time) override;

  // See interface for documentation.
  void stop() override;

private:
  /// Internal finite state machine to control the internal state.
  class internal_fsm
  {
  public:
    /// Default destructor - It reports a fatal error if the state is \c running or \c wait_stop.
    ~internal_fsm()
    {
      std::unique_lock<std::mutex> lock(state_mutex);
      report_fatal_error_if_not((state != states::running) && (state != states::wait_stop), "Unexpected state.");
    }

    /// Notifies the start of the processing.
    void start()
    {
      std::unique_lock<std::mutex> lock(state_mutex);
      report_fatal_error_if_not(state == states::idle, "Unexpected state.");
      state = states::running;
      state_cvar.notify_all();
    }

    /// \brief Notifies that the asynchronous execution has stopped.
    ///
    /// Changes the state to \e stopped and sends the corresponding notification.
    void notify_stop()
    {
      std::unique_lock<std::mutex> lock(state_mutex);
      report_fatal_error_if_not(state == states::wait_stop, "Unexpected state.");
      state = states::stopped;
      state_cvar.notify_all();
    }

    /// Requests all asynchronous processing to stop.
    void request_stop()
    {
      std::unique_lock<std::mutex> lock(state_mutex);

      // Skip if it is not running.
      if (state != states::running) {
        return;
      }

      state = states::wait_stop;
      state_cvar.notify_all();
    }

    /// \brief Waits for all asynchronous processing to stop.
    /// \remark A fatal error is reported if wait_stop() is called while it is running without calling first
    ///         request_stop().
    void wait_stop()
    {
      std::unique_lock<std::mutex> lock(state_mutex);
      report_fatal_error_if_not(state != states::running, "Unexpected state.");

      while (state == states::wait_stop) {
        state_cvar.wait(lock);
      }
    }

    /// Returns true if the asynchronous execution is running.
    bool is_running() const
    {
      std::unique_lock<std::mutex> lock(state_mutex);
      return state == states::running;
    }

  private:
    /// Possible states.
    enum class states {
      /// The asynchronous processing has not been started.
      idle = 0,
      /// The asynchronous processing has started and not notify_stop.
      running,
      /// A stop command has been issued - waiting for the asynchronous tasks to finish.
      wait_stop,
      /// The asynchronous processing started, ran and it is currently notify_stop.
      stopped
    };

    /// Protects concurrent read and write of the state.
    mutable std::mutex state_mutex;
    /// Notifies state transitions.
    std::condition_variable state_cvar;
    /// Actual state.
    states state = states::idle;
  };

  /// \brief Processes downlink baseband.
  /// \param[in] timestamp Current processing time.
  void dl_process(baseband_gateway_timestamp timestamp);

  /// Processes uplink baseband.
  void ul_process();

  sampling_rate                                                         srate;
  unsigned                                                              tx_buffer_size;
  unsigned                                                              rx_buffer_size;
  std::chrono::nanoseconds                                              cpu_throttling_time;
  task_executor&                                                        rx_executor;
  task_executor&                                                        tx_executor;
  task_executor&                                                        uplink_executor;
  task_executor&                                                        downlink_executor;
  baseband_gateway_receiver&                                            receiver;
  baseband_gateway_transmitter&                                         transmitter;
  uplink_processor_baseband&                                            uplink_processor;
  downlink_processor_baseband&                                          downlink_processor;
  blocking_queue<std::unique_ptr<baseband_gateway_buffer_dynamic>>      rx_buffers;
  blocking_queue<std::unique_ptr<baseband_gateway_buffer_dynamic>>      tx_buffers;
  baseband_gateway_timestamp                                            tx_time_offset;
  baseband_gateway_timestamp                                            rx_to_tx_max_delay;
  internal_fsm                                                          tx_state;
  internal_fsm                                                          rx_state;
  baseband_gateway_timestamp                                            last_rx_timestamp;
  std::mutex                                                            last_rx_mutex;
  std::condition_variable                                               last_rx_cvar;
  optional<std::chrono::time_point<std::chrono::high_resolution_clock>> last_tx_time;
};

} // namespace srsran