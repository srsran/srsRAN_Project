/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/span.h"
#include "srsran/ofh/ethernet/ethernet_mac_address.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ru/ofh/ru_ofh_configuration.h"

namespace srsran {

/// Parses the string containing Ethernet MAC address.
inline bool parse_mac_address(const std::string& mac_str, ether::mac_address& mac)
{
  std::array<unsigned, 6> data       = {};
  int                     bytes_read = std::sscanf(
      mac_str.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &data[0], &data[1], &data[2], &data[3], &data[4], &data[5]);
  if (bytes_read != ether::ETH_ADDR_LEN) {
    fmt::print("Invalid MAC address provided: {}\n", mac_str);
    return false;
  }

  std::copy(data.begin(), data.end(), mac.begin());

  return true;
}

/// Validates the bandwidth argument provided as a user input.
inline bool is_valid_bw(unsigned bandwidth)
{
  // Bandwidth cannot be less than 5MHz.
  if (bandwidth < 5U) {
    return false;
  }

  // Check from [5-25] in steps of 5.
  if (bandwidth < 26U) {
    return ((bandwidth % 5) == 0);
  }

  // Check from [30-100] in steps of 10.
  if (bandwidth < 101U) {
    return ((bandwidth % 10) == 0);
  }

  return false;
}

namespace ru_emu_stats {

/// Helper class that represents a KPI counter.
class kpi_counter
{
  std::atomic<uint64_t> counter{0};
  uint64_t              last_value_printed = 0U;

public:
  /// Returns value accumulated since last call.
  uint64_t calculate_acc_value()
  {
    uint64_t current_value = counter.load(std::memory_order_relaxed);
    uint64_t total         = current_value - last_value_printed;
    last_value_printed     = current_value;
    return total;
  }

  /// Increments value by the given amount.
  void increment(unsigned n = 1) { counter.fetch_add(n, std::memory_order_relaxed); }
};

} // namespace ru_emu_stats
} // namespace srsran
