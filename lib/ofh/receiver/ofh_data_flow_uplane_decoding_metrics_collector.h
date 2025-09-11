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

#include "../support/metrics_helpers.h"
#include "srsran/ofh/receiver/ofh_receiver_data_flow_metrics.h"
#include <atomic>
#include <chrono>
#include <limits>

namespace srsran {
namespace ofh {

/// Open Fronthaul receiver data flow performance metrics collector.
class data_flow_message_decoding_metrics_collector
{
public:
  explicit data_flow_message_decoding_metrics_collector(bool is_enabled) : is_disabled(!is_enabled) {}

  /// Returns true if the metrics collector is enabled, false otherwise.
  bool enabled() const { return !is_disabled; }

  /// Updates the performance statistics.
  ///
  /// \param[in] exec_latency Execution time taken by processing a received message.
  void update_stats(std::chrono::nanoseconds exec_latency)
  {
    if (is_disabled) {
      return;
    }

    sum_elapsed_ns.fetch_add(exec_latency.count(), std::memory_order_relaxed);
    count.fetch_add(1u, std::memory_order_relaxed);
    update_minmax(static_cast<uint32_t>(exec_latency.count()), max_latency_ns, min_latency_ns);
  }

  /// Increases by one the number of dropped messages.
  void increase_dropped_messages() { nof_dropped_messages.fetch_add(1, std::memory_order_relaxed); }

  /// Collect the performance metrics of a data flow.
  void collect_metrics(rx_data_flow_perf_metrics& metrics)
  {
    metrics.nof_dropped_messages = nof_dropped_messages.exchange(0, std::memory_order_relaxed);

    uint32_t count_val          = count.load(std::memory_order_relaxed);
    uint32_t min_latency_val_ns = min_latency_ns.load(std::memory_order_relaxed);
    uint32_t max_latency_val_ns = max_latency_ns.load(std::memory_order_relaxed);
    uint64_t sum_elapsed_val_ns = sum_elapsed_ns.load(std::memory_order_relaxed);

    metrics.message_unpacking_avg_latency_us =
        count_val ? static_cast<double>(sum_elapsed_val_ns) / count_val / 1000.0 : 0.f;

    metrics.message_unpacking_min_latency_us =
        (min_latency_val_ns == default_min_latency_ns) ? 0.f : static_cast<float>(min_latency_val_ns) / 1000.f;

    metrics.message_unpacking_max_latency_us =
        (max_latency_val_ns == default_max_latency_ns) ? 0.f : static_cast<float>(max_latency_val_ns) / 1000.f;

    metrics.cpu_usage_us = static_cast<double>(sum_elapsed_val_ns) / 1000.0;

    reset();
  }

private:
  /// Default values for min and max latencies.
  static constexpr uint32_t default_min_latency_ns = std::numeric_limits<uint32_t>::max();
  static constexpr uint32_t default_max_latency_ns = std::numeric_limits<uint32_t>::min();

  /// Resets values of all internal counters.
  void reset()
  {
    count.store(0, std::memory_order_relaxed);
    sum_elapsed_ns.store(0, std::memory_order_relaxed);
    min_latency_ns.store(default_min_latency_ns, std::memory_order_relaxed);
    max_latency_ns.store(default_max_latency_ns, std::memory_order_relaxed);
  }

  std::atomic<uint32_t> count                = {};
  std::atomic<uint64_t> sum_elapsed_ns       = {};
  std::atomic<uint32_t> min_latency_ns       = default_min_latency_ns;
  std::atomic<uint32_t> max_latency_ns       = default_max_latency_ns;
  std::atomic<uint32_t> nof_dropped_messages = {0};

  const bool is_disabled;
};

} // namespace ofh
} // namespace srsran
