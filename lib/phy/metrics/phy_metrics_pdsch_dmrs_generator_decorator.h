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
#include "srsran/phy/upper/signal_processors/pdsch/dmrs_pdsch_processor.h"
#include "srsran/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace srsran {

/// DM-RS for PDSCH generator metric decorator.
class phy_metrics_dmrs_pdsch_processor_decorator : public dmrs_pdsch_processor
{
public:
  /// Creates a DM-RS for PDSCH generator from a base instance and a metric notifier.
  phy_metrics_dmrs_pdsch_processor_decorator(std::unique_ptr<dmrs_pdsch_processor> base_,
                                             pdsch_dmrs_generator_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid encoder.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override
  {
    pdsch_dmrs_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base->map(grid, config);
    }
    notifier.on_new_metric(metrics);
  }

private:
  std::unique_ptr<dmrs_pdsch_processor> base;
  pdsch_dmrs_generator_metric_notifier& notifier;
};

} // namespace srsran
