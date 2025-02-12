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
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/math/math_utils.h"
#include <atomic>

namespace srsran {

/// Pseudo-random sequence generator metrics aggregator.
class scrambling_metrics_aggregator : public pseudo_random_sequence_generator_metric_notifier
{
private:
  // See interface for documentation.
  void on_new_metric(const pseudo_random_sequence_generator_metrics& metrics) override
  {
    // Implement me!
  }
};

} // namespace srsran
