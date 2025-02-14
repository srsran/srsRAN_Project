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

/// PDSCH processor metrics aggregator.
class pdsch_processor_metrics_aggregator : public pdsch_processor_metric_notifier
{
public:
  /// Gets the total number of processed codeblocks.
  uint64_t get_nof_processed_tbs() const { return count; }

  /// Gets the average transmission latency in microseconds.
  double get_avg_latency_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_completion_ns) / static_cast<double>(count) * 1e-3
               : 0;
  }

  /// Gets the minimum transmission latency in microseconds.
  double get_min_latency_us() const
  {
    return (min_latency_ns == UINT64_MAX) ? 0 : static_cast<double>(min_latency_ns) / 1000.0;
  }

  /// Gets the maximum transmission latency in microseconds.
  double get_max_latency_us() const { return static_cast<double>(max_latency_ns) / 1000.0; }

  /// \brief Gets the average return time in microseconds.
  ///
  /// It is the average processing time of a single-thread PDSCH processor.
  double get_avg_return_time_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_return_ns) / static_cast<double>(count) * 1e-3
               : 0;
  }

  /// Gets the processing rate.
  double get_process_rate_Mbps() const
  {
    return sum_elapsed_completion_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_tb_sz) / static_cast<double>(sum_elapsed_completion_ns) * 1000
               : 0;
  }

  /// Gets the total amount of time the PDSCH processor spent processing.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_completion_ns); }

  /// Gets the CPU usage in microseconds of the PDSCH processor.
  uint64_t get_cpu_usage_us() const { return sum_used_cpu_time_us; }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_tb_sz                 = 0;
    sum_elapsed_return_ns     = 0;
    sum_elapsed_completion_ns = 0;
    count                     = 0;
    sum_used_cpu_time_us      = 0;
    min_latency_ns            = UINT64_MAX;
    max_latency_ns            = 0;
  }

private:
  // See interface for documentation.
  void on_new_metric(const pdsch_processor_metrics& metrics) override
  {
    sum_tb_sz += metrics.tbs.to_bits().value();
    sum_elapsed_return_ns += metrics.elapsed_return.count();
    sum_elapsed_completion_ns += metrics.elapsed_completion.count();
    update_minmax(metrics.elapsed_completion.count(), max_latency_ns, min_latency_ns);
    ++count;
    sum_used_cpu_time_us += metrics.self_cpu_time_usage.count();
  }

  std::atomic<uint64_t> sum_tb_sz                 = {};
  std::atomic<uint64_t> sum_elapsed_return_ns     = {};
  std::atomic<uint64_t> sum_elapsed_completion_ns = {};
  std::atomic<uint64_t> count                     = {};
  std::atomic<uint64_t> sum_used_cpu_time_us      = {};
  std::atomic<uint64_t> min_latency_ns            = UINT64_MAX;
  std::atomic<uint64_t> max_latency_ns            = 0;
};

} // namespace srsran
