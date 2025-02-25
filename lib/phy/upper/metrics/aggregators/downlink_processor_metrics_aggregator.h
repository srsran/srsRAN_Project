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
               ? static_cast<double>(sum_elapsed_finish_ns) / static_cast<double>(count) * 1e-3
               : 0;
  }

  /// Gets the minimum transmission latency in microseconds.
  double get_min_latency_us() const
  {
    uint64_t min_latency_ns = unpack_duration(packed_min_latency_ns);
    return (min_latency_ns == unpack_duration(default_packed_min_latency_ns))
               ? 0
               : static_cast<double>(min_latency_ns) / 1000.0;
  }

  /// Gets the maximum transmission latency in microseconds and the slot point in which was detected.
  std::pair<double, slot_point> get_max_latency_us() const
  {
    uint64_t   packed_max_latency = packed_max_latency_ns;
    uint64_t   max_latency_ns     = unpack_duration(packed_max_latency);
    slot_point slot               = unpack_slot(packed_max_latency);
    return {static_cast<double>(max_latency_ns) / 1000.0, slot};
  }

  /// Resets values of all internal counters.
  void reset()
  {
    count                    = 0;
    sum_elapsed_data_ns      = 0;
    sum_elapsed_configure_ns = 0;
    sum_elapsed_finish_ns    = 0;
    packed_min_latency_ns    = default_packed_min_latency_ns;
    packed_max_latency_ns    = default_packed_max_latency_ns;
  }

private:
  static constexpr uint64_t default_packed_min_latency_ns =
      pack_slot_and_duration({}, std::numeric_limits<uint32_t>::max());
  static constexpr uint64_t default_packed_max_latency_ns =
      pack_slot_and_duration({}, std::numeric_limits<uint32_t>::min());

  // See interface for documentation.
  void on_new_metric(const downlink_processor_metrics& metrics) override
  {
    sum_elapsed_data_ns += metrics.elapsed_data.count();
    sum_elapsed_configure_ns += metrics.elapsed_configure.count();
    sum_elapsed_finish_ns += metrics.elapsed_finish.count();
    update_slotmin(metrics.slot, metrics.elapsed_finish.count(), packed_min_latency_ns);
    update_slotmax(metrics.slot, metrics.elapsed_finish.count(), packed_max_latency_ns);
    ++count;
  }

  std::atomic<uint64_t> sum_elapsed_data_ns      = {};
  std::atomic<uint64_t> sum_elapsed_configure_ns = {};
  std::atomic<uint64_t> sum_elapsed_finish_ns    = {};
  std::atomic<uint64_t> count                    = {};
  std::atomic<uint64_t> packed_min_latency_ns    = default_packed_min_latency_ns;
  std::atomic<uint64_t> packed_max_latency_ns    = default_packed_max_latency_ns;
};

} // namespace srsran
