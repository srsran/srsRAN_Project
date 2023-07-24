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
