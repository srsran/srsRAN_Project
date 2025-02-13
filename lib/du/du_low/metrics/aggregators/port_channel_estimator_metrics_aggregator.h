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

/// Port channel estimator metrics aggregator.
class port_channel_estimator_metrics_aggregator : public port_channel_estimator_metric_notifier
{
public:
  /// Gets the average processing latency in microseconds.
  double get_avg_latency_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_ns) / static_cast<double>(count) * 1e-3
               : 0;
  }

  /// Gets the total amount of time spent in port channel estimator.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_elapsed_ns = 0;
    count          = 0;
  }

private:
  // See interface for documentation.
  void on_new_metric(const port_channel_estimator_metrics& metrics) override
  {
    sum_elapsed_ns += metrics.elapsed.count();
    ++count;
  }

  std::atomic<uint64_t> sum_elapsed_ns = {};
  std::atomic<uint64_t> count          = {};
};

} // namespace srsran
