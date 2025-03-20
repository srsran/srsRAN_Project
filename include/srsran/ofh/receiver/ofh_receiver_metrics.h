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

namespace srsran {
namespace ofh {

/// Open Fronthaul received messages metrics.
struct received_messages_metrics {
  /// Number of OFH messages received on time.
  unsigned nof_on_time_messages;
  /// Number of OFH messages received early.
  unsigned nof_early_messages;
  /// Number of OFH messages received late.
  unsigned nof_late_messages;
};

/// Open Fronthaul receiver performance metrics.
struct receiver_performance_metrics {
  /// Receiver message processing metrics.
  struct unpacking_perf_metrics {
    /// Open Fronthaul message unpacking statistics.
    float message_unpacking_min_latency_us;
    float message_unpacking_max_latency_us;
    float message_unpacking_avg_latency_us;

    /// CPU usage in microseconds of the message processing in the receiver.
    float cpu_usage_us;
  };

  unpacking_perf_metrics data_processing_metrics;
  unpacking_perf_metrics prach_processing_metrics;
};

/// Open Fronthaul receiver metrics.
struct receiver_metrics {
  received_messages_metrics    rx_messages_metrics;
  receiver_performance_metrics rx_perf_metrics;
};

} // namespace ofh
} // namespace srsran
