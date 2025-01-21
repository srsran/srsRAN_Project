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
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/math/math_utils.h"
#include <atomic>

namespace srsran {

/// PUSCH processor metric producer.
class pusch_processor_metric_producer_impl : private pusch_processor_metric_notifier
{
public:
  /// Default constructor.
  pusch_processor_metric_producer_impl() = default;

  /// Gets the LDPC decoder metric interface.
  pusch_processor_metric_notifier& get_notifier() { return *this; }

  /// Gets the average codeblock latency in microseconds.
  double get_avg_data_latency_us() const
  {
    return static_cast<double>(sum_data_elapsed_ns) / static_cast<double>(count) * 1e-3;
  }
  /// Gets the average codeblock latency in microseconds.
  double get_avg_uci_latency_us() const
  {
    return static_cast<double>(sum_uci_elapsed_ns) / static_cast<double>(uci_count) * 1e-3;
  }

  /// \brief Gets the average PUSCH processing transmission rate.
  ///
  /// This metric is calculated as the ratio of the total number of processed bits and the total elapsed time.
  double get_processing_rate_Mbps() const
  {
    return static_cast<double>(sum_tbs) / static_cast<double>(sum_data_elapsed_ns) * 1000;
  }

  /// Gets the total time spend by PUSCH processors.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_data_elapsed_ns); }

private:
  // See interface for documentation.
  void new_metric(const pusch_processor_metrics& metrics) override
  {
    sum_tbs += metrics.tbs.to_bits().value();
    sum_data_elapsed_ns += metrics.elapsed_data.count();
    sum_return_elapsed_ns += metrics.elapsed_return.count();
    if (metrics.crc_ok) {
      ++sum_crc_ok;
    }
    ++count;

    if (metrics.elapsed_uci.has_value()) {
      sum_uci_elapsed_ns += metrics.elapsed_uci.value().count();
      ++uci_count;
    }
  }

  std::atomic<uint64_t> sum_tbs               = {};
  std::atomic<uint64_t> sum_crc_ok            = {};
  std::atomic<uint64_t> sum_return_elapsed_ns = {};
  std::atomic<uint64_t> sum_data_elapsed_ns   = {};
  std::atomic<uint64_t> sum_uci_elapsed_ns    = {};
  std::atomic<uint64_t> count                 = {};
  std::atomic<uint64_t> uci_count             = {};
};

} // namespace srsran
