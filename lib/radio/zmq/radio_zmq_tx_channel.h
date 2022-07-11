/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "radio_zmq_tx_channel_fsm.h"
#include "srsgnb/gateways/baseband/baseband_gateway_buffer.h"
#include "srsgnb/radio/radio_notification_handler.h"
#include "srsgnb/srslog/srslog.h"
#include "srsgnb/support/async/async_queue.h"
#include "srsgnb/support/executors/task_executor.h"
#include <atomic>
#include <set>
#include <zmq.h>

namespace srsgnb {

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
  dyn_blocking_queue<radio_sample_type> circular_buffer;
  /// Transmission buffer.
  std::vector<radio_sample_type> buffer;
  /// Notification handler.
  radio_notification_handler& notification_handler;
  /// Asynchronous task executor.
  task_executor& async_executor;
  /// Indicates the number of transmitted samples.
  uint64_t sample_count = 0;

  /// Transmits a single sample.
  void transmit_sample(radio_sample_type sample);

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

  bool is_successful() const { return state_fsm.is_running(); }

  void receive_request();

  void send_response();

  void run_async();

  void align(uint64_t timestamp);

  void transmit(span<radio_sample_type> buffer);

  void stop();

  void wait_stop();
};

} // namespace srsgnb
