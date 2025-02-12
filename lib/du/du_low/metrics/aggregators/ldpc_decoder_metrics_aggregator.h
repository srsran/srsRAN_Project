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
#include "srsran/support/resource_usage/scoped_resource_usage.h"
#include <atomic>

namespace srsran {

/// LDPC decoder metric producer.
class ldpc_decoder_metrics_aggregator : public ldpc_decoder_metric_notifier
{
public:
  /// Gets the total number of processed codeblocks.
  uint64_t get_nof_processed_cb() const { return count; }

  /// Gets the average codeblock number of iterations.
  double get_avg_nof_iterations() const
  {
    return count.load(std::memory_order_relaxed) ? static_cast<double>(sum_nof_iterations) / static_cast<double>(count)
                                                 : 0;
  }

  /// Gets the average codeblock size in bits.
  double get_avg_cb_size() const
  {
    return count.load(std::memory_order_relaxed) ? static_cast<double>(sum_cb_sz) / static_cast<double>(count) : 0;
  }

  /// Gets the average codeblock latency in microseconds.
  double get_avg_cb_latency_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_ns) / static_cast<double>(count) * 1e-3
               : 0;
  }

  /// Gets the processing rate.
  double get_decode_rate_Mbps() const
  {
    return sum_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_cb_sz) / static_cast<double>(sum_elapsed_ns) * 1000
               : 0;
  }

  /// Gets the total amount of time the LDPC decoder spent decoding.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

  /// Gets the CPU utilization of the LDPC decoder.
  double get_cpu_utilization() const
  {
    return sum_measurement_time_us.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_used_cpu_time_us) / static_cast<double>(sum_measurement_time_us)
               : 0.0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_cb_sz               = 0;
    sum_nof_iterations      = 0;
    sum_elapsed_ns          = 0;
    count                   = 0;
    sum_crc_ok              = 0;
    sum_used_cpu_time_us    = 0;
    sum_measurement_time_us = 0;
  }

private:
  // See interface for documentation.
  void on_new_metric(const ldpc_decoder_metrics& metrics) override
  {
    sum_cb_sz += metrics.cb_sz.value();
    sum_nof_iterations += metrics.nof_iterations;
    sum_elapsed_ns += metrics.elapsed.count();
    if (metrics.crc_ok) {
      ++sum_crc_ok;
    }
    ++count;

    sum_used_cpu_time_us += (metrics.cpu_measurements.user_time.count() + metrics.cpu_measurements.system_time.count());
    sum_measurement_time_us += metrics.cpu_measurements.duration.count();
  }

  std::atomic<uint64_t> sum_cb_sz               = {};
  std::atomic<uint64_t> sum_nof_iterations      = {};
  std::atomic<uint64_t> sum_crc_ok              = {};
  std::atomic<uint64_t> sum_elapsed_ns          = {};
  std::atomic<uint64_t> count                   = {};
  std::atomic<uint64_t> sum_used_cpu_time_us    = {};
  std::atomic<uint64_t> sum_measurement_time_us = {};
};

} // namespace srsran
