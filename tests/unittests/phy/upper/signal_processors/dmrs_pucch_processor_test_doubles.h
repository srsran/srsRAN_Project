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
#include "srsran/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"

namespace srsran {

class dmrs_pucch_processor_spy : public dmrs_pucch_processor
{
public:
  struct entry_t {
    const channel_estimate*     estimate;
    const resource_grid_reader* grid;
    config_t                    config;
  };

  void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const config_t& config) override
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

class dmrs_pucch_estimator_factory_spy : public dmrs_pucch_estimator_factory
{
public:
  std::unique_ptr<dmrs_pucch_processor> create_format1() override
  {
    std::unique_ptr<dmrs_pucch_processor_spy> spy = std::make_unique<dmrs_pucch_processor_spy>();
    format1_entries.push_back(spy.get());
    return spy;
  }

  std::unique_ptr<dmrs_pucch_processor> create_format2() override
  {
    std::unique_ptr<dmrs_pucch_processor_spy> spy = std::make_unique<dmrs_pucch_processor_spy>();
    format2_entries.push_back(spy.get());
    return spy;
  }

  std::vector<dmrs_pucch_processor_spy*>& get_format1_entries() { return format1_entries; }
  std::vector<dmrs_pucch_processor_spy*>& get_format2_entries() { return format2_entries; }

private:
  std::vector<dmrs_pucch_processor_spy*> format1_entries;
  std::vector<dmrs_pucch_processor_spy*> format2_entries;
};

} // namespace srsran