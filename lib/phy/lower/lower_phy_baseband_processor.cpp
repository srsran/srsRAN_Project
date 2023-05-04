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

#include "lower_phy_baseband_processor.h"

using namespace srsran;

lower_phy_baseband_processor::lower_phy_baseband_processor(const lower_phy_baseband_processor::configuration& config) :
  rx_executor(*config.rx_task_executor),
  tx_executor(*config.tx_task_executor),
  uplink_executor(*config.ul_task_executor),
  downlink_executor(*config.dl_task_executor),
  receiver(*config.receiver),
  transmitter(*config.transmitter),
  uplink_processor(*config.ul_bb_proc),
  downlink_processor(*config.dl_bb_proc),
  rx_buffers(default_nof_rx_buffers),
  tx_buffers(default_nof_tx_buffers),
  rx_to_tx_delay(config.rx_to_tx_delay)
{
  srsran_assert(config.rx_task_executor, "Invalid receive task executor.");
  srsran_assert(config.tx_task_executor, "Invalid transmit task executor.");
  srsran_assert(config.ul_task_executor, "Invalid uplink task executor.");
  srsran_assert(config.dl_task_executor, "Invalid downlink task executor.");
  srsran_assert(config.receiver, "Invalid baseband receiver.");
  srsran_assert(config.transmitter, "Invalid baseband transmitter.");
  srsran_assert(config.ul_bb_proc, "Invalid uplink processor.");
  srsran_assert(config.dl_bb_proc, "Invalid downlink processor.");
  srsran_assert(config.nof_rx_ports != 0, "Invalid number of receive ports.");
  srsran_assert(config.nof_tx_ports != 0, "Invalid number of transmit ports.");
  srsran_assert(config.transmitter->get_buffer_size() != 0, "Invalid buffer size.");
  srsran_assert(config.receiver->get_buffer_size() != 0, "Invalid buffer size.");
  srsran_assert(config.rx_to_tx_delay != 0, "Invalid receive to transmit delay.");

  unsigned buffer_size = std::min(receiver.get_buffer_size(), transmitter.get_buffer_size());

  // Create queue of receive buffers.
  for (unsigned i_buffer = 0; i_buffer != default_nof_rx_buffers; ++i_buffer) {
    rx_buffers.push_blocking(std::make_unique<baseband_gateway_buffer_dynamic>(config.nof_rx_ports, buffer_size));
  }

  // Create queue of transmit buffers.
  for (unsigned i_buffer = 0; i_buffer != default_nof_tx_buffers; ++i_buffer) {
    tx_buffers.push_blocking(std::make_unique<baseband_gateway_buffer_dynamic>(config.nof_tx_ports, buffer_size));
  }
}

void lower_phy_baseband_processor::start()
{
  state.start();
  rx_executor.execute([this]() { process(); });
}

void lower_phy_baseband_processor::stop()
{
  state.stop_and_wait();
}

void lower_phy_baseband_processor::process()
{
  // Check if it is running, notify stop and return without enqueueing more tasks.
  if (!state.is_running()) {
    state.notify_stop();
    return;
  }

  // Get receive buffer.
  std::unique_ptr<baseband_gateway_buffer> rx_buffer = rx_buffers.pop_blocking();

  // Receive baseband.
  baseband_gateway_receiver::metadata rx_metadata = receiver.receive(*rx_buffer);

  baseband_gateway_timestamp transmit_ts = rx_metadata.ts + rx_to_tx_delay;

  // Get transmit buffer. It forces the Rx thread to slow down if there are no more transmit buffers available.
  std::unique_ptr<baseband_gateway_buffer> dl_buffer = tx_buffers.pop_blocking();

  // Enqueue downlink processing.
  downlink_executor.execute([this, transmit_ts, dl_buffer = std::move(dl_buffer)]() mutable {
    // Process downlink buffer.
    downlink_processor.process(*dl_buffer);

    // Enqueue transmission.
    tx_executor.execute([this, transmit_ts, tx_buffer = std::move(dl_buffer)]() mutable {
      // Prepare transmit metadata.
      baseband_gateway_transmitter::metadata tx_metadata;
      tx_metadata.ts = transmit_ts;

      // Transmit buffer.
      transmitter.transmit(*tx_buffer, tx_metadata);

      // Return transmit buffer to the queue.
      tx_buffers.push_blocking(std::move(tx_buffer));
    });
  });

  // Queue uplink buffer processing.
  uplink_executor.execute([this, ul_buffer = std::move(rx_buffer)]() mutable {
    // Process UL.
    uplink_processor.process(*ul_buffer);

    // Return buffer to receive.
    rx_buffers.push_blocking(std::move(ul_buffer));
  });

  // Enqueue next iteration if it is running.
  rx_executor.defer([this]() { process(); });
}
