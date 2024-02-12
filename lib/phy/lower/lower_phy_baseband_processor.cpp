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

#include "lower_phy_baseband_processor.h"
#include "srsran/adt/interval.h"
#include "srsran/instrumentation/traces/ru_traces.h"

using namespace srsran;

lower_phy_baseband_processor::lower_phy_baseband_processor(const lower_phy_baseband_processor::configuration& config) :
  srate(config.srate),
  tx_buffer_size(config.tx_buffer_size),
  rx_buffer_size(config.rx_buffer_size),
  cpu_throttling_time((config.tx_buffer_size * static_cast<uint64_t>(config.system_time_throttling * 1e6)) /
                      config.srate.to_kHz()),
  rx_executor(*config.rx_task_executor),
  tx_executor(*config.tx_task_executor),
  uplink_executor(*config.ul_task_executor),
  downlink_executor(*config.dl_task_executor),
  receiver(*config.receiver),
  transmitter(*config.transmitter),
  uplink_processor(*config.ul_bb_proc),
  downlink_processor(*config.dl_bb_proc),
  rx_buffers(config.nof_rx_buffers),
  tx_buffers(config.nof_tx_buffers),
  tx_time_offset(config.tx_time_offset),
  rx_to_tx_max_delay(config.rx_to_tx_max_delay)
{
  static constexpr interval<float> system_time_throttling_range(0, 1);

  srsran_assert(tx_buffer_size, "Invalid buffer size.");
  srsran_assert(rx_buffer_size, "Invalid buffer size.");
  srsran_assert(config.rx_task_executor, "Invalid receive task executor.");
  srsran_assert(system_time_throttling_range.contains(config.system_time_throttling),
                "System time throttling (i.e., {}) is out of the range {}.",
                config.system_time_throttling,
                system_time_throttling_range);
  srsran_assert(config.tx_task_executor, "Invalid transmit task executor.");
  srsran_assert(config.ul_task_executor, "Invalid uplink task executor.");
  srsran_assert(config.dl_task_executor, "Invalid downlink task executor.");
  srsran_assert(config.receiver, "Invalid baseband receiver.");
  srsran_assert(config.transmitter, "Invalid baseband transmitter.");
  srsran_assert(config.ul_bb_proc, "Invalid uplink processor.");
  srsran_assert(config.dl_bb_proc, "Invalid downlink processor.");
  srsran_assert(config.nof_rx_ports != 0, "Invalid number of receive ports.");
  srsran_assert(config.nof_tx_ports != 0, "Invalid number of transmit ports.");

  // Create queue of receive buffers.
  while (!rx_buffers.full()) {
    rx_buffers.push_blocking(std::make_unique<baseband_gateway_buffer_dynamic>(config.nof_rx_ports, rx_buffer_size));
  }

  // Create queue of transmit buffers.
  while (!tx_buffers.full()) {
    tx_buffers.push_blocking(std::make_unique<baseband_gateway_buffer_dynamic>(config.nof_tx_ports, tx_buffer_size));
  }
}

void lower_phy_baseband_processor::start(baseband_gateway_timestamp init_time)
{
  last_rx_timestamp = init_time;

  rx_state.start();
  report_fatal_error_if_not(rx_executor.execute([this]() { ul_process(); }), "Failed to execute initial uplink task.");

  tx_state.start();
  report_fatal_error_if_not(
      downlink_executor.execute([this, init_time]() { dl_process(init_time + rx_to_tx_max_delay); }),
      "Failed to execute initial downlink task.");
}

void lower_phy_baseband_processor::stop()
{
  rx_state.request_stop();
  tx_state.request_stop();
  rx_state.wait_stop();
  tx_state.wait_stop();
}

