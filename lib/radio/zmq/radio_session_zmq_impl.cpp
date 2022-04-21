
#include "radio_session_zmq_impl.h"

using namespace srsgnb;

radio_session_zmq_impl::radio_session_zmq_impl(const radio_configuration::radio& config,
                                               task_executor&                    async_task_executor,
                                               radio_notification_handler&       notifier) :
  logger(srslog::fetch_basic_logger("ZMQ Session", false))
{
  // Make ZMQ context.
  zmq_context = zmq_ctx_new();
  if (zmq_context == nullptr) {
    logger.error("Failed to create ZMQ context. {}.", zmq_strerror(zmq_errno()));
    return;
  }

  // For each Tx stream.
  for (unsigned stream_id = 0; stream_id != config.tx_streams.size(); ++stream_id) {
    const radio_configuration::stream& radio_stream_config = config.tx_streams[stream_id];

    // Prepare transmit stream configuration.
    radio_zmq_tx_stream::stream_description stream_config;
    stream_config.socket_type = ZMQ_REP;
    for (unsigned channel_id = 0; channel_id != config.tx_streams.size(); ++channel_id) {
      stream_config.address.push_back(radio_stream_config.channels[channel_id].args);
    }
    stream_config.stream_id         = stream_id;
    stream_config.stream_id_str     = "zmq:tx:" + std::to_string(stream_id);
    stream_config.log_level         = config.log_level;
    stream_config.trx_timeout_ms    = DEFAULT_TRX_TIMEOUT_MS;
    stream_config.linger_timeout_ms = DEFAULT_LINGER_TIMEOUT_MS;
    stream_config.buffer_size       = DEFAULT_BUFFER_SIZE_SAMPLES;

    // Create stream.
    tx_streams.push_back(
        std::make_unique<radio_zmq_tx_stream>(zmq_context, stream_config, async_task_executor, notifier));

    // Check is the stream was created successfully.
    if (!tx_streams.back()->is_successful()) {
      return;
    }
  }

  // For each Rx stream.
  for (unsigned stream_id = 0; stream_id != config.rx_streams.size(); ++stream_id) {
    const radio_configuration::stream& radio_stream_config = config.rx_streams[stream_id];

    // Prepare transmit stream configuration.
    radio_zmq_rx_stream::stream_description stream_config;
    stream_config.socket_type = ZMQ_REQ;
    for (unsigned channel_id = 0; channel_id != config.rx_streams.size(); ++channel_id) {
      stream_config.address.push_back(radio_stream_config.channels[channel_id].args);
    }
    stream_config.stream_id         = stream_id;
    stream_config.stream_id_str     = "zmq:rx:" + std::to_string(stream_id);
    stream_config.log_level         = config.log_level;
    stream_config.trx_timeout_ms    = DEFAULT_TRX_TIMEOUT_MS;
    stream_config.linger_timeout_ms = DEFAULT_LINGER_TIMEOUT_MS;
    stream_config.buffer_size       = DEFAULT_BUFFER_SIZE_SAMPLES;

    // Create stream.
    rx_streams.push_back(
        std::make_unique<radio_zmq_rx_stream>(zmq_context, stream_config, async_task_executor, notifier));

    // Check is the stream was created successfully.
    if (!rx_streams.back()->is_successful()) {
      return;
    }
  }

  successful = true;
}

radio_session_zmq_impl::~radio_session_zmq_impl()
{
  // Destroy ZMQ context.
  if (zmq_context != nullptr) {
    zmq_ctx_shutdown(zmq_context);
    zmq_context = nullptr;
  }
}

void radio_session_zmq_impl::stop()
{
  // Signal stop for each transmit stream.
  for (auto& stream : tx_streams) {
    stream->stop();
  }

  // Signal stop for each receive stream.
  for (auto& stream : rx_streams) {
    stream->stop();
  }

  // Wait for transmitter streams to join.
  for (auto& stream : tx_streams) {
    stream->wait_stop();
  }
  for (auto& stream : rx_streams) {
    stream->wait_stop();
  }
}

void radio_session_zmq_impl::transmit(unsigned                                      stream_id,
                                      const radio_data_plane_transmitter::metadata& metadata,
                                      radio_baseband_buffer&                        data)
{
  srsran_always_assert(stream_id < tx_streams.size(),
                       "Stream identifier ({}) exceeds the number of transmit streams ({})",
                       stream_id,
                       tx_streams.size());

  tx_streams[stream_id]->transmit(data);
}

radio_data_plane_receiver::metadata radio_session_zmq_impl::receive(radio_baseband_buffer& data, unsigned stream_id)
{
  srsran_always_assert(stream_id < rx_streams.size(),
                       "Stream identifier ({}) exceeds the number of receive streams ({})",
                       stream_id,
                       rx_streams.size());

  rx_streams[stream_id]->receive(data);

  return radio_data_plane_receiver::metadata();
}

bool radio_session_zmq_impl::set_tx_gain(unsigned port_id, double gain_dB)
{
  return false;
}

bool radio_session_zmq_impl::set_rx_gain(unsigned port_id, double gain_dB)
{
  return false;
}
