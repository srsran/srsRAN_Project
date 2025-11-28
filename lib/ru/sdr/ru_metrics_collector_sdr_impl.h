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

#include "lower_phy/lower_phy_sector.h"
#include "srsran/ru/ru_metrics_collector.h"
#include <vector>

namespace srsran {

class ru_radio_metrics_collector;

/// Metrics collector implementation for the SDR RU.
class ru_metrics_collector_sdr_impl : public ru_metrics_collector
{
  ru_radio_metrics_collector&                      radio;
  std::vector<lower_phy_sector_metrics_collector*> sector_metrics_collectors;

public:
  explicit ru_metrics_collector_sdr_impl(ru_radio_metrics_collector& radio_) : radio(radio_) {}

  /// Sets the list of lower PHY sector collectors.
  void set_lower_phy_sectors(std::vector<lower_phy_sector_metrics_collector*> collectors)
  {
    sector_metrics_collectors = std::move(collectors);
  }

  // See interface for documentation.
  void collect_metrics(ru_metrics& metrics) override;
};

} // namespace srsran
