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

/// LDPC rate dematcher metric producer.
class ldpc_rate_dematcher_metric_producer_impl : private ldpc_rate_dematcher_metric_notifier
{
public:
  /// Gets the LDPC rate dematcher metric interface.
  ldpc_rate_dematcher_metric_notifier& get_notifier() { return *this; }

  /// Gets the average codeblock latency in microseconds.
  double get_avg_cb_latency_us() const
  {
    return static_cast<double>(sum_elapsed_ns) / static_cast<double>(count) * 1e-3;
  }

  /// Gets the average processing rate.
  double get_avg_rate_Mbps() const
  {
    return static_cast<double>(sum_input_size) / static_cast<double>(sum_elapsed_ns) * 1000;
  }

  /// Gets the total processing time.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

private:
  // See interface for documentation.
  void new_metric(const ldpc_rate_dematcher_metrics& metrics) override
  {
    sum_input_size += metrics.input_size.value();
    sum_elapsed_ns += metrics.elapsed.count();
    ++count;
  }

  std::atomic<uint64_t> sum_input_size = {};
  std::atomic<uint64_t> sum_elapsed_ns = {};
  std::atomic<uint64_t> count          = {};
};

} // namespace srsran
