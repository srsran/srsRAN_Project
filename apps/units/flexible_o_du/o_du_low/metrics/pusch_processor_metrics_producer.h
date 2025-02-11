/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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

  /// Gets the total time spent by the PUSCH processors waiting for the asynchronous decoding to be completed.
  std::chrono::nanoseconds get_total_wait_time() const { return std::chrono::nanoseconds(sum_waiting_ns); }

private:
  // See interface for documentation.
  void new_metric(const pusch_processor_metrics& metrics) override
  {
    sum_tbs += metrics.tbs.to_bits().value();
    sum_data_elapsed_ns += metrics.elapsed_data.count();
    sum_return_elapsed_ns += metrics.elapsed_return.count();
    if (metrics.elapsed_data > metrics.elapsed_return) {
      sum_waiting_ns += metrics.elapsed_data.count() - metrics.elapsed_return.count();
    }
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
  std::atomic<uint64_t> sum_waiting_ns        = {};
  std::atomic<uint64_t> count                 = {};
  std::atomic<uint64_t> uci_count             = {};
};

} // namespace srsran
