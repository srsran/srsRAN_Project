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

#include "radio_zmq_rx_stream.h"

using namespace srsran;

radio_zmq_rx_stream::radio_zmq_rx_stream(void*                         zmq_context,
                                         const stream_description&     config,
                                         task_executor&                async_executor_,
                                         radio_zmq_tx_align_interface& tx_align_,
                                         radio_notification_handler&   notification_handler) :
  tx_align(tx_align_)
{
  // For each channel...
  for (unsigned channel_id = 0; channel_id != config.address.size(); ++channel_id) {
    // Prepare configuration.
    radio_zmq_rx_channel::channel_description channel_description;
    channel_description.socket_type       = config.socket_type;
    channel_description.address           = config.address[channel_id];
    channel_description.stream_id         = config.stream_id;
    channel_description.channel_id        = channel_id;
    channel_description.channel_id_str    = config.stream_id_str + ":" + std::to_string(channel_id);
    channel_description.log_level         = config.log_level;
    channel_description.trx_timeout_ms    = config.trx_timeout_ms;
    channel_description.linger_timeout_ms = config.linger_timeout_ms;
    channel_description.buffer_size       = config.buffer_size;

    // Create channel.
    channels.emplace_back(std::make_unique<radio_zmq_rx_channel>(
        zmq_context, channel_description, notification_handler, async_executor_));

    // Check if the channel construction was successful.
    if (!channels.back()->is_successful()) {
      return;
    }
  }

  successful = true;
}

void radio_zmq_rx_stream::stop()
{
  for (auto& channel : channels) {
    channel->stop();
  }
}

void radio_zmq_rx_stream::start(baseband_gateway_timestamp init_time)
{
  sample_count = init_time;
  for (auto& channel : channels) {
    channel->start();
  }
}

void radio_zmq_rx_stream::wait_stop()
{
  for (auto& channel : channels) {
    channel->wait_stop();
  }
}

baseband_gateway_receiver::metadata radio_zmq_rx_stream::receive(baseband_gateway_buffer_writer& data)
{
  // Make sure the number of data channels is coherent with the number of the stream channels.
  report_fatal_error_if_not(data.get_nof_channels() == channels.size(),
                            "Invalid number of channels ({}) expected {}.",
                            data.get_nof_channels(),
                            channels.size());

  // Prepare return metadata.
  baseband_gateway_receiver::metadata ret;
  ret.ts = get_sample_count();

  // Calculate transmit timestamp that has already expired.
  uint64_t passed_timestamp = ret.ts + data.get_nof_samples();

  // Align all transmit timestamps.
  tx_align.align(passed_timestamp, RECEIVE_TS_ALIGN_TIMEOUT);

  // Receive samples for each channel.
  for (unsigned channel_id = 0; channel_id != channels.size(); ++channel_id) {
    channels[channel_id]->receive(data.get_channel_buffer(channel_id));
  }

  // Increment the number of samples.
  sample_count += data.get_nof_samples();

  return ret;
}
