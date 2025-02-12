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

/// PDSCH DM-RS generator metric producer.
class pdsch_dmrs_generator_metric_producer_impl : private pdsch_dmrs_generator_metric_notifier
{
public:
  /// Gets the PDSCH DM-RS metric interface.
  pdsch_dmrs_generator_metric_notifier& get_notifier() { return *this; }

  /// Gets the average latency in microseconds.
  double get_avg_latency_us() const { return static_cast<double>(sum_elapsed_ns) / static_cast<double>(count) * 1e-3; }

  /// Gets the total processing time.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

private:
  // See interface for documentation.
  void on_new_metric(const pdsch_dmrs_generator_metrics& metrics) override
  {
    sum_elapsed_ns += metrics.elapsed.count();
    ++count;
  }

  std::atomic<uint64_t> sum_elapsed_ns = {};
  std::atomic<uint64_t> count          = {};
};

} // namespace srsran
