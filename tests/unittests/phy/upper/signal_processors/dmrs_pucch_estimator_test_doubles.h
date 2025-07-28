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

#include "../../phy_test_utils.h"
#include "srsran/phy/upper/signal_processors/dmrs_pucch_estimator.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"

namespace srsran {

class dmrs_pucch_estimator_spy : public dmrs_pucch_estimator
{
public:
  struct entry_format2_t {
    const channel_estimate*     estimate;
    const resource_grid_reader* grid;
    format2_configuration       config;
  };

  struct entry_format3_t {
    const channel_estimate*     estimate;
    const resource_grid_reader* grid;
    format3_configuration       config;
  };

  struct entry_format4_t {
    const channel_estimate*     estimate;
    const resource_grid_reader* grid;
    format4_configuration       config;
  };

  void
  estimate(channel_estimate& estimate, const resource_grid_reader& grid, const format2_configuration& config) override
  {
    format2_entries.emplace_back(entry_format2_t{&estimate, &grid, config});
  }

  void
  estimate(channel_estimate& estimate, const resource_grid_reader& grid, const format3_configuration& config) override
  {
    format3_entries.emplace_back(entry_format3_t{&estimate, &grid, config});
  }

  void
  estimate(channel_estimate& estimate, const resource_grid_reader& grid, const format4_configuration& config) override
  {
    format4_entries.emplace_back(entry_format4_t{&estimate, &grid, config});
  }

  span<const entry_format2_t> get_format2_entries() const { return format2_entries; }
  span<const entry_format3_t> get_format3_entries() const { return format3_entries; }
  span<const entry_format4_t> get_format4_entries() const { return format4_entries; }

  void clear()
  {
    format2_entries.clear();
    format3_entries.clear();
    format4_entries.clear();
  }

private:
  std::vector<entry_format2_t> format2_entries;
  std::vector<entry_format3_t> format3_entries;
  std::vector<entry_format4_t> format4_entries;
};

class dmrs_pucch_estimator_factory_spy : public dmrs_pucch_estimator_factory
{
public:
  std::unique_ptr<dmrs_pucch_estimator> create() override
  {
    std::unique_ptr<dmrs_pucch_estimator_spy> spy = std::make_unique<dmrs_pucch_estimator_spy>();
    entries.push_back(spy.get());
    return spy;
  }

  std::vector<dmrs_pucch_estimator_spy*>& get_entries() { return entries; }

private:
  std::vector<dmrs_pucch_estimator_spy*> entries;
};

} // namespace srsran
