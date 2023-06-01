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

#pragma once

#include "radio_zmq_tx_channel_fsm.h"
#include "srsran/adt/blocking_queue.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "srsran/radio/radio_notification_handler.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/async/async_queue.h"
#include "srsran/support/executors/task_executor.h"
#include <atomic>
#include <set>
#include <zmq.h>

namespace srsran {

class radio_zmq_tx_channel
{
private:
  /// Lists the supported socket types.
  static const std::set<int> VALID_SOCKET_TYPES;
  /// Wait time after a buffer try push failed.
  static constexpr unsigned CIRC_BUFFER_TRY_PUSH_SLEEP_FOR_MS = 1;
  /// Maximum number of trials for binding.
  static constexpr unsigned BIND_MAX_TRIALS = 10;
  /// Sleep time after a bind failure in seconds.
  static constexpr unsigned BIND_FAILURE_SLEEP_SECONDS = 1;

  /// Indicates the stream identifier.
  unsigned stream_id;
  /// Channel identifier.
  unsigned channel_id;
  /// Indicates the stream identifier string.
  std::string channel_id_str;
  /// Indicates the current state of the stream.
  radio_zmq_tx_channel_fsm state_fsm;
  /// ZMQ socket.
  void* sock = nullptr;
  /// ZMQ socket type.
  int socket_type;
  /// Logger.
  srslog::basic_logger& logger;
  /// Stores transmit buffer.
  blocking_queue<cf_t> circular_buffer;
  /// Transmission buffer.
  std::vector<cf_t> buffer;
  /// Notification handler.
  radio_notification_handler& notification_handler;
  /// Asynchronous task executor.
  task_executor& async_executor;
  /// Indicates the number of transmitted samples. Protected for concurrent read-write.
  std::atomic<uint64_t> sample_count = {0};
  /// Protects concurrent transmit alignment operations from the receiver thread.
  std::mutex              transmit_alignment_mutex;
  std::condition_variable transmit_alignment_cvar;

  std::atomic<bool> is_tx_enabled = {false};

  /// Transmits a single sample.
  void transmit_samples(span<const cf_t> data);

public:
  /// Describes the necessary parameters to create a ZMQ Tx channel.
  struct channel_description {
    /// Indicates the socket type.
    int socket_type;
    /// Indicates the address to bind.
    std::string address;
    /// Stream identifier.
    unsigned stream_id;
    /// Channel identifier.
    unsigned channel_id;
    /// Stream identifier string.
    std::string channel_id_str;
    /// Logging level.
    std::string log_level;
    /// Indicates the socket send and receive timeout in milliseconds. It is ignored if it is zero.
    unsigned trx_timeout_ms;
    /// Indicates the socket linger timeout in milliseconds. If is ignored if trx_timeout_ms is zero.
    unsigned linger_timeout_ms;
    /// Buffer size.
    unsigned buffer_size;
  };

  radio_zmq_tx_channel(void*                       zmq_context,
                       const channel_description&  config,
                       radio_notification_handler& notification_handler,
                       task_executor&              async_executor_);

  ~radio_zmq_tx_channel();

  bool is_successful() const { return state_fsm.is_initiated(); }

  void receive_request();

  void send_response();

  void run_async();

  bool align(uint64_t timestamp, std::chrono::milliseconds timeout);

  void transmit(span<const cf_t> buffer);

  void start(uint64_t init_time);

  void stop();

  void wait_stop();
};

} // namespace srsran
