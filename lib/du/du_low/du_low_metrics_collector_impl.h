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

#include "srsran/du/du_low/du_low_metrics_collector.h"
#include "srsran/phy/upper/upper_phy_metrics_collector.h"
#include <vector>

namespace srsran {
namespace srs_du {

class du_low_metrics_collector_impl : public du_low_metrics_collector
{
  std::vector<upper_phy_metrics_collector*> upper_phy_collectors;

public:
  explicit du_low_metrics_collector_impl(std::vector<upper_phy_metrics_collector*> upper_phy_collectors_) :
    upper_phy_collectors(std::move(upper_phy_collectors_))
  {
  }

  // See interface for documentation.
  void collect_metrics(du_low_metrics& metrics) override;

  /// Returns true if metrics are disabled, otherwise false.
  bool disabled() const { return upper_phy_collectors.empty(); }
  /// Returns true if metrics are enabled, otherwise false.
  bool enabled() const { return !disabled(); }
};

} // namespace srs_du
} // namespace srsran
