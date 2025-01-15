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

#include "baseband_gateway_receiver_test_doubles.h"
#include "baseband_gateway_transmitter_test_doubles.h"
#include "srsran/gateways/baseband/baseband_gateway.h"
#include <random>

namespace srsran {

/// \brief Baseband gateway spy class.
///
/// Records all interactions from a lower physical layer with a baseband gateway. The receive method generates random
/// baseband samples and metadata.
class baseband_gateway_spy : public baseband_gateway
{
public:
  // See interface for documentation.
  baseband_gateway_transmitter& get_transmitter() override { return transmitter; }

  // See interface for documentation.
  baseband_gateway_receiver& get_receiver() override { return receiver; }

  /// Gets all transmit entries.
  const std::vector<baseband_gateway_transmitter_spy::entry_t>& get_transmit_entries() const
  {
    return transmitter.get_entries();
  }

  /// Gets all receive entries.
  const std::vector<baseband_gateway_receiver_spy::entry_t>& get_receive_entries() const
  {
    return receiver.get_entries();
  }

  void set_receiver_current_timestamp(baseband_gateway_timestamp timestamp)
  {
    receiver.set_current_timestamp(timestamp);
  }

  /// Clears all types of entries.
  void clear_all_entries()
  {
    transmitter.clear();
    receiver.clear();
  }

private:
  baseband_gateway_transmitter_spy transmitter;
  baseband_gateway_receiver_spy    receiver;
};

} // namespace srsran
