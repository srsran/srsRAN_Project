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

/// LDPC decoder metric producer.
class ldpc_decoder_metric_producer_impl : private ldpc_decoder_metric_notifier
{
public:
  /// Gets the LDPC decoder metric interface.
  ldpc_decoder_metric_notifier& get_notifier() { return *this; }

  /// Gets the total number of processed codeblocks.
  uint64_t get_nof_processed_cb() const { return count; }

  /// Gets the average codeblock size in bits.
  double get_avg_cb_size() const { return static_cast<double>(sum_cb_sz) / static_cast<double>(count); }

  /// Gets the average codeblock latency in microseconds.
  double get_avg_cb_latency_us() const
  {
    return static_cast<double>(sum_elapsed_ns) / static_cast<double>(count) * 1e-3;
  }

  /// Gets the processing rate.
  double get_decode_rate_Mbps() const
  {
    return static_cast<double>(sum_cb_sz) / static_cast<double>(sum_elapsed_ns) * 1000;
  }

  /// Gets the total amount of time the LDPC decoder spent decoding.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

private:
  // See interface for documentation.
  void new_metric(const ldpc_decoder_metrics& metrics) override
  {
    sum_cb_sz += metrics.cb_sz.value();
    sum_nof_iterations += metrics.nof_iterations;
    sum_elapsed_ns += metrics.elapsed.count();
    if (metrics.crc_ok) {
      ++sum_crc_ok;
    }
    ++count;
  }

  std::atomic<uint64_t> sum_cb_sz          = {};
  std::atomic<uint64_t> sum_nof_iterations = {};
  std::atomic<uint64_t> sum_crc_ok         = {};
  std::atomic<uint64_t> sum_elapsed_ns     = {};
  std::atomic<uint64_t> count              = {};
};

} // namespace srsran
