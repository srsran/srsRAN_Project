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

/// Open Fronthaul Ethernet transmitter metrics.
struct transmitter_metrics {
  /// Average instantaneous transmission throughput in Megabits per second.
  float avg_rate_Mbps;
  /// Average latency of packet transmission in microseconds.
  float avg_packet_tx_latency_us;
  /// Maximum latency of packet transmission in microseconds.
  float max_packet_tx_latency_us;
  /// Minimum latency of packet transmission in microseconds.
  float min_packet_tx_latency_us;
  /// Total number of bytes transmitted during metrics period.
  uint64_t total_nof_bytes;
  /// CPU usage in microseconds of the Ethernet transmitter.
  float cpu_usage_us;
};

} // namespace ether
} // namespace srsran
