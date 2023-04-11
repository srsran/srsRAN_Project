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
#include "srsran/gateways/baseband/baseband_gateway.h"
#include "srsran/gateways/baseband/baseband_gateway_buffer.h"
#include "srsran/gateways/baseband/baseband_gateway_receiver.h"
#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include "srsran/phy/lower/amplitude_controller/amplitude_controller.h"
#include "srsran/phy/lower/lower_phy_controller.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_baseband.h"
#include <condition_variable>
#include <mutex>

namespace srsran {

/// Adapts the lower physical layer baseband gateway to the internal processors baseband interfaces.
class processor_baseband_adaptor : public lower_phy_controller
{
public:
  /// Collects the parameters necessary to initialize the baseband adaptor.
  struct configuration {
    task_executor*                rx_task_executor;
    task_executor*                tx_task_executor;
    task_executor*                ul_task_executor;
    task_executor*                dl_task_executor;
    baseband_gateway_receiver*    receiver;
    baseband_gateway_transmitter* transmitter;
    uplink_processor_baseband*    ul_bb_proc;
    downlink_processor_baseband*  dl_bb_proc;
    unsigned                      nof_tx_ports;
    unsigned                      nof_rx_ports;
    baseband_gateway_timestamp    rx_to_tx_delay;
  };

  /// Constructs a baseband adaptor.
  processor_baseband_adaptor(const configuration& config);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void request_stop() override;

  void wait_stop() override;

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

    /// Stops and waits for the asynchronous execution finishes.
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

    /// Stops and waits for the asynchronous execution finishes.
    void wait_stop()
    {
      std::unique_lock<std::mutex> lock(state_mutex);
      report_fatal_error_if_not(state != states::running, "Unexpected state.");

      // Stop is successful if the state is idle or stopped.
      while ((state != states::idle) && (state != states::stopped)) {
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
      /// The asynchronous processing has bot been started.
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