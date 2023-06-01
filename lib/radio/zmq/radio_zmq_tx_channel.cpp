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

#include "radio_zmq_tx_channel.h"

using namespace srsran;

const std::set<int> radio_zmq_tx_channel::VALID_SOCKET_TYPES = {ZMQ_REP};

radio_zmq_tx_channel::radio_zmq_tx_channel(void*                       zmq_context,
                                           const channel_description&  config,
                                           radio_notification_handler& notification_handler_,
                                           task_executor&              async_executor_) :
  stream_id(config.stream_id),
  channel_id(config.channel_id),
  channel_id_str(config.channel_id_str),
  socket_type(config.socket_type),
  logger(srslog::fetch_basic_logger(config.channel_id_str, false)),
  circular_buffer(config.buffer_size),
  buffer(config.buffer_size * sizeof(cf_t)),
  notification_handler(notification_handler_),
  async_executor(async_executor_)
{
  // Set log level.
  logger.set_level(srslog::str_to_basic_level(config.log_level));

  // Validate the socket type.
  if (VALID_SOCKET_TYPES.count(config.socket_type) == 0) {
    logger.error("Invalid transmitter type {} ({}).", config.socket_type, config.address);
    return;
  }

  // Create socket.
  sock = zmq_socket(zmq_context, config.socket_type);
  if (sock == nullptr) {
    logger.error("Failed to open transmitter socket ({}). {}.", config.address, zmq_strerror(zmq_errno()));
    return;
  }

  // Bind socket.
  logger.info("Binding to address {}.", config.address);
  bool     bind_success     = false;
  unsigned bind_trial_count = 0;
  while (!bind_success && bind_trial_count != BIND_MAX_TRIALS) {
    // Sleep if it is not the first trial.
    if (bind_trial_count != 0) {
      zmq_sleep(BIND_FAILURE_SLEEP_SECONDS);
    }

    // Try to bind.
    bind_success = (zmq_bind(sock, config.address.c_str()) == 0);
    if (!bind_success) {
      // Unhandled error.
      if (zmq_errno() != EADDRINUSE) {
        break;
      }

      // Address already in use error.
      logger.warning("Failed to bind transmitter socket ({}). {}. Trying in 1 seconds.",
                     config.address,
                     zmq_strerror(zmq_errno()));
      bind_trial_count++;
    }
  }

  if (!bind_success) {
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

radio_zmq_tx_channel::~radio_zmq_tx_channel()
{
  // Close socket if opened.
  if (sock != nullptr) {
    zmq_close(sock);
    sock = nullptr;
  }
}

void radio_zmq_tx_channel::receive_request()
{
  // Receive Transmit request is socket type is REPLY and no request is available.
  if (socket_type == ZMQ_REP) {
    // Receive request.
    uint8_t dummy = 0;
    int     n     = zmq_recv(sock, &dummy, sizeof(dummy), ZMQ_DONTWAIT);

    // Request received.
    if (n > 0) {
      logger.debug("Socket received request.");
      state_fsm.request_received();
      return;
    }

    // Error.
    if (n < 0) {
      // Error happened.
      int err = zmq_errno();
      if (err == EFSM || err == EAGAIN) {
        // Ignore timeout and FSM error.
      } else {
        // This error cannot be ignored.
        logger.error("Socket failed to receive request. {}.", zmq_strerror(zmq_errno()));
        state_fsm.on_error();
        return;
      }
    }
  }

  // Implement other socket types here.
  // ...
}

void radio_zmq_tx_channel::send_response()
{
  // Try popping samples until the circular buffer is empty.
  unsigned popped = circular_buffer.try_pop(buffer.begin(), buffer.end());
  unsigned count  = popped;
  while (state_fsm.is_running() && popped != 0 && count < buffer.size()) {
    popped = circular_buffer.try_pop(buffer.begin() + count, buffer.end());
    count += popped;
  }

  // If stop was called return without transitioning.
  if (!state_fsm.is_running()) {
    return;
  }

  // If no samples are available return without transitioning state.
  if (count == 0) {
    return;
  }

  // Otherwise, send samples over socket.
  int nbytes = count * sizeof(cf_t);
  int n      = zmq_send(sock, (void*)buffer.data(), nbytes, 0);

  // Check if an error occurred.
  if (n < 0) {
    logger.error("Exception to transmit data. {}.", zmq_strerror(zmq_errno()));
    state_fsm.on_error();
    return;
  }

  // Check if the number of bytes is correct.
  if (n != nbytes) {
    logger.error("Failed to transmit data. Unmatched number of bytes {}!={}.", n, nbytes);
    state_fsm.on_error();
    return;
  }

  logger.debug("Socket sent {} samples.", count);

  // If successful transition to wait for data.
  state_fsm.data_sent();
}

void radio_zmq_tx_channel::run_async()
{
  // Receive request if there is no pending request. Otherwise, send response.
  if (!state_fsm.has_pending_request()) {
    receive_request();
  } else {
    send_response();
  }

  // Feedback task if not stopped.
  if (state_fsm.is_running()) {
    if (not async_executor.defer([this]() { run_async(); })) {
      logger.error("Unable to initiate async task");
    }
  } else {
    logger.debug("Stopped asynchronous task.");
    state_fsm.async_task_stopped();
  }
}

void radio_zmq_tx_channel::transmit_samples(span<const cf_t> data)
{
  unsigned count = 0;
  while (count < data.size() && state_fsm.is_running()) {
    unsigned pushed = circular_buffer.try_push(data.begin() + count, data.end());
    while (state_fsm.is_running() && !pushed) {
      // Notify buffer overflow.
      radio_notification_handler::event_description event;
      event.stream_id  = stream_id;
      event.channel_id = channel_id;
      event.source     = radio_notification_handler::event_source::TRANSMIT;
      event.type       = radio_notification_handler::event_type::OVERFLOW;
      notification_handler.on_radio_rt_event(event);

      // Wait some time before trying again.
      unsigned sleep_for_ms = CIRC_BUFFER_TRY_PUSH_SLEEP_FOR_MS;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_for_ms));
      pushed = circular_buffer.try_push(data.begin() + count, data.end());
    }
    count += pushed;
  }
  // Increment sample count.
  sample_count += count;
}

