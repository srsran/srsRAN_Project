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

#include "srsran/du/du_low/du_low_metrics_collector.h"
#include "srsran/phy/upper/upper_phy_metrics_collector.h"

namespace srsran {
namespace srs_du {

class du_low_metrics_collector_impl : public du_low_metrics_collector
{
  upper_phy_metrics_collector* upper_phy_collector = nullptr;

public:
  du_low_metrics_collector_impl() = default;

  explicit du_low_metrics_collector_impl(upper_phy_metrics_collector* upper_phy_collector_) :
    upper_phy_collector(upper_phy_collector_)
  {
  }

  // See interface for documentation.
  void collect_metrics(du_low_metrics& metrics) override;

  /// Returns true if metrics are disabled, otherwise false.
  bool disabled() const { return upper_phy_collector == nullptr; }

  /// Returns true if metrics are enabled, otherwise false.
  bool enabled() const { return !disabled(); }
};

} // namespace srs_du
} // namespace srsran
