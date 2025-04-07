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

#include "lower_phy/lower_phy_sector.h"
#include "srsran/ru/ru_metrics_collector.h"
#include <vector>

namespace srsran {

class ru_radio_metrics_collector;

/// Metrics collector implementation for the RU generic.
class ru_metrics_collector_generic_impl : public ru_metrics_collector
{
  ru_radio_metrics_collector&    radio;
  std::vector<lower_phy_sector*> sectors;

public:
  explicit ru_metrics_collector_generic_impl(ru_radio_metrics_collector& radio_) : radio(radio_) {}

  /// Sets the lower phy sector of this collector.
  void set_lower_phy_sectors(std::vector<lower_phy_sector*> lower_sectors) { sectors = std::move(lower_sectors); }

  // See interface for documentation.
  void collect_metrics(ru_metrics& metrics) override;
};

} // namespace srsran
