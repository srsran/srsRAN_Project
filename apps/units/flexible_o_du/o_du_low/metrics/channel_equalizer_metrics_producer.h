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
#include "srsran/ran/pusch/pusch_constants.h"
#include <atomic>

namespace srsran {

/// Channel equalizer metric producer.
class channel_equalizer_metric_producer_impl : private channel_equalizer_metric_notifier
{
public:
  /// Gets the channel equalizer metric notifier.
  channel_equalizer_metric_notifier& get_notifier() { return *this; }

  /// Gets the channel equalizer average processing rate in MREps (Millions of RE per second).
  double get_average_rate_MRes(unsigned nof_layers) const
  {
    if (nof_layers < 1) {
      return std::numeric_limits<double>::quiet_NaN();
    }
    const metrics_per_layer& temp = metrics_collection[nof_layers - 1];
    return static_cast<double>(temp.sum_nof_re_layer) / static_cast<double>(temp.sum_elapsed_ns) * 1000.0;
  }

  /// Gets the total time spent by the channel equalizer.
  std::chrono::nanoseconds get_total_time() const
  {
    uint64_t sum_ns = std::accumulate(metrics_collection.begin(),
                                      metrics_collection.end(),
                                      0UL,
                                      [](uint64_t sum, const metrics_per_layer& m) { return sum + m.sum_elapsed_ns; });
    return std::chrono::nanoseconds(sum_ns);
  }

private:
  // See interface for documentation.
  void new_metric(const channel_equalizer_metrics& metrics) override
  {
    if (metrics.nof_layers < 1) {
      return;
    }
    metrics_per_layer& temp = metrics_collection[metrics.nof_layers - 1];
    temp.sum_nof_re_layer += metrics.nof_re;
    temp.sum_elapsed_ns += metrics.elapsed.count();
    ++temp.count;
  }

  struct metrics_per_layer {
    std::atomic<uint64_t> sum_nof_re_layer = {};
    std::atomic<uint64_t> sum_elapsed_ns   = {};
    std::atomic<uint64_t> count            = {};
  };
  std::array<metrics_per_layer, pusch_constants::MAX_NOF_LAYERS> metrics_collection;
};

} // namespace srsran