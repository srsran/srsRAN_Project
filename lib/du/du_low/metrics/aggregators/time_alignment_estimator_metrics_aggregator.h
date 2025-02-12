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

/// Time alignment estimator metrics aggregator.
class time_alignment_estimator_metrics_aggregator : public time_alignment_estimator_metric_notifier
{
private:
  // See interface for documentation.
  void on_new_metric(const time_alignment_estimator_metrics& metrics) override
  {
    // Implement me!
  }
};

} // namespace srsran
