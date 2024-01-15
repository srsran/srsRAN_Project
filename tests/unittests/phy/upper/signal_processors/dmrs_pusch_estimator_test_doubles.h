/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../../phy_test_utils.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"

namespace srsran {

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

} // namespace srsran