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
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/support/math/math_utils.h"
#include <atomic>

namespace srsran {

/// Channel precoder metrics aggregator.
class channel_precoder_metrics_aggregator : public channel_precoder_metric_notifier
{
public:
  /// Get the channel precoder average processing rate in MREps (Millions of RE per second).
  double get_average_rate_MRes(unsigned nof_layers) const
  {
    if (nof_layers < 1) {
      return std::numeric_limits<double>::quiet_NaN();
    }
    const metrics_per_layer& temp = metrics_collection[nof_layers - 1];
    return temp.sum_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(temp.sum_nof_re_layer.load(std::memory_order_relaxed)) /
                     static_cast<double>(temp.sum_elapsed_ns.load(std::memory_order_relaxed)) * 1000.0
               : 0;
  }

  /// Gets the total time spent by the channel precoder processing given number of layers.
  std::chrono::nanoseconds get_total_time(unsigned nof_layers) const
  {
    if (nof_layers < 1) {
      return {};
    }
    const metrics_per_layer& temp = metrics_collection[nof_layers - 1];
    return std::chrono::nanoseconds(temp.sum_elapsed_ns.load(std::memory_order_relaxed));
  }

  /// Gets the total time spent by the channel precoder.
  uint64_t get_total_time_ns() const { return total_elapsed_ns.load(std::memory_order_relaxed); }

  /// Gets the CPU usage in microseconds of the layer mapper and precoder.
  double get_cpu_usage_us() const
  {
    return static_cast<double>(total_elapsed_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    for (auto& metrics : metrics_collection) {
      metrics.reset();
    }
    total_elapsed_ns.store(0, std::memory_order_relaxed);
  }

private:
  // See interface for documentation.
  void on_new_metric(const channel_precoder_metrics& metrics) override
  {
    if (metrics.nof_layers < 1) {
      return;
    }
    metrics_per_layer& temp = metrics_collection[metrics.nof_layers - 1];
    temp.sum_nof_re_layer.fetch_add(metrics.nof_re, std::memory_order_relaxed);
    temp.sum_elapsed_ns.fetch_add(metrics.measurements.duration.count(), std::memory_order_relaxed);
    total_elapsed_ns.fetch_add(metrics.measurements.duration.count(), std::memory_order_relaxed);
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

  std::array<metrics_per_layer, pdsch_constants::MAX_NOF_LAYERS> metrics_collection;
  std::atomic<uint64_t>                                          total_elapsed_ns = {};
};

} // namespace srsran
