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

/// Metrics collector implementation for the RU generic.
class ru_metrics_collector_generic_impl : public ru_metrics_collector
{
  std::vector<lower_phy_sector*> sectors;

public:
  explicit ru_metrics_collector_generic_impl(std::vector<lower_phy_sector*> sectors_) : sectors(sectors_) {}

  // See interface for documentation.
  void collect_metrics(ru_metrics& metrics) override;
};

} // namespace srsran
