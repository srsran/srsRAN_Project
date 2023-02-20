/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../phy_test_utils.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"

namespace srsgnb {

class dmrs_pusch_estimator_spy : public dmrs_pusch_estimator
{
public:
  struct entry_t {
    channel_estimate*           estimate;
    const resource_grid_reader* grid;
    configuration               config;
  };

  void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const configuration& config) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.estimate = &estimate;
    entry.grid     = &grid;
    entry.config   = config;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<entry_t> entries;
};

PHY_SPY_FACTORY_TEMPLATE(dmrs_pusch_estimator);

} // namespace srsgnb