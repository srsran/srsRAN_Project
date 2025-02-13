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

/// PDSCH DM-RS generator metrics aggregator.
class pdsch_dmrs_generator_metrics_aggregator : public pdsch_dmrs_generator_metric_notifier
{
public:
  /// Gets the average latency in microseconds.
  double get_avg_latency_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_ns) / static_cast<double>(count) * 1e-3
               : 0;
  }

  /// Gets the total processing time.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

  /// Gets the CPU usage in microseconds of the DM-RS generator.
  uint64_t get_cpu_usage_us() const { return sum_used_cpu_time_us; }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_elapsed_ns       = 0;
    count                = 0;
    sum_used_cpu_time_us = 0;
  }

private:
  // See interface for documentation.
  void on_new_metric(const pdsch_dmrs_generator_metrics& metrics) override
  {
    sum_elapsed_ns += metrics.elapsed.count();
    ++count;

    sum_used_cpu_time_us += (metrics.cpu_measurements.user_time.count() + metrics.cpu_measurements.system_time.count());
  }

  std::atomic<uint64_t> sum_elapsed_ns       = {};
  std::atomic<uint64_t> count                = {};
  std::atomic<uint64_t> sum_used_cpu_time_us = {};
};

} // namespace srsran
