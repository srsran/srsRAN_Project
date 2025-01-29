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
#include <atomic>

namespace srsran {

/// LDPC encoder metric producer.
class ldpc_encoder_metric_producer_impl : private ldpc_encoder_metric_notifier
{
public:
  /// Gets the LDPC encoder metric interface.
  ldpc_encoder_metric_notifier& get_notifier() { return *this; }

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
  double get_encode_rate_Mbps() const
  {
    return static_cast<double>(sum_cb_sz) / static_cast<double>(sum_elapsed_ns) * 1000;
  }

  /// Gets the total amount of time the LDPC encoder spent decoding.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

private:
  // See interface for documentation.
  void new_metric(const ldpc_encoder_metrics& metrics) override
  {
    sum_cb_sz += metrics.cb_sz.value();
    sum_elapsed_ns += metrics.elapsed.count();
    ++count;
  }

  std::atomic<uint64_t> sum_cb_sz      = {};
  std::atomic<uint64_t> sum_elapsed_ns = {};
  std::atomic<uint64_t> count          = {};
};

} // namespace srsran
