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

#include "radio_zmq_rx_stream.h"
#include "radio_zmq_tx_stream.h"
#include "srsran/gateways/baseband/baseband_gateway.h"
#include <memory>

namespace srsran {

/// \brief Implement baseband gateway interface for UHD.
///
/// It contains a transmit stream and a receive stream.
class radio_zmq_baseband_gateway : public baseband_gateway
{
public:
  radio_zmq_baseband_gateway(void*                                          zmq_context,
                             task_executor&                                 async_executor_,
                             radio_notification_handler&                    notification_handler,
                             const radio_zmq_tx_stream::stream_description& tx_stream_config,
                             const radio_zmq_rx_stream::stream_description& rx_stream_config) :
    tx_stream(zmq_context, tx_stream_config, async_executor_, notification_handler),
    rx_stream(zmq_context, rx_stream_config, async_executor_, tx_stream, notification_handler)
  {
  }

  // See interface for documentation.
  baseband_gateway_transmitter& get_transmitter() override { return tx_stream; }

  // See interface for documentation.
  baseband_gateway_receiver& get_receiver() override { return rx_stream; }

  /// Gets the transmit stream.
  radio_zmq_tx_stream& get_tx_stream() { return tx_stream; }

  /// Gets the transmit stream.
  radio_zmq_rx_stream& get_rx_stream() { return rx_stream; }

private:
  /// Transmit stream.
  radio_zmq_tx_stream tx_stream;
  /// Receive stream.
  radio_zmq_rx_stream rx_stream;
};

} // namespace srsran
