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

#include "ofh_data_flow_uplane_decoding_metrics_collector.h"
#include "srsran/ofh/receiver/ofh_receiver_metrics.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul message receiver metrics collector.
class message_receiver_metrics_collector
{
public:
  explicit message_receiver_metrics_collector(bool is_enabled_) :
    is_enabled(is_enabled_), df_uplink_metrics_collector(is_enabled), df_prach_metrics_collector(is_enabled)
  {
  }

  /// Returns true if the metrics collector is enabled, false otherwise.
  bool enabled() const { return is_enabled; }

  /// Updates the PRACH message decoding statistics given the execution time taken by processing a received message.
  void update_prach_stats(std::chrono::nanoseconds exec_latency)
  {
    df_prach_metrics_collector.update_stats(exec_latency);
  }

  /// Updates the uplink data message decoding statistics given the execution time taken by processing a received
  /// message.
  void update_uplink_stats(std::chrono::nanoseconds exec_latency)
  {
    df_uplink_metrics_collector.update_stats(exec_latency);
  }

  /// Collects message receiver performance metrics.
  void collect_metrics(message_decoding_performance_metrics& metrics)
  {
    if (!enabled()) {
    }

    df_uplink_metrics_collector.collect_metrics(metrics.data_processing_metrics);
    df_prach_metrics_collector.collect_metrics(metrics.prach_processing_metrics);
  }

private:
  const bool                                   is_enabled;
  data_flow_message_decoding_metrics_collector df_uplink_metrics_collector;
  data_flow_message_decoding_metrics_collector df_prach_metrics_collector;
};

} // namespace ofh
} // namespace srsran
