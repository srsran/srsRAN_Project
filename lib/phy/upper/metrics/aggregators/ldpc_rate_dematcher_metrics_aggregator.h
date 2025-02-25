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
#include <atomic>

namespace srsran {

/// LDPC rate dematcher metric aggregator.
class ldpc_rate_dematcher_metrics_aggregator : public ldpc_rate_dematcher_metric_notifier
{
public:
  /// Gets the average codeblock latency in microseconds.
  double get_avg_cb_latency_us() const
  {
    return count ? static_cast<double>(sum_elapsed_ns) / static_cast<double>(count) * 1e-3 : 0;
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
    return sum_elapsed_ns ? static_cast<double>(sum_input_size) / static_cast<double>(sum_elapsed_ns) * 1000 : 0;
  }

  /// Gets the total processing time.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

  /// Gets the CPU usage in microseconds of the LDPC rate dematcher.
  double get_cpu_usage_us() const { return static_cast<double>(sum_elapsed_ns) / 1000.0; }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_input_size    = 0;
    sum_elapsed_ns    = 0;
    count             = 0;
    min_cb_latency_ns = UINT64_MAX;
    max_cb_latency_ns = 0;
  }

private:
  // See interface for documentation.
  void on_new_metric(const ldpc_rate_dematcher_metrics& metrics) override
  {
    sum_input_size += metrics.input_size.value();
    sum_elapsed_ns += metrics.measurements.duration.count();
    ++count;
    update_minmax(metrics.measurements.duration.count(), max_cb_latency_ns, min_cb_latency_ns);
  }

  std::atomic<uint64_t> sum_input_size    = {};
  std::atomic<uint64_t> sum_elapsed_ns    = {};
  std::atomic<uint64_t> count             = {};
  std::atomic<uint64_t> min_cb_latency_ns = UINT64_MAX;
  std::atomic<uint64_t> max_cb_latency_ns = 0;
};

} // namespace srsran
