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
#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"
#include <chrono>

namespace srsran {

/// Interface for transmitter alignment from the receiver.
class radio_zmq_tx_align_interface
{
public:
  /// Default destructor.
  virtual ~radio_zmq_tx_align_interface() = default;

  /// Aligns a transmit stream by transmitting zeros until the indicated timestamp.
  virtual bool align(baseband_gateway_timestamp timestamp, std::chrono::milliseconds timeout) = 0;
};

} // namespace srsran