void lower_phy_baseband_processor::dl_process(baseband_gateway_timestamp timestamp)
{
  // Check if it is running, notify stop and return without enqueueing more tasks.
  if (!tx_state.is_running()) {
    tx_state.notify_stop();
    return;
  }

  // Get transmit baseband buffer. It blocks if all the buffers are enqueued for transmission.
  std::unique_ptr<baseband_gateway_buffer_dynamic> dl_buffer = tx_buffers.pop_blocking();

  // Throttling mechanism to keep a maximum latency of one millisecond in the transmit buffer based on the latest
  // received timestamp.
  {
    // Calculate maximum waiting time to avoid deadlock.
    std::chrono::microseconds timeout_duration = 2 * std::chrono::microseconds(tx_buffer_size * 1000 / srate.to_kHz());
    // Maximum time point to wait for.
    std::chrono::time_point<std::chrono::steady_clock> wait_until_tp =
        std::chrono::steady_clock::now() + timeout_duration;
    // Wait until one of these conditions is met:
    // - The reception timestamp reaches the desired value;
    // - The system time reaches the maximum waiting time; or
    // - The lower PHY was stopped.
    while ((timestamp > (last_rx_timestamp.load(std::memory_order_acquire) + rx_to_tx_max_delay)) &&
           (std::chrono::steady_clock::now() < wait_until_tp) && tx_state.is_running()) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

  // Throttling mechanism to slow down the baseband processing.
  if ((cpu_throttling_time.count() > 0) && (last_tx_time.has_value())) {
    std::chrono::time_point<std::chrono::high_resolution_clock> now     = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds                                    elapsed = now - last_tx_time.value();

    if (elapsed < cpu_throttling_time) {
      std::this_thread::sleep_until(last_tx_time.value() + cpu_throttling_time);
    }
  }
  last_tx_time.emplace(std::chrono::high_resolution_clock::now());

  // Process downlink buffer.
  trace_point                           tp          = ru_tracer.now();
  baseband_gateway_transmitter_metadata baseband_md = downlink_processor.process(dl_buffer->get_writer(), timestamp);
  ru_tracer << trace_event("downlink_baseband", tp);

  // Set transmission timestamp.
  baseband_md.ts = timestamp + tx_time_offset;

  // Enqueue transmission.
  report_fatal_error_if_not(tx_executor.execute([this, tx_buffer = std::move(dl_buffer), baseband_md]() mutable {
    trace_point tx_tp = ru_tracer.now();

    // Transmit buffer.
    transmitter.transmit(tx_buffer->get_reader(), baseband_md);

    // Return transmit buffer to the queue.
    tx_buffers.push_blocking(std::move(tx_buffer));

    ru_tracer << trace_event("transmit_baseband", tx_tp);
  }),
                            "Failed to execute transmit task.");

  // Enqueue DL process task.
  report_fatal_error_if_not(downlink_executor.defer([this, timestamp]() { dl_process(timestamp + tx_buffer_size); }),
                            "Failed to execute downlink processing task");
}

void lower_phy_baseband_processor::ul_process()
{
  // Check if it is running, notify stop and return without enqueueing more tasks.
  if (!rx_state.is_running()) {
    rx_state.notify_stop();
    return;
  }

  // Get receive buffer.
  std::unique_ptr<baseband_gateway_buffer_dynamic> rx_buffer = rx_buffers.pop_blocking();

  // Receive baseband.
  trace_point                         tp          = ru_tracer.now();
  baseband_gateway_receiver::metadata rx_metadata = receiver.receive(rx_buffer->get_writer());
  ru_tracer << trace_event("receive_baseband", tp);

  // Update last timestamp.
  last_rx_timestamp.store(rx_metadata.ts + rx_buffer->get_nof_samples(), std::memory_order_release);

  // Queue uplink buffer processing.
  report_fatal_error_if_not(uplink_executor.execute([this, ul_buffer = std::move(rx_buffer), rx_metadata]() mutable {
    trace_point ul_tp = ru_tracer.now();

    // Process UL.
    uplink_processor.process(ul_buffer->get_reader(), rx_metadata.ts);

    // Return buffer to receive.
    rx_buffers.push_blocking(std::move(ul_buffer));

    ru_tracer << trace_event("uplink_baseband", ul_tp);
  }),
                            "Failed to execute uplink processing task.");

  // Enqueue next iteration if it is running.
  report_fatal_error_if_not(rx_executor.defer([this]() { ul_process(); }), "Failed to execute receive task.");
}
