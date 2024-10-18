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

#include "srsran/adt/span.h"
#include "srsran/ofh/ethernet/ethernet_mac_address.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ru/ru_ofh_configuration.h"

namespace srsran {

/// Helper function to convert array of port indexes to string.
inline std::string port_ids_to_str(span<unsigned> ports)
{
  fmt::memory_buffer str_buffer;
  fmt::format_to(str_buffer, "{");
  for (unsigned i = 0, e = ports.size(); i != e; ++i) {
    fmt::format_to(str_buffer, "{}{}", ports[i], (i == (e - 1)) ? "}" : ", ");
  }
  return to_string(str_buffer);
}

/// Helper function to parse list of ports provided as a string.
inline std::vector<unsigned> parse_port_id(const std::string& port_id_str)
{
  std::vector<unsigned> port_ids;
  size_t                start_pos = port_id_str.find('{');
  size_t                end_pos   = port_id_str.find('}');
  if (start_pos == std::string::npos || end_pos == std::string::npos) {
    return port_ids;
  }
  std::string       ports_comma_separated = port_id_str.substr(start_pos + 1, end_pos - 1);
  std::stringstream ss(ports_comma_separated);
  int               port;
  while (ss >> port) {
    port_ids.push_back(port);
    if (ss.peek() == ',' || ss.peek() == ' ') {
      ss.ignore();
    }
  }
  return port_ids;
}

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
