
#include "radio_zmq_rx_stream.h"

using namespace srsgnb;

radio_zmq_rx_stream::radio_zmq_rx_stream(void*                       zmq_context,
                                         const stream_description&   config,
                                         task_executor&              async_executor_,
                                         radio_notification_handler& notification_handler)
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

void radio_zmq_rx_stream::wait_stop()
{
  for (auto& channel : channels) {
    channel->wait_stop();
  }
}

void radio_zmq_rx_stream::receive(radio_baseband_buffer& data)
{
  srsran_always_assert(data.get_nof_channels() == channels.size(),
                       "Invalid number of channels ({}) expected {}.",
                       data.get_nof_channels(),
                       channels.size());

  for (unsigned channel_id = 0; channel_id != channels.size(); ++channel_id) {
    channels[channel_id]->receive(data.get_channel_buffer(channel_id));
  }
}
