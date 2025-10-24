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

#include "srsran/adt/blocking_queue.h"
#include "srsran/gateways/baseband/baseband_gateway_receiver.h"
#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/phy/lower/lower_phy_controller.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_baseband.h"
#include "srsran/phy/lower/sampling_rate.h"
#include <future>

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
    /// Subcarrier spacing.
    subcarrier_spacing scs;
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
    /// Receive buffers size.
    unsigned rx_buffer_size;
    /// Number of receive buffers of size \c rx_buffer_size.
    unsigned nof_rx_buffers;
    /// System time-based throttling. See \ref lower_phy_configuration::system_time_throttling.
    float system_time_throttling;
    /// Number of slots to execute before a complete stop after requesting to stop.
    unsigned stop_nof_slots;
  };

  /// Constructs a baseband adaptor.
  explicit lower_phy_baseband_processor(const configuration& config);

  // See interface for documentation.
  void start(baseband_gateway_timestamp init_time, baseband_gateway_timestamp sfn0_ref_time) override;

  // See interface for documentation.
  void stop() override;

private:
  /// Internal finite state machine to control the internal state.
  class internal_fsm
  {
  public:
    /// Initialize the internal FSM with the number of processing slots required to close the lower PHY.
    internal_fsm(unsigned stop_count) : state_stopped(state_wait_stop + stop_count) {}

    /// Default destructor - It reports a fatal error if the state is \c running or \c wait_stop.
    ~internal_fsm()
    {
      uint32_t current_state = state.load();
      report_fatal_error_if_not((current_state == state_idle) || (current_state >= state_stopped), "Unexpected state.");
    }

    /// \brief Notifies the start of the processing.
    /// \remark A fatal error is reported if start() is called while the processor is not in \c idle state.
    void start()
    {
      uint32_t expected_state = state_idle;
      bool     success        = state.compare_exchange_strong(expected_state, state_running);
      report_fatal_error_if_not(
          success, "The starting expected state is 0x{:08x} (idle) but found 0x{:08x}.", state_idle, expected_state);
    }

    /// \brief Requests all asynchronous processing to stop.
    /// \remark A fatal error is reported if request_stop() is called more than once.
    void request_stop()
    {
      uint32_t previous_state = state.fetch_xor(state_wait_stop);
      report_fatal_error_if_not((previous_state & state_wait_stop) == 0, "Stopping has been requested more than once.");
    }

    /// \brief Waits for all asynchronous processing to stop.
    /// \remark A fatal error is reported if wait_stop() is called while it is \c idle or \c running without calling
    /// first request_stop().
    void wait_stop()
    {
      report_fatal_error_if_not((state.load() & state_wait_stop) != 0, "Unexpected state.");

      // Wait for the state to transition to stop.
      stop_control.get_future().wait();
    }

    /// \brief Call on the event of processing.
    /// \return \c true if the state is running, otherwise \c false.
    bool on_process()
    {
      // Detect stop mask.
      if ((state.load() & state_wait_stop) != 0) {
        // Increment the process count before considering stopped.
        uint32_t current_state = state.fetch_add(1) + 1;
        if (current_state >= state_stopped) {
          stop_control.set_value();
          return false;
        }
      }
      return true;
    }

  private:
    /// State value in idle.
    static constexpr uint32_t state_idle = 0x7fffffff;
    /// State value while running.
    static constexpr uint32_t state_running = 0x00000000;
    /// State mask while the lower PHY is stopping.
    static constexpr uint32_t state_wait_stop = 0x80000000;
    /// Stopped state, depends on the maximum processing delay number of slots.
    const uint32_t state_stopped;

    /// Actual state.
    std::atomic<uint32_t> state{state_idle};
    /// Promise for controlling the stop sequence.
    std::promise<void> stop_control;
  };

  /// \brief Processes downlink baseband.
  /// \param[in] timestamp Current processing time.
  void dl_process(baseband_gateway_timestamp timestamp);

  /// Processes uplink baseband.
  void ul_process();

  /// \brief Subtracts the System Frame Number (SFN) Zero reference time to a given timestamp.
  ///
  /// To avoid an overflow in the substraction, a number of samples is added to the timestamp that results in the same
  /// SFN and slot.
  baseband_gateway_timestamp apply_timestamp_sfn0_ref(baseband_gateway_timestamp timestamp) const
  {
    // Add the time of a superframe which is 1024 frames to avoid overflow.
    if (timestamp < start_time_sfn0) {
      timestamp += divide_ceil(start_time_sfn0, nof_samples_per_super_frame) * nof_samples_per_super_frame;
    }

    return timestamp - start_time_sfn0;
  }

  sampling_rate                                                              srate;
  uint64_t                                                                   nof_samples_per_super_frame;
  unsigned                                                                   rx_buffer_size;
  std::chrono::microseconds                                                  slot_duration;
  float                                                                      system_time_throttling_ratio;
  task_executor&                                                             rx_executor;
  task_executor&                                                             tx_executor;
  task_executor&                                                             uplink_executor;
  baseband_gateway_receiver&                                                 receiver;
  baseband_gateway_transmitter&                                              transmitter;
  uplink_processor_baseband&                                                 uplink_processor;
  downlink_processor_baseband&                                               downlink_processor;
  blocking_queue<std::unique_ptr<baseband_gateway_buffer_dynamic>>           rx_buffers;
  baseband_gateway_timestamp                                                 tx_time_offset;
  baseband_gateway_timestamp                                                 rx_to_tx_max_delay;
  baseband_gateway_timestamp                                                 start_time_sfn0;
  internal_fsm                                                               tx_state;
  internal_fsm                                                               rx_state;
  std::atomic<baseband_gateway_timestamp>                                    last_rx_timestamp;
  std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> last_tx_time;
  unsigned                                                                   last_tx_buffer_size = 0;
};

} // namespace srsran
