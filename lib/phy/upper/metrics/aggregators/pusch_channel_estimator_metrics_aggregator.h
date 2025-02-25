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
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_ns) / static_cast<double>(count) * 1e-3
               : 0;
  }

  /// Gets the minimum processing processing latency in microseconds.
  double get_min_latency_us() const
  {
    return (min_proc_latency_ns == UINT64_MAX) ? 0 : static_cast<double>(min_proc_latency_ns) / 1000.0;
  }

  /// Gets the maximum processing latency in microseconds.
  double get_max_latency_us() const { return static_cast<double>(max_proc_latency_ns) / 1000.0; }

  /// Gets the average PRB processing rate in millions of PRB-per-second.
  double get_processing_rate() const
  {
    return sum_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_nof_prb) / static_cast<double>(sum_elapsed_ns) * 1e3
               : 0;
  }

  /// Gets the total execution time of the channel estimation.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

  /// Gets the CPU usage in microseconds of the PUSCH channel estimator.
  double get_cpu_usage_us() const { return static_cast<double>(sum_elapsed_ns) / 1000.0; }

  /// Resets values of all internal counters.
  void reset()
  {
    count               = 0;
    sum_nof_prb         = 0;
    sum_elapsed_ns      = 0;
    min_proc_latency_ns = UINT64_MAX;
    max_proc_latency_ns = 0;
  }

private:
  // See interface for documentation.
  void on_new_metric(const pusch_channel_estimator_metrics& metrics) override
  {
    sum_nof_prb += metrics.nof_prb;
    sum_elapsed_ns += metrics.measurements.duration.count();
    ++count;
    update_minmax(metrics.measurements.duration.count(), max_proc_latency_ns, min_proc_latency_ns);
  }

  std::atomic<uint64_t> count               = {};
  std::atomic<uint64_t> sum_nof_prb         = {};
  std::atomic<uint64_t> sum_elapsed_ns      = {};
  std::atomic<uint64_t> min_proc_latency_ns = UINT64_MAX;
  std::atomic<uint64_t> max_proc_latency_ns = 0;
};

} // namespace srsran
