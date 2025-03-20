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
