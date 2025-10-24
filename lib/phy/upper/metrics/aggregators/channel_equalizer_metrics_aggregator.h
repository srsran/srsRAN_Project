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
#include "srsran/ran/pusch/pusch_constants.h"
#include <atomic>

namespace srsran {

/// Channel equalizer metrics aggregator.
class channel_equalizer_metrics_aggregator : public channel_equalizer_metric_notifier
{
public:
  /// Gets the channel equalizer average processing rate in MREps (Millions of RE per second).
  double get_average_rate_MRes(unsigned nof_layers) const
  {
    if (nof_layers < 1) {
      return std::numeric_limits<double>::quiet_NaN();
    }
    const metrics_per_layer& temp = metrics_collection[nof_layers - 1];
    return static_cast<double>(temp.sum_nof_re_layer.load(std::memory_order_relaxed)) /
           static_cast<double>(temp.sum_elapsed_ns.load(std::memory_order_relaxed)) * 1000.0;
  }

  /// Gets the total time spent by the channel equalizer.
  std::chrono::nanoseconds get_total_time() const
  {
    uint64_t sum_ns = std::accumulate(
        metrics_collection.begin(), metrics_collection.end(), 0UL, [](uint64_t sum, const metrics_per_layer& m) {
          return sum + m.sum_elapsed_ns.load(std::memory_order_relaxed);
        });
    return std::chrono::nanoseconds(sum_ns);
  }

  /// Gets the CPU usage in microseconds of the channel equalizer.
  double get_cpu_usage_us() const
  {
    uint64_t sum = 0;
    for (const auto& m : metrics_collection) {
      sum += m.sum_elapsed_ns.load(std::memory_order_relaxed);
    }
    return static_cast<double>(sum) / 1000.0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    for (auto& metrics : metrics_collection) {
      metrics.reset();
    }
  }

private:
  // See interface for documentation.
  void on_new_metric(const channel_equalizer_metrics& metrics) override
  {
    if (metrics.nof_layers < 1) {
      return;
    }
    metrics_per_layer& temp = metrics_collection[metrics.nof_layers - 1];
    temp.sum_nof_re_layer.fetch_add(metrics.nof_re, std::memory_order_relaxed);
    temp.sum_elapsed_ns.fetch_add(metrics.measurements.duration.count(), std::memory_order_relaxed);
  }

  struct metrics_per_layer {
    std::atomic<uint64_t> sum_nof_re_layer = {};
    std::atomic<uint64_t> sum_elapsed_ns   = {};

    void reset()
    {
      sum_nof_re_layer.store(0, std::memory_order_relaxed);
      sum_elapsed_ns.store(0, std::memory_order_relaxed);
    }
  };

  std::array<metrics_per_layer, pusch_constants::MAX_NOF_LAYERS> metrics_collection;
};

} // namespace srsran
