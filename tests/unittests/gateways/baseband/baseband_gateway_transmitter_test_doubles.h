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
#include "baseband_gateway_buffer_test_doubles.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include <random>

namespace srsran {

class baseband_gateway_transmitter_spy : public baseband_gateway_transmitter
{
public:
  struct entry_t {
    baseband_gateway_transmitter::metadata metadata;
    baseband_gateway_buffer_read_only      data;
  };

  // See interface for documentation.
  void transmit(const baseband_gateway_buffer_reader& data, const baseband_gateway_transmitter::metadata& md) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.metadata = md;
    entry.data     = baseband_gateway_buffer_read_only(data);
  }

  /// Gets all transmit entries.
  const std::vector<entry_t>& get_entries() const { return entries; }

  /// Clears all types of entries.
  void clear() { entries.clear(); }

private:
  std::vector<entry_t> entries;
};

} // namespace srsran