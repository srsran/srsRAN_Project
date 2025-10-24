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

/// LDPC encoder metric aggregator.
class ldpc_encoder_metrics_aggregator : public ldpc_encoder_metric_notifier
{
public:
  /// Gets the total number of processed codeblocks.
  uint64_t get_nof_processed_cb() const { return count.load(std::memory_order_relaxed); }

  /// Gets the average codeblock size in bits.
  double get_avg_cb_size() const
  {
    return count.load(std::memory_order_relaxed) ? static_cast<double>(sum_cb_sz.load(std::memory_order_relaxed)) /
                                                       static_cast<double>(count.load(std::memory_order_relaxed))
                                                 : 0.0;
  }

  /// Gets the average codeblock latency in microseconds.
  double get_avg_cb_latency_us() const
  {
    return count.load(std::memory_order_relaxed) ? static_cast<double>(sum_elapsed_ns.load(std::memory_order_relaxed)) /
                                                       static_cast<double>(count.load(std::memory_order_relaxed)) * 1e-3
                                                 : 0.0;
  }

  /// Gets the minimum codeblock processing latency in microseconds.
  double get_min_cb_latency_us() const
  {
    return (min_cb_latency_ns.load(std::memory_order_relaxed) == UINT64_MAX)
               ? 0
               : static_cast<double>(min_cb_latency_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Gets the maximum codeblock processing latency in microseconds.
  double get_max_cb_latency_us() const
  {
    return static_cast<double>(max_cb_latency_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Gets the processing rate.
  double get_encode_rate_Mbps() const
  {
    return sum_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_cb_sz.load(std::memory_order_relaxed)) /
                     static_cast<double>(sum_elapsed_ns.load(std::memory_order_relaxed)) * 1000
               : 0.0;
  }

  /// Gets the total amount of time the LDPC encoder spent encoding.
  std::chrono::nanoseconds get_total_time() const
  {
    return std::chrono::nanoseconds(sum_elapsed_ns.load(std::memory_order_relaxed));
  }

  /// Gets the CPU usage in microseconds of the LDPC encoder.
  double get_cpu_usage_us() const
  {
    return static_cast<double>(sum_elapsed_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_cb_sz.store(0, std::memory_order_relaxed);
    sum_elapsed_ns.store(0, std::memory_order_relaxed);
    count.store(0, std::memory_order_relaxed);
    min_cb_latency_ns.store(UINT64_MAX, std::memory_order_relaxed);
    max_cb_latency_ns.store(0, std::memory_order_relaxed);
  }

private:
  // See interface for documentation.
  void on_new_metric(const ldpc_encoder_metrics& metrics) override
  {
    sum_cb_sz.fetch_add(metrics.cb_sz.value(), std::memory_order_relaxed);
    sum_elapsed_ns.fetch_add(metrics.measurements.duration.count(), std::memory_order_relaxed);
    count.fetch_add(1, std::memory_order_relaxed);
    update_minmax(metrics.measurements.duration.count(), max_cb_latency_ns, min_cb_latency_ns);
  }

  std::atomic<uint64_t> sum_cb_sz         = {};
  std::atomic<uint64_t> sum_elapsed_ns    = {};
  std::atomic<uint64_t> count             = {};
  std::atomic<uint64_t> min_cb_latency_ns = UINT64_MAX;
  std::atomic<uint64_t> max_cb_latency_ns = 0;
};

} // namespace srsran
