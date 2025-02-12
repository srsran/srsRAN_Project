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
#include "srsran/phy/upper/signal_processors/port_channel_estimator.h"
#include "srsran/phy/upper/unique_rx_buffer.h"

namespace srsran {

/// Port channel estimator metric decorator.
class phy_metrics_port_channel_estimator_decorator : public port_channel_estimator
{
public:
  /// Creates port channel estimator decorator from a base instance and a metric notifier.
  phy_metrics_port_channel_estimator_decorator(std::unique_ptr<port_channel_estimator> base_,
                                               port_channel_estimator_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid base instance.");
  }

  // See interface for documentation.
  void compute(channel_estimate&           estimate,
               const resource_grid_reader& grid,
               unsigned                    port,
               const dmrs_symbol_list&     pilots,
               const configuration&        cfg) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();
    base->compute(estimate, grid, port, pilots, cfg);
    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.on_new_metric({.elapsed = tp_after - tp_before});
  }

private:
  std::unique_ptr<port_channel_estimator> base;
  port_channel_estimator_metric_notifier& notifier;
};

} // namespace srsran
