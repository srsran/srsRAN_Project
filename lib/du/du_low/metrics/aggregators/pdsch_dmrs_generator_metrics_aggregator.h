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
#include <atomic>

namespace srsran {

/// PDSCH DM-RS generator metrics aggregator.
class pdsch_dmrs_generator_metrics_aggregator : public pdsch_dmrs_generator_metric_notifier
{
private:
  // See interface for documentation.
  void on_new_metric(const pdsch_dmrs_generator_metrics& metrics) override
  {
    // Implement me!
  }
};

} // namespace srsran
