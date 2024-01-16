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

#include "radio_uhd_rx_stream.h"
#include "radio_uhd_tx_stream.h"
#include "srsran/gateways/baseband/baseband_gateway.h"
#include <memory>

namespace srsran {

/// \brief Implement baseband gateway interface for UHD.
///
/// It contains a transmit stream and a receive stream.
class radio_uhd_baseband_gateway : public baseband_gateway
{
public:
  radio_uhd_baseband_gateway(radio_uhd_device&                              device,
                             task_executor&                                 async_executor,
                             radio_notification_handler&                    notifier,
                             const radio_uhd_tx_stream::stream_description& tx_stream_config,
                             const radio_uhd_rx_stream::stream_description& rx_stream_config) :
    tx_stream(device.create_tx_stream(async_executor, notifier, tx_stream_config)),
    rx_stream(device.create_rx_stream(notifier, rx_stream_config))
  {
  }

  // See interface for documentation.
  baseband_gateway_transmitter& get_transmitter() override { return *tx_stream; }

  // See interface for documentation.
  baseband_gateway_receiver& get_receiver() override { return *rx_stream; }

  // See interface for documentation.
  unsigned get_transmitter_optimal_buffer_size() const override { return tx_stream->get_buffer_size(); }

  // See interface for documentation.
  unsigned get_receiver_optimal_buffer_size() const override { return rx_stream->get_buffer_size(); }

  /// Gets the transmit stream instance.
  radio_uhd_tx_stream& get_tx_stream() { return *tx_stream; }

  /// Gets the transmit stream.
  radio_uhd_rx_stream& get_rx_stream() { return *rx_stream; }

  /// Checks if the streams are successfully initialized.
  bool is_successful() const
  {
    if (!tx_stream || !rx_stream) {
      return false;
    }

    return tx_stream->is_successful() && rx_stream->is_successful();
  }

private:
  /// Transmit stream.
  std::unique_ptr<radio_uhd_tx_stream> tx_stream;
  /// Receive stream.
  std::unique_ptr<radio_uhd_rx_stream> rx_stream;
};

} // namespace srsran
