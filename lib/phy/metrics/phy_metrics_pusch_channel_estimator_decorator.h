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
#include "srsran/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include <memory>

namespace srsran {

/// PUSCH channel estimator metric decorator.
class phy_metrics_pusch_channel_estimator_decorator : public dmrs_pusch_estimator
{
public:
  /// Creates an PUSCH channel estimator from a base estimator instance and a metric notifier.
  phy_metrics_pusch_channel_estimator_decorator(std::unique_ptr<dmrs_pusch_estimator>    base_,
                                                pusch_channel_estimator_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid estimator.");
  }

  // See interface for documentation.
  void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const configuration& config) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();
    base->estimate(estimate, grid, config);
    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.on_new_metric({.nof_prb = static_cast<unsigned>(config.rb_mask.count()), .elapsed = tp_after - tp_before});
  }

private:
  std::unique_ptr<dmrs_pusch_estimator>    base;
  pusch_channel_estimator_metric_notifier& notifier;
};

} // namespace srsran
