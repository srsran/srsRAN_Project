/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
