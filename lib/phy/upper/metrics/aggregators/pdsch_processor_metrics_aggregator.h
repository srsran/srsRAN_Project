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

#include "aggregator_helpers.h"
#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/metrics/phy_metrics_reports.h"
#include <atomic>
#include <utility>

namespace srsran {

/// PDSCH processor metrics aggregator.
class pdsch_processor_metrics_aggregator : public pdsch_processor_metric_notifier
{
public:
  /// Gets the total number of processed codeblocks.
  uint64_t get_nof_processed_tbs() const { return count.load(std::memory_order_relaxed); }

  /// Gets the average transmission latency in microseconds.
  double get_avg_latency_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_completion_ns.load(std::memory_order_relaxed)) /
                     static_cast<double>(count.load(std::memory_order_relaxed)) * 1e-3
               : 0;
  }

  /// Gets the minimum transmission latency in microseconds.
  double get_min_latency_us() const
  {
    uint64_t packed_min_latency = packed_min_latency_ns.load(std::memory_order_relaxed);
    uint64_t min_latency_ns     = unpack_duration(packed_min_latency);
    return (min_latency_ns == std::numeric_limits<uint32_t>::max()) ? 0 : static_cast<double>(min_latency_ns) / 1000.0;
  }

  /// Gets the maximum transmission latency in microseconds and the slot point in which was detected.
  std::pair<double, slot_point> get_max_latency_us() const
  {
    uint64_t   packed_max_latency = packed_max_latency_ns.load(std::memory_order_relaxed);
    uint64_t   max_latency_ns     = unpack_duration(packed_max_latency);
    slot_point slot               = unpack_slot(packed_max_latency);
    return {static_cast<double>(max_latency_ns) / 1000.0, slot};
  }

  /// \brief Gets the average return time in microseconds.
  ///
  /// It is the average processing time of a single-thread PDSCH processor.
  double get_avg_return_time_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_return_ns.load(std::memory_order_relaxed)) /
                     static_cast<double>(count.load(std::memory_order_relaxed)) * 1e-3
               : 0;
  }

  /// Gets the processing rate.
  double get_process_rate_Mbps() const
  {
    return sum_elapsed_completion_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_tb_sz.load(std::memory_order_relaxed)) /
                     static_cast<double>(sum_elapsed_completion_ns.load(std::memory_order_relaxed)) * 1000
               : 0;
  }

  /// Gets the total amount of time the PDSCH processor spent processing.
  std::chrono::nanoseconds get_total_time() const
  {
    return std::chrono::nanoseconds(sum_elapsed_completion_ns.load(std::memory_order_relaxed));
  }

  /// Gets the CPU usage in microseconds of the PDSCH processor.
  double get_cpu_usage_us() const
  {
    return static_cast<double>(sum_used_cpu_time_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_tb_sz.store(0, std::memory_order_relaxed);
    sum_elapsed_return_ns.store(0, std::memory_order_relaxed);
    sum_elapsed_completion_ns.store(0, std::memory_order_relaxed);
    count.store(0, std::memory_order_relaxed);
    sum_used_cpu_time_ns.store(0, std::memory_order_relaxed);
    packed_min_latency_ns.store(default_packed_min_latency_ns, std::memory_order_relaxed);
    packed_max_latency_ns.store(default_packed_max_latency_ns, std::memory_order_relaxed);
  }

private:
  static constexpr uint64_t default_packed_min_latency_ns =
      pack_slot_and_duration({}, std::numeric_limits<uint32_t>::max());
  static constexpr uint64_t default_packed_max_latency_ns =
      pack_slot_and_duration({}, std::numeric_limits<uint32_t>::min());

  // See interface for documentation.
  void on_new_metric(const pdsch_processor_metrics& metrics) override
  {
    sum_tb_sz.fetch_add(metrics.tbs.to_bits().value(), std::memory_order_relaxed);
    sum_elapsed_return_ns.fetch_add(metrics.elapsed_return.count(), std::memory_order_relaxed);
    sum_elapsed_completion_ns.fetch_add(metrics.elapsed_completion.count(), std::memory_order_relaxed);
    update_slotmin(metrics.slot, metrics.elapsed_completion.count(), packed_min_latency_ns);
    update_slotmax(metrics.slot, metrics.elapsed_completion.count(), packed_max_latency_ns);
    count.fetch_add(1, std::memory_order_relaxed);
    sum_used_cpu_time_ns.fetch_add(metrics.self_cpu_time_usage.count(), std::memory_order_relaxed);
  }

  std::atomic<uint64_t> sum_tb_sz                 = {};
  std::atomic<uint64_t> sum_elapsed_return_ns     = {};
  std::atomic<uint64_t> sum_elapsed_completion_ns = {};
  std::atomic<uint64_t> count                     = {};
  std::atomic<uint64_t> sum_used_cpu_time_ns      = {};
  std::atomic<uint64_t> packed_min_latency_ns     = default_packed_min_latency_ns;
  std::atomic<uint64_t> packed_max_latency_ns     = default_packed_max_latency_ns;
};

} // namespace srsran
