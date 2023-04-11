/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
  baseband_gateway_transmitter& get_transmitter(unsigned stream_id) override
  {
    transmitter.set_stream_id(stream_id);
    return transmitter;
  }

  // See interface for documentation.
  baseband_gateway_receiver& get_receiver(unsigned stream_id) override
  {
    receiver.set_stream_id(stream_id);
    return receiver;
  }

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

  void set_transmitter_buffer_size(unsigned buffer_size) { transmitter.set_buffer_size(buffer_size); }

  void set_receiver_buffer_size(unsigned buffer_size) { receiver.set_buffer_size(buffer_size); }

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
