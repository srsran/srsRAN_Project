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

/// Downlink processor metrics aggregator.
class downlink_processor_metrics_aggregator : public downlink_processor_metric_notifier
{
public:
  /// Gets the average transmission latency in microseconds.
  double get_avg_latency_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_finish_ns.load(std::memory_order_relaxed)) /
                     static_cast<double>(count.load(std::memory_order_relaxed)) * 1e-3
               : 0;
  }

  /// Gets the minimum transmission latency in microseconds.
  double get_min_latency_us() const
  {
    uint64_t min_latency_ns = unpack_duration(packed_min_latency_ns.load(std::memory_order_relaxed));
    return (min_latency_ns == unpack_duration(default_packed_min_latency_ns))
               ? 0
               : static_cast<double>(min_latency_ns) / 1000.0;
  }

  /// Gets the maximum transmission latency in microseconds and the slot point in which was detected.
  std::pair<double, slot_point> get_max_latency_us() const
  {
    uint64_t   packed_max_latency = packed_max_latency_ns.load(std::memory_order_relaxed);
    uint64_t   max_latency_ns     = unpack_duration(packed_max_latency);
    slot_point slot               = unpack_slot(packed_max_latency);
    return {static_cast<double>(max_latency_ns) / 1000.0, slot};
  }

  /// Resets values of all internal counters.
  void reset()
  {
    count.store(0, std::memory_order_relaxed);
    sum_elapsed_data_ns.store(0, std::memory_order_relaxed);
    sum_elapsed_configure_ns.store(0, std::memory_order_relaxed);
    sum_elapsed_finish_ns.store(0, std::memory_order_relaxed);
    packed_min_latency_ns.store(default_packed_min_latency_ns, std::memory_order_relaxed);
    packed_max_latency_ns.store(default_packed_max_latency_ns, std::memory_order_relaxed);
  }

private:
  static constexpr uint64_t default_packed_min_latency_ns =
      pack_slot_and_duration({}, std::numeric_limits<uint32_t>::max());
  static constexpr uint64_t default_packed_max_latency_ns =
      pack_slot_and_duration({}, std::numeric_limits<uint32_t>::min());

  // See interface for documentation.
  void on_new_metric(const downlink_processor_metrics& metrics) override
  {
    sum_elapsed_data_ns.fetch_add(metrics.elapsed_data.count(), std::memory_order_relaxed);
    sum_elapsed_configure_ns.fetch_add(metrics.elapsed_configure.count(), std::memory_order_relaxed);
    sum_elapsed_finish_ns.fetch_add(metrics.elapsed_finish.count(), std::memory_order_relaxed);
    update_slotmin(metrics.slot, metrics.elapsed_finish.count(), packed_min_latency_ns);
    update_slotmax(metrics.slot, metrics.elapsed_finish.count(), packed_max_latency_ns);
    count.fetch_add(1, std::memory_order_relaxed);
  }

  std::atomic<uint64_t> sum_elapsed_data_ns      = {};
  std::atomic<uint64_t> sum_elapsed_configure_ns = {};
  std::atomic<uint64_t> sum_elapsed_finish_ns    = {};
  std::atomic<uint64_t> count                    = {};
  std::atomic<uint64_t> packed_min_latency_ns    = default_packed_min_latency_ns;
  std::atomic<uint64_t> packed_max_latency_ns    = default_packed_max_latency_ns;
};

} // namespace srsran
