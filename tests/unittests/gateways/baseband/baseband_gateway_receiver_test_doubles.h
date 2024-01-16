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
#include "baseband_gateway_buffer_test_doubles.h"
#include "srsran/gateways/baseband/baseband_gateway_receiver.h"
#include <random>

namespace srsran {

class baseband_gateway_receiver_spy : public baseband_gateway_receiver
{
public:
  struct entry_t {
    baseband_gateway_receiver::metadata metadata;
    baseband_gateway_buffer_read_only   data;
  };

  baseband_gateway_receiver_spy() : sample_dist(-1.0F, +1.0F) {}

  // See interface for documentation.
  metadata receive(baseband_gateway_buffer_writer& data) override
  {
    // Fill data with random samples.
    for (unsigned channel = 0; channel != data.get_nof_channels(); ++channel) {
      span<cf_t> buffer = data[channel];
      std::generate(buffer.begin(), buffer.end(), [this]() { return cf_t(sample_dist(rgen), sample_dist(rgen)); });
    }

    entries.emplace_back();
    entry_t& entry    = entries.back();
    entry.metadata.ts = current_timestamp;
    entry.data        = data;

    current_timestamp += data.get_nof_samples();

    return entry.metadata;
  }

  void set_current_timestamp(baseband_gateway_timestamp timestamp) { current_timestamp = timestamp; }

  /// Gets all receive entries.
  const std::vector<entry_t>& get_entries() const { return entries; }

  /// Clears all types of entries.
  void clear() { entries.clear(); }

private:
  std::mt19937                          rgen;
  std::uniform_real_distribution<float> sample_dist;
  baseband_gateway_timestamp            current_timestamp = 0;
  std::vector<entry_t>                  entries;
};

} // namespace srsran