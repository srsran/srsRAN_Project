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

/// Helper function to convert array of port indexes to string.
inline std::string port_ids_to_str(span<unsigned> ports)
{
  fmt::memory_buffer str_buffer;
  fmt::format_to(std::back_inserter(str_buffer), "{");
  for (unsigned i = 0, e = ports.size(); i != e; ++i) {
    fmt::format_to(std::back_inserter(str_buffer), "{}{}", ports[i], (i == (e - 1)) ? "}" : ", ");
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

/// Validates the bandwidth argument provided as a user input.
inline bool is_valid_bandwidth(unsigned bandwidth)
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

} // namespace srsran
