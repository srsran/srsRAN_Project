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
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/math/math_utils.h"
#include <atomic>

namespace srsran {

/// PUSCH channel estimator metrics aggregator.
class pusch_channel_estimator_metrics_aggregator : public pusch_channel_estimator_metric_notifier
{
public:
  /// Gets the average channel estimation time in microseconds.
  double get_average_latency() const
  {
    return count.load(std::memory_order_relaxed) ? static_cast<double>(sum_elapsed_ns.load(std::memory_order_relaxed)) /
                                                       static_cast<double>(count.load(std::memory_order_relaxed)) * 1e-3
                                                 : 0;
  }

  /// Gets the minimum processing processing latency in microseconds.
  double get_min_latency_us() const
  {
    return (min_proc_latency_ns.load(std::memory_order_relaxed) == UINT64_MAX)
               ? 0
               : static_cast<double>(min_proc_latency_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Gets the maximum processing latency in microseconds.
  double get_max_latency_us() const
  {
    return static_cast<double>(max_proc_latency_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Gets the average PRB processing rate in millions of PRB-per-second.
  double get_processing_rate() const
  {
    return sum_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_nof_prb.load(std::memory_order_relaxed)) /
                     static_cast<double>(sum_elapsed_ns.load(std::memory_order_relaxed)) * 1e3
               : 0;
  }

  /// Gets the total execution time of the channel estimation.
  std::chrono::nanoseconds get_total_time() const
  {
    return std::chrono::nanoseconds(sum_elapsed_ns.load(std::memory_order_relaxed));
  }

  /// Gets the CPU usage in microseconds of the PUSCH channel estimator.
  double get_cpu_usage_us() const
  {
    return static_cast<double>(sum_elapsed_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    count.store(0, std::memory_order_relaxed);
    sum_nof_prb.store(0, std::memory_order_relaxed);
    sum_elapsed_ns.store(0, std::memory_order_relaxed);
    min_proc_latency_ns.store(UINT64_MAX, std::memory_order_relaxed);
    max_proc_latency_ns.store(0, std::memory_order_relaxed);
  }

private:
  // See interface for documentation.
  void on_new_metric(const pusch_channel_estimator_metrics& metrics) override
  {
    sum_nof_prb.fetch_add(metrics.nof_prb, std::memory_order_relaxed);
    sum_elapsed_ns.fetch_add(metrics.measurements.duration.count(), std::memory_order_relaxed);
    count.fetch_add(1, std::memory_order_relaxed);
    update_minmax(metrics.measurements.duration.count(), max_proc_latency_ns, min_proc_latency_ns);
  }

  std::atomic<uint64_t> count               = {};
  std::atomic<uint64_t> sum_nof_prb         = {};
  std::atomic<uint64_t> sum_elapsed_ns      = {};
  std::atomic<uint64_t> min_proc_latency_ns = UINT64_MAX;
  std::atomic<uint64_t> max_proc_latency_ns = 0;
};

} // namespace srsran
