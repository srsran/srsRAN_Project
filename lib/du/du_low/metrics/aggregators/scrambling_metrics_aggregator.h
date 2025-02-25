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

/// Pseudo-random sequence generator metrics aggregator.
class scrambling_metrics_aggregator : public pseudo_random_sequence_generator_metric_notifier
{
public:
  /// Gets the average initialization time in microseconds.
  double get_average_init_time_us() const
  {
    return init_count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_init_elapsed_ns) / static_cast<double>(init_count) / 1000.0
               : 0;
  }

  /// Gets the average advance rate in Mbps.
  double get_advance_rate_Mbps() const
  {
    return sum_advance_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_advance_nof_bits) / static_cast<double>(sum_advance_elapsed_ns) * 1000.0
               : 0;
  }

  /// Gets the average generation rate in Mbps.
  double get_generate_rate_Mbps() const
  {
    return sum_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_nof_bits) / static_cast<double>(sum_elapsed_ns) * 1000.0
               : 0;
  }

  /// Gets the total time spent by the scrambler.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

  /// Gets the CPU usage in microseconds of scrambling operations.
  double get_cpu_usage_us() const
  {
    return static_cast<double>(sum_init_elapsed_ns + sum_advance_elapsed_ns + sum_elapsed_ns) / 1000.0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_init_elapsed_ns    = 0;
    init_count             = 0;
    sum_advance_nof_bits   = 0;
    sum_advance_elapsed_ns = 0;
    sum_nof_bits           = 0;
    sum_elapsed_ns         = 0;
    count                  = 0;
  }

private:
  // See interface for documentation.
  void on_new_metric(const pseudo_random_sequence_generator_metrics& metrics) override
  {
    switch (metrics.method) {
      case pseudo_random_sequence_generator_metrics::methods::init:
        sum_init_elapsed_ns += metrics.measurements.duration.count();
        ++init_count;
        break;
      case pseudo_random_sequence_generator_metrics::methods::advance:
        sum_advance_nof_bits += metrics.nof_bits;
        sum_advance_elapsed_ns += metrics.measurements.duration.count();
        break;
      case pseudo_random_sequence_generator_metrics::methods::apply_xor_soft_bit:
      case pseudo_random_sequence_generator_metrics::methods::apply_xor_unpacked:
      case pseudo_random_sequence_generator_metrics::methods::apply_xor_packed:
      case pseudo_random_sequence_generator_metrics::methods::generate_bit_packed:
      case pseudo_random_sequence_generator_metrics::methods::generate_float:
        sum_nof_bits += metrics.nof_bits;
        sum_elapsed_ns += metrics.measurements.duration.count();
        break;
    }
    ++count;
  }

  std::atomic<uint64_t> sum_init_elapsed_ns    = {};
  std::atomic<uint64_t> init_count             = {};
  std::atomic<uint64_t> sum_advance_nof_bits   = {};
  std::atomic<uint64_t> sum_advance_elapsed_ns = {};
  std::atomic<uint64_t> sum_nof_bits           = {};
  std::atomic<uint64_t> sum_elapsed_ns         = {};
  std::atomic<uint64_t> count                  = {};
};

} // namespace srsran
