//
// Copyright 2021-2025 Software Radio Systems Limited
//
// By using this file, you agree to the terms and conditions set
// forth in the LICENSE file which can be found at the top level of
// the distribution.
//

#pragma once

namespace srsran {

struct upper_phy_metrics;

/// Upper PHY metrics collector.
class upper_phy_metrics_collector
{
public:
  virtual ~upper_phy_metrics_collector() = default;

  /// Collects the metrics of this upper PHY.
  virtual void collect_metrics(upper_phy_metrics& metrics) = 0;
};

} // namespace srsran