bool radio_zmq_tx_channel::align(uint64_t timestamp, std::chrono::milliseconds timeout)
{
  unsigned count = 0;

  // Early return if alignment is unnecessary.
  if (sample_count >= timestamp) {
    // Return true if the timestamp is in the past.
    return sample_count > timestamp;
  }

  // Protect concurrent alignment and transmit.
  std::unique_lock<std::mutex> lock(transmit_alignment_mutex);

  // If the channel has never transmitted, skip wait.
  if (is_tx_enabled && (timeout.count() != 0)) {
    // Otherwise, wait for the transmitter to transmit.
    bool is_not_timeout =
        transmit_alignment_cvar.wait_for(lock, timeout, [this, timestamp]() { return sample_count >= timestamp; });
    if (is_not_timeout) {
      return sample_count > timestamp;
    }
    is_tx_enabled = false;
  }

  std::array<cf_t, 1024> zero_buffer = {};
  // Transmit zeros until the sample count reaches the timestamp.
  while (sample_count < timestamp && state_fsm.is_running()) {
    unsigned   to_send = std::min(zero_buffer.size(), timestamp - sample_count);
    span<cf_t> zeros   = span<cf_t>(zero_buffer.begin(), to_send);
    transmit_samples(zeros);
    count += zeros.size();
  }

  if (count > 0) {
    logger.debug("Aligned with {} zeros.", count);
  }
  return false;
}

void radio_zmq_tx_channel::transmit(span<const cf_t> data)
{
  logger.debug("Requested to transmit {} samples.", data.size());

  // Protect concurrent alignment and transmit.
  std::unique_lock<std::mutex> lock(transmit_alignment_mutex);

  // Actual baseband transmission.
  transmit_samples(data);

  // Indicate that transmit is enabled.
  is_tx_enabled = true;

  // Notify transmission.
  transmit_alignment_cvar.notify_all();
}

void radio_zmq_tx_channel::start(uint64_t init_time)
{
  sample_count = init_time;

  // Start processing.
  state_fsm.on_start();
  if (not async_executor.defer([this]() { run_async(); })) {
    logger.error("Unable to initiate radio zmq tx async task");
  }
}

void radio_zmq_tx_channel::stop()
{
  logger.debug("Stopping...");
  state_fsm.stop();
}

void radio_zmq_tx_channel::wait_stop()
{
  state_fsm.wait_stop();
  logger.debug("Stopped successfully.");
}
