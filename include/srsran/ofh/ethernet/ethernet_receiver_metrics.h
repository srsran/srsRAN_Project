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

#include <cstdint>

namespace srsran {
namespace ether {

/// Open Fronthaul Ethernet receiver metrics.
struct receiver_metrics {
  /// Average latency of packet reception in microseconds.
  float avg_packet_rx_latency_us;
  /// Maximum latency of packet reception in microseconds.
  float max_packet_rx_latency_us;
  /// Total number of bytes received during metrics period.
  uint64_t total_nof_bytes;
  /// CPU usage in microseconds of the Ethernet receiver.
  float cpu_usage_us;
};

} // namespace ether
} // namespace srsran
