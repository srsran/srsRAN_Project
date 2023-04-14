/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsran/adt/blocking_queue.h"
#include "srsran/gateways/baseband/baseband_gateway_buffer.h"
#include "srsran/gateways/baseband/baseband_gateway_receiver.h"
#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include "srsran/phy/lower/lower_phy_controller.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_baseband.h"
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
  /// Collects the parameters necessary to initialize the baseband adaptor.
  struct configuration {
    /// Receive task executor - Main the lower PHY processing executor it receives baseband from \ref
    /// baseband_gateway_receiver, reserves baseband buffers and pushes tasks to the other executors.
    task_executor* rx_task_executor;
    /// Transmit task executor - Transmits baseband buffers and releases the downlink baseband processing buffer to the
    /// pool.
    task_executor* tx_task_executor;
    /// Uplink task executor - Notifies uplink-related time boundaries, runs the baseband demodulation and notifies
    /// availability of data.
    task_executor* ul_task_executor;
    /// Downlink task executor - Notifies downlink-related time boundaries and runs the baseband modulation.
    task_executor* dl_task_executor;
    /// Baseband receiver gateway - Receives baseband samples from a radio.
    baseband_gateway_receiver* receiver;
    /// Baseband transmitter gateway - Transmits baseband samples through a radio.
    baseband_gateway_transmitter* transmitter;
    /// Uplink baseband processor - processes uplink baseband samples.
    uplink_processor_baseband* ul_bb_proc;
    /// Downlink processor baseband - processes downlink baseband samples.
    downlink_processor_baseband* dl_bb_proc;
    /// Number of transmit ports.
    unsigned nof_tx_ports;
    /// Number of receive ports.
    unsigned nof_rx_ports;
    /// Receive to transmit delay in samples.
    baseband_gateway_timestamp rx_to_tx_delay;
  };

  /// Constructs a baseband adaptor.
  lower_phy_baseband_processor(const configuration& config);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  /// Internal finite state machine to control the internal state.
  class internal_fsm
  {
  public:
    /// Notifies the start of the processing.
    void start()
    {
      std::unique_lock<std::mutex> lock(state_mutex);
      report_fatal_error_if_not(state == states::idle, "Unexpected state.");
      state = states::running;
      state_cvar.notify_all();
    }

    /// Notifies the stop of the processing.
    void stopped()
    {
      std::unique_lock<std::mutex> lock(state_mutex);
      report_fatal_error_if_not(state == states::wait_stop, "Unexpected state.");
      state = states::stopped;
      state_cvar.notify_all();
    }

    /// Requests all asynchronous processing to stop.
    void stop_and_wait()
    {
      std::unique_lock<std::mutex> lock(state_mutex);

      // Skip if it is not running.
      if (state != states::running) {
        return;
      }

      state = states::wait_stop;
      state_cvar.notify_all();

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
      /// The asynchronous processing has started and not stopped.
      running,
      /// A stop has been commanded and it is waiting for the asynchronous tasks to finish.
      wait_stop,
      /// The asynchronous processing started, ran and it is currently stopped.
      stopped
    };

    /// Protects concurrent read and write of the state.
    mutable std::mutex state_mutex;
    /// Notifies state transitions.
    std::condition_variable state_cvar;
    /// Actual state.
    states state = states::idle;
  };

  /// Processes baseband.
  void process();

  /// Number of receive buffers.
  static constexpr unsigned default_nof_rx_buffers = 128;
  /// Number of transmit buffers.
  static constexpr unsigned default_nof_tx_buffers = 128;

  task_executor&                                           rx_executor;
  task_executor&                                           tx_executor;
  task_executor&                                           uplink_executor;
  task_executor&                                           downlink_executor;
  baseband_gateway_receiver&                               receiver;
  baseband_gateway_transmitter&                            transmitter;
  uplink_processor_baseband&                               uplink_processor;
  downlink_processor_baseband&                             downlink_processor;
  blocking_queue<std::unique_ptr<baseband_gateway_buffer>> rx_buffers;
  blocking_queue<std::unique_ptr<baseband_gateway_buffer>> tx_buffers;
  baseband_gateway_timestamp                               rx_to_tx_delay;
  internal_fsm                                             state;
};

} // namespace srsran