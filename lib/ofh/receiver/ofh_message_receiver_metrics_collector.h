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

#include "ofh_data_flow_uplane_decoding_metrics_collector.h"
#include "srsran/ofh/receiver/ofh_receiver_metrics.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul message receiver metrics collector.
class message_receiver_metrics_collector
{
public:
  message_receiver_metrics_collector(bool                                          is_enabled_,
                                     data_flow_message_decoding_metrics_collector& df_uplink_metrics_collector_,
                                     data_flow_message_decoding_metrics_collector& df_prach_metrics_collector_) :
    is_enabled(is_enabled_),
    df_uplink_metrics_collector(df_uplink_metrics_collector_),
    df_prach_metrics_collector(df_prach_metrics_collector_)
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

  /// Updates the number of skipped messages by increasing the counter the given value.
  void update_skipped_messages(unsigned value) { nof_skipped_messages.fetch_add(value, std::memory_order_relaxed); }

  /// Increases dropped messages by one.
  void increase_dropped_messages() { nof_dropped_messages.fetch_add(1, std::memory_order_relaxed); }

  /// Collects message receiver performance metrics.
  void collect_metrics(message_decoding_performance_metrics& metrics)
  {
    if (!enabled()) {
    }

    df_uplink_metrics_collector.collect_metrics(metrics.data_processing_metrics);
    df_prach_metrics_collector.collect_metrics(metrics.prach_processing_metrics);
    metrics.nof_dropped_messages = nof_dropped_messages.exchange(0, std::memory_order_relaxed);
    metrics.nof_skipped_messages = nof_skipped_messages.exchange(0, std::memory_order_relaxed);
  }

private:
  const bool                                    is_enabled;
  data_flow_message_decoding_metrics_collector& df_uplink_metrics_collector;
  data_flow_message_decoding_metrics_collector& df_prach_metrics_collector;
  /// \brief Number of possible skipped OFH messages.
  ///
  /// A message is considered skipped when the expected sequence id does not match the  message sequence id, and the
  /// message sequence id value is higher than expected.
  std::atomic<unsigned> nof_skipped_messages = {0};
  /// \brief Number of OFH messages dropped.
  ///
  /// A message is dropped when the expected sequence id does not match the message sequence id and the message sequence
  /// id value is smaller than expected..
  std::atomic<unsigned> nof_dropped_messages = {0};
};

} // namespace ofh
} // namespace srsran
