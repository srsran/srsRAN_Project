
#include "radio_zmq_rx_channel.h"

using namespace srsgnb;

const std::set<int> radio_zmq_rx_channel::VALID_SOCKET_TYPES = {ZMQ_REQ};

radio_zmq_rx_channel::radio_zmq_rx_channel(void*                       zmq_context,
                                           const channel_description&  config,
                                           radio_notification_handler& notification_handler_,
                                           task_executor&              async_executor_) :
  stream_id(config.stream_id),
  channel_id(config.channel_id),
  socket_type(config.socket_type),
  logger(srslog::fetch_basic_logger(config.channel_id_str, false)),
  circular_buffer(config.buffer_size),
  buffer(config.buffer_size * sizeof(radio_sample_type)),
  notification_handler(notification_handler_),
  async_executor(async_executor_)
{
  // Set log level.
  logger.set_level(srslog::str_to_basic_level(config.log_level));

  // Validate the socket type.
  if (VALID_SOCKET_TYPES.count(config.socket_type) == 0) {
    logger.error("Invalid receiver type {} ({}).\n", config.socket_type, config.address);
    return;
  }

  // Create socket.
  sock = zmq_socket(zmq_context, config.socket_type);
  if (sock == nullptr) {
    logger.error("Failed to open transmitter socket ({}). {}.\n", config.address, zmq_strerror(zmq_errno()));
    return;
  }

  // Bind socket.
  logger.debug("Connecting to address {}.", config.address);
  if (zmq_connect(sock, config.address.c_str()) == -1) {
    logger.error("Failed to bind transmitter socket ({}). {}.\n", config.address, zmq_strerror(zmq_errno()));
    return;
  }

  // If a timeout is set...
  if (config.trx_timeout_ms) {
    int timeout = config.trx_timeout_ms;

    // Set receive timeout.
    if (zmq_setsockopt(sock, ZMQ_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
      logger.error("Failed to set receive timeout on tx socket. {}.\n", zmq_strerror(zmq_errno()));
      return;
    }

    // Set send timeout.
    if (zmq_setsockopt(sock, ZMQ_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
      logger.error("Failed to set send timeout on tx socket. {}.\n", zmq_strerror(zmq_errno()));
      return;
    }

    // Set linger timeout.
    timeout = config.linger_timeout_ms;
    if (zmq_setsockopt(sock, ZMQ_LINGER, &timeout, sizeof(timeout)) == -1) {
      logger.error("Failed to set linger timeout on tx socket. {}.\n", zmq_strerror(zmq_errno()));
      return;
    }
  }

  // Indicate the initialization was successful.
  state_fsm.init_successful();

  // Start processing.
  async_executor.defer([this]() { run_async(); });
}

radio_zmq_rx_channel::~radio_zmq_rx_channel()
{
  // Close socket if opened.
  if (sock != nullptr) {
    zmq_close(sock);
    sock = nullptr;
  }
}

void radio_zmq_rx_channel::send_request()
{
  // Receive Transmit request is socket type is REPLY and no request is available.
  if (socket_type == ZMQ_REQ) {
    // Receive request.
    uint8_t dummy = 0;
    int     n     = zmq_send(sock, &dummy, sizeof(dummy), 0);

    // Request received.
    if (n > 0) {
      logger.debug("Socket sent request.");
      state_fsm.request_sent();
      return;
    }

    // Error.
    if (n < 0) {
      // Error happened.
      int err = zmq_errno();
      if (err == EFSM || err == EAGAIN) {
        // Ignore timeout and FSM error.
        // logger.debug("Exception to send request. {}.", zmq_strerror(zmq_errno()));
      } else {
        // This error cannot be ignored.
        logger.error("Socket failed to send request. {}.", zmq_strerror(zmq_errno()));
        state_fsm.on_error();
        return;
      }
    }
  }

  // Implement other socket types here.
  // ...
}

void radio_zmq_rx_channel::receive_response()
{
  // Otherwise, send samples over socket.
  int sample_size = sizeof(radio_sample_type);
  int nbytes      = buffer.size() * sample_size;
  int n           = zmq_recv(sock, (void*)buffer.data(), nbytes, ZMQ_DONTWAIT);

  // Check if an error occurred.
  if (n < 0) {
    // Error happened.
    int err = zmq_errno();
    if (err == EFSM || err == EAGAIN) {
      // Ignore timeout and FSM error.
      // logger.debug("Exception to receive data. {}.", zmq_strerror(zmq_errno()));
      return;
    } else {
      // This error cannot be ignored.
      logger.error("Socket failed to receive DATA. {}.", zmq_strerror(zmq_errno()));
      state_fsm.on_error();
      return;
    }
  }

  // Make sure the received number of bytes is valid.
  if (n % sample_size != 0) {
    logger.error("Socket failed to receive DATA. Invalid number of bytes {}%{}={}.", n, sample_size, n % sample_size);
    state_fsm.on_error();
    return;
  }

  // Convert number of bytes to samples.
  unsigned nsamples = n / sample_size;
  logger.debug("Socket received {} samples.", nsamples);

  // Make sure the buffer size has not been exceeded.
  srsran_always_assert(nsamples <= buffer.size(),
                       "Buffer overflow. Buffer size ({}) is not enough for the received number of samples ({})",
                       buffer.size(),
                       nsamples);

  for (unsigned count = 0; count != nsamples; ++count) {
    while (state_fsm.is_running() && !circular_buffer.try_push(buffer[count])) {
      // Notify buffer overflow.
      radio_notification_handler::event_description event;
      event.stream_id  = stream_id;
      event.channel_id = channel_id;
      event.source     = radio_notification_handler::event_source::RECEIVE;
      event.type       = radio_notification_handler::event_type::OVERFLOW;
      notification_handler.on_radio_rt_event(event);

      // Wait some time before trying again.
      unsigned sleep_for_ms = CIRC_BUFFER_TRY_PUSH_SLEEP_FOR_MS;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_for_ms));
    }
  }

  // If successful transition to wait for data.
  state_fsm.data_received();
}

void radio_zmq_rx_channel::run_async()
{
  // Transmit request if it has no pending response, otherwise receive response.
  if (!state_fsm.has_pending_response()) {
    send_request();
  } else {
    receive_response();
  }

  // Feedback task if not stopped.
  if (state_fsm.is_running()) {
    async_executor.defer([this]() { run_async(); });
  } else {
    logger.debug("Stopped asynchronous task.");
    state_fsm.async_task_stopped();
  }
}

void radio_zmq_rx_channel::receive(span<radio_sample_type> data)
{
  logger.debug("Requested to receive {} samples.", data.size());

  // For each sample...
  for (radio_sample_type& sample : data) {
    // Try to push sample.
    while (state_fsm.is_running() && !circular_buffer.try_pop(sample)) {
      // Notify buffer underflow.
      radio_notification_handler::event_description event;
      event.stream_id  = stream_id;
      event.channel_id = channel_id;
      event.source     = radio_notification_handler::event_source::RECEIVE;
      event.type       = radio_notification_handler::event_type::UNDERFLOW;
      notification_handler.on_radio_rt_event(event);

      // Wait some time before trying again.
      unsigned sleep_for_ms = CIRC_BUFFER_TRY_POP_SLEEP_FOR_MS;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_for_ms));
    }
  }
}

void radio_zmq_rx_channel::stop()
{
  logger.debug("Stopping...");
  state_fsm.stop();
}

void radio_zmq_rx_channel::wait_stop()
{
  state_fsm.wait_stop();
  logger.debug("Stopped successfully.");
}