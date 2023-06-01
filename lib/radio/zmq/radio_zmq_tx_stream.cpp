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

#include "radio_zmq_tx_stream.h"

using namespace srsran;

radio_zmq_tx_stream::radio_zmq_tx_stream(void*                       zmq_context,
                                         const stream_description&   config,
                                         task_executor&              async_executor_,
                                         radio_notification_handler& notification_handler_) :
  notification_handler(notification_handler_)
{
  // For each channel...
  for (unsigned channel_id = 0; channel_id != config.address.size(); ++channel_id) {
    // Prepare configuration.
    radio_zmq_tx_channel::channel_description channel_description;
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
    channels.emplace_back(std::make_unique<radio_zmq_tx_channel>(
        zmq_context, channel_description, notification_handler_, async_executor_));

    // Check if the channel construction was successful.
    if (!channels.back()->is_successful()) {
      return;
    }
  }

  successful = true;
}

void radio_zmq_tx_stream::start(srsran::baseband_gateway_timestamp init_time)
{
  for (auto& channel : channels) {
    channel->start(init_time);
  }
}

void radio_zmq_tx_stream::stop()
{
  for (auto& channel : channels) {
    channel->stop();
  }
}

void radio_zmq_tx_stream::wait_stop()
{
  for (auto& channel : channels) {
    channel->wait_stop();
  }
}

bool radio_zmq_tx_stream::align(baseband_gateway_timestamp timestamp, std::chrono::milliseconds timeout)
{
  // Returns true if at least one channel is in the past.
  bool timestamp_passed = false;
  for (auto& channel : channels) {
    timestamp_passed = timestamp_passed || channel->align(timestamp, timeout);
  }
  return timestamp_passed;
}

void radio_zmq_tx_stream::transmit(const baseband_gateway_buffer_reader& data, const metadata& md)
{
  report_fatal_error_if_not(data.get_nof_channels() == channels.size(),
                            "Invalid number of channels ({}) expected {}.",
                            data.get_nof_channels(),
                            channels.size());

  // Align stream to the new timestamp.
  bool timestamp_passed = align(md.ts, TRANSMIT_TS_ALIGN_TIMEOUT);

  // Notify that a timestamp is late.
  if (timestamp_passed) {
    radio_notification_handler::event_description event_description;
    event_description.stream_id  = radio_notification_handler::UNKNOWN_ID;
    event_description.channel_id = radio_notification_handler::UNKNOWN_ID;
    event_description.source     = radio_notification_handler::event_source::TRANSMIT;
    event_description.type       = radio_notification_handler::event_type::LATE;
    notification_handler.on_radio_rt_event(event_description);
    return;
  }

  for (unsigned channel_id = 0; channel_id != channels.size(); ++channel_id) {
    channels[channel_id]->transmit(data.get_channel_buffer(channel_id));
  }
}
