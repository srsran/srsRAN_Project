/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "radio_zmq_tx_align_interface.h"
#include "radio_zmq_tx_channel.h"
#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include <memory>

namespace srsran {

/// Implements a gateway receiver based on ZMQ transmit socket.
class radio_zmq_tx_stream : public baseband_gateway_transmitter, public radio_zmq_tx_align_interface
{
private:
  /// Alignment timeout. Waits this time before padding zeros.
  const std::chrono::milliseconds TRANSMIT_TS_ALIGN_TIMEOUT = std::chrono::milliseconds(0);
  /// Radio notification handler interface.
  radio_notification_handler& notification_handler;
  /// Indicates whether the class was initialized successfully.
  bool successful = false;
  /// Stores independent channels.
  std::vector<std::unique_ptr<radio_zmq_tx_channel>> channels;

public:
  /// Describes the necessary parameters to create a ZMQ Tx stream.
  struct stream_description {
    /// Indicates the socket type.
    int socket_type;
    /// Indicates the addresses to bind. The number of elements indicate the number of channels.
    std::vector<std::string> address;
    /// Stream identifier.
    unsigned stream_id;
    /// Stream identifier string.
    std::string stream_id_str;
    /// Logging level.
    srslog::basic_levels log_level;
    /// Indicates the socket send and receive timeout in milliseconds. It is ignored if it is zero.
    unsigned trx_timeout_ms;
    /// Indicates the socket linger timeout in milliseconds. If is ignored if trx_timeout_ms is zero.
    unsigned linger_timeout_ms;
    /// Indicates the channel buffer size.
    unsigned buffer_size;
  };

  radio_zmq_tx_stream(void*                       zmq_context,
                      const stream_description&   config,
                      task_executor&              async_executor_,
                      radio_notification_handler& notification_handler);

  bool is_successful() const { return successful; }

  // See interface for documentation.
  bool align(baseband_gateway_timestamp timestamp, std::chrono::milliseconds timeout) override;

  // See interface for documentation.
  void transmit(const baseband_gateway_buffer_reader& data, const baseband_gateway_transmitter_metadata& md) override;

  void start(baseband_gateway_timestamp init_time);

  void stop();

  void wait_stop();
};

} // namespace srsran
