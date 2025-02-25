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

#include "aggregator_helpers.h"
#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/metrics/phy_metrics_reports.h"
#include <atomic>

namespace srsran {

/// LDPC rate matcher metrics aggregator.
class ldpc_rate_matcher_metrics_aggregator : public ldpc_rate_matcher_metric_notifier
{
public:
  /// Gets the average codeblock latency in microseconds.
  double get_avg_cb_latency_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_ns) / static_cast<double>(count) * 1e-3
               : 0.0;
  }

  /// Gets the minimum codeblock processing latency in microseconds.
  double get_min_cb_latency_us() const
  {
    return (min_cb_latency_ns == UINT64_MAX) ? 0 : static_cast<double>(min_cb_latency_ns) / 1000.0;
  }

  /// Gets the maximum codeblock processing latency in microseconds.
  double get_max_cb_latency_us() const { return static_cast<double>(max_cb_latency_ns) / 1000.0; }

  /// Gets the average processing rate.
  double get_avg_rate_Mbps() const
  {
    return sum_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_output_size) / static_cast<double>(sum_elapsed_ns) * 1000
               : 0.0;
  }

  /// Gets the total processing time.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

  /// Gets the CPU usage in microseconds of the LDPC rate matcher.
  double get_cpu_usage_us() const { return static_cast<double>(sum_elapsed_ns) / 1000.0; }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_output_size   = 0;
    sum_elapsed_ns    = 0;
    count             = 0;
    min_cb_latency_ns = UINT64_MAX;
    max_cb_latency_ns = 0;
  }

private:
  // See interface for documentation.
  void on_new_metric(const ldpc_rate_matcher_metrics& metrics) override
  {
    sum_output_size += metrics.output_size.value();
    sum_elapsed_ns += metrics.measurements.duration.count();
    ++count;
    update_minmax(metrics.measurements.duration.count(), max_cb_latency_ns, min_cb_latency_ns);
  }

  std::atomic<uint64_t> sum_output_size   = {};
  std::atomic<uint64_t> sum_elapsed_ns    = {};
  std::atomic<uint64_t> count             = {};
  std::atomic<uint64_t> min_cb_latency_ns = UINT64_MAX;
  std::atomic<uint64_t> max_cb_latency_ns = 0;
};

} // namespace srsran
