/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
