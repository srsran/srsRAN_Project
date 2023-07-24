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
