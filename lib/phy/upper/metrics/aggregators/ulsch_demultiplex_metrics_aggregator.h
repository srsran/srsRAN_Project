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

/// UL-SCH demultiplexer metrics aggregator.
class ulsch_demultiplex_metrics_aggregator : public ulsch_demultiplex_metric_notifier
{
public:
  /// Gets the UL-SCH demultiplexer average initialization time in us.
  double get_average_init_time_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_init_ns.load(std::memory_order_relaxed)) /
                     static_cast<double>(count.load(std::memory_order_relaxed)) / 1000.0
               : 0;
  }

  /// Gets the UL-SCH demultiplexer average finalization time in us.
  double get_average_finish_time_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_on_end_codeword_ns.load(std::memory_order_relaxed)) /
                     static_cast<double>(count.load(std::memory_order_relaxed)) / 1000.0
               : 0;
  }

  /// Gets the UL-SCH demultiplexer average processing rate in Mbps.
  double get_average_rate_Mbps() const
  {
    return sum_elapsed_on_new_block_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_nof_bits.load(std::memory_order_relaxed)) /
                     static_cast<double>(sum_elapsed_on_new_block_ns.load(std::memory_order_relaxed)) * 1000.0
               : 0;
  }

  /// Gets the total time spent by the UL-SCH demultiplexer.
  std::chrono::nanoseconds get_total_time() const
  {
    return std::chrono::nanoseconds(sum_elapsed_init_ns.load(std::memory_order_relaxed) +
                                    sum_elapsed_on_new_block_ns.load(std::memory_order_relaxed) +
                                    sum_elapsed_on_end_codeword_ns.load(std::memory_order_relaxed));
  }

  /// Gets the CPU usage in microseconds of the UL-SCH demultiplexer.
  double get_cpu_usage_us() const { return sum_used_cpu_time_ns.load(std::memory_order_relaxed) / 1000; }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_elapsed_init_ns.store(0, std::memory_order_relaxed);
    sum_elapsed_on_new_block_ns.store(0, std::memory_order_relaxed);
    sum_elapsed_on_end_codeword_ns.store(0, std::memory_order_relaxed);
    sum_nof_bits.store(0, std::memory_order_relaxed);
    count.store(0, std::memory_order_relaxed);
    sum_used_cpu_time_ns.store(0, std::memory_order_relaxed);
  }

private:
  // See interface for documentation.
  void on_new_metric(const ulsch_demultiplex_metrics& metrics) override
  {
    sum_elapsed_init_ns.fetch_add(metrics.elapsed_init.count(), std::memory_order_relaxed);
    sum_elapsed_on_new_block_ns.fetch_add(metrics.elapsed_on_new_block.count(), std::memory_order_relaxed);
    sum_elapsed_on_end_codeword_ns.fetch_add(metrics.elapsed_on_end_codeword.count(), std::memory_order_relaxed);
    sum_nof_bits.fetch_add(metrics.sum_nof_bits, std::memory_order_relaxed);
    sum_used_cpu_time_ns.fetch_add(metrics.cpu_time_usage.count(), std::memory_order_relaxed);
    count.fetch_add(1, std::memory_order_relaxed);
  }

  std::atomic<uint64_t> sum_elapsed_init_ns            = {};
  std::atomic<uint64_t> sum_elapsed_on_new_block_ns    = {};
  std::atomic<uint64_t> sum_elapsed_on_end_codeword_ns = {};
  std::atomic<uint64_t> sum_nof_bits                   = {};
  std::atomic<uint64_t> count                          = {};
  std::atomic<uint64_t> sum_used_cpu_time_ns           = {};
};

} // namespace srsran
