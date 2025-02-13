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

#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/metrics/phy_metrics_reports.h"
#include <atomic>

namespace srsran {

/// Time alignment estimator metrics aggregator.
class time_alignment_estimator_metrics_aggregator : public time_alignment_estimator_metric_notifier
{
public:
  /// Gets the average processing latency in microseconds.
  double get_avg_latency_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_ns) / static_cast<double>(count) * 1e-3
               : 0;
  }

  /// Gets the average processing rate in MREps (millions of resource elements per second).
  double get_avg_rate_MREps() const
  {
    return sum_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_nof_re) / static_cast<double>(sum_elapsed_ns) * 1000
               : 0;
  }

  /// Gets the total amount of time the TA estimator spent calculating.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

  /// Gets the CPU usage in microseconds of TA estimator.
  uint64_t get_cpu_usage_us() const { return sum_used_cpu_time_us; }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_nof_re           = 0;
    sum_elapsed_ns       = 0;
    count                = 0;
    sum_used_cpu_time_us = 0;
  }

private:
  // See interface for documentation.
  void on_new_metric(const time_alignment_estimator_metrics& metrics) override
  {
    sum_nof_re += metrics.nof_re;
    sum_elapsed_ns += metrics.elapsed.count();
    ++count;

    sum_used_cpu_time_us += (metrics.cpu_measurements.user_time.count() + metrics.cpu_measurements.system_time.count());
  }

  std::atomic<uint64_t> sum_nof_re           = {};
  std::atomic<uint64_t> sum_elapsed_ns       = {};
  std::atomic<uint64_t> count                = {};
  std::atomic<uint64_t> sum_used_cpu_time_us = {};
};

} // namespace srsran
