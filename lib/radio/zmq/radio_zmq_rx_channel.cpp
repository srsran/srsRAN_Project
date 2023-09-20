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

#include "radio_zmq_rx_channel.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

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
  buffer(config.buffer_size * sizeof(cf_t)),
  notification_handler(notification_handler_),
  async_executor(async_executor_)
{
  // Set log level.
  logger.set_level(config.log_level);

  // Validate the socket type.
  if (VALID_SOCKET_TYPES.count(config.socket_type) == 0) {
    logger.error("Invalid receiver type {} ({}).", config.socket_type, config.address);
    return;
  }

  // Create socket.
  sock = zmq_socket(zmq_context, config.socket_type);
  if (sock == nullptr) {
    logger.error("Failed to open transmitter socket ({}). {}.", config.address, zmq_strerror(zmq_errno()));
    return;
  }

  // Bind socket.
  logger.info("Connecting to address {}.", config.address);
  if (zmq_connect(sock, config.address.c_str()) == -1) {
    logger.error("Failed to bind transmitter socket ({}). {}.", config.address, zmq_strerror(zmq_errno()));
    return;
  }

  // If a timeout is set...
  if (config.trx_timeout_ms) {
    int timeout = config.trx_timeout_ms;

    // Set receive timeout.
    if (zmq_setsockopt(sock, ZMQ_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
      logger.error("Failed to set receive timeout on tx socket. {}.", zmq_strerror(zmq_errno()));
      return;
    }

    // Set send timeout.
    if (zmq_setsockopt(sock, ZMQ_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
      logger.error("Failed to set send timeout on tx socket. {}.", zmq_strerror(zmq_errno()));
      return;
    }

    // Set linger timeout.
    timeout = config.linger_timeout_ms;
    if (zmq_setsockopt(sock, ZMQ_LINGER, &timeout, sizeof(timeout)) == -1) {
      logger.error("Failed to set linger timeout on tx socket. {}.", zmq_strerror(zmq_errno()));
      return;
    }
  }

  // Indicate the initialization was successful.
  state_fsm.init_successful();
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
        logger.debug("Exception to send request. {}.", zmq_strerror(zmq_errno()));
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
  int sample_size = sizeof(cf_t);
  int nbytes      = buffer.size() * sample_size;
  int n           = zmq_recv(sock, (void*)buffer.data(), nbytes, ZMQ_DONTWAIT);

  // Make sure the received message has not been truncated.
  if (n > nbytes) {
    logger.error("Truncated {} bytes in ZMQ message.", n - nbytes);
    state_fsm.on_error();
    return;
  }

  // Check if an error occurred.
  if (n < 0) {
    // Error happened.
    int err = zmq_errno();
    if (err == EFSM || err == EAGAIN) {
      // Ignore timeout and FSM error.
      return;
    }

    // This error cannot be ignored.
    logger.error("Socket failed to receive DATA. {}.", zmq_strerror(zmq_errno()));
    state_fsm.on_error();
    return;
  }

  // Make sure the received number of bytes is consistent with the sample number of bytes.
  if (n % sample_size != 0) {
    logger.error("Socket failed to receive DATA. Invalid number of bytes {}%{}={}.", n, sample_size, n % sample_size);
    state_fsm.on_error();
    return;
  }

  // Convert number of bytes to samples.
  unsigned nsamples = n / sample_size;
  logger.debug("Socket received {} samples.", nsamples);

  // Make sure the buffer size has not been exceeded.
  report_fatal_error_if_not(nsamples <= buffer.size(),
                            "Buffer overflow. Buffer size ({}) is not enough for the received number of samples ({})",
                            buffer.size(),
                            nsamples);

  unsigned count = 0;
  while (state_fsm.is_running() && (count != nsamples)) {
    // Try to write samples into the buffer.
    unsigned pushed = circular_buffer.try_push(buffer.begin() + count, buffer.begin() + nsamples);

    // Check if the push was successful.
    if (pushed == 0) {
      // Notify buffer overflow.
      radio_notification_handler::event_description event;
      event.stream_id  = stream_id;
      event.channel_id = channel_id;
      event.source     = radio_notification_handler::event_source::RECEIVE;
      event.type       = radio_notification_handler::event_type::OVERFLOW;
      notification_handler.on_radio_rt_event(event);

      // Wait some time before trying again.
      std::this_thread::sleep_for(circ_buffer_try_push_sleep);
    }

    // Increment sample count.
    count += pushed;
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

  // Check if the state timer expired.
  if (state_fsm.has_wait_expired()) {
    logger.info("Waiting for {}.", state_fsm.has_pending_response() ? "data" : "request");
  }

  // Feedback task if not stopped.
  if (state_fsm.is_running()) {
    if (not async_executor.defer([this]() { run_async(); })) {
      logger.error("Unable to initiate radio zmq async task");
    }
  } else {
    logger.debug("Stopped asynchronous task.");
    state_fsm.async_task_stopped();
  }
}

void radio_zmq_rx_channel::receive(span<cf_t> data)
{
  logger.debug("Requested to receive {} samples.", data.size());

  // Create and start a timer to inform about deadlocks.
  radio_zmq_timer timer(true);

  // Try to read samples from circular buffer.
  unsigned count = 0;
  while (state_fsm.is_running() && (count != data.size())) {
    // Try to pop samples.
    unsigned popped = circular_buffer.try_pop(data.begin() + count, data.end());

    // Wait some time before trying again.
    if (popped == 0) {
      std::this_thread::sleep_for(circ_buffer_try_pop_sleep);
    }

    // Check if an excess of time passed while trying to read samples.
    if (timer.is_expired()) {
      logger.info("Waiting for reading samples. Completed {} of {} samples.", count, data.size());
      timer.start();
    }

    // Increment count.
    count += popped;
  }

  if (!state_fsm.is_running()) {
    srsvec::zero(data);
  }
}

void radio_zmq_rx_channel::start()
{
  if (not async_executor.defer([this]() { run_async(); })) {
    logger.error("Unable to initiate radio zmq execution");
    return;
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
