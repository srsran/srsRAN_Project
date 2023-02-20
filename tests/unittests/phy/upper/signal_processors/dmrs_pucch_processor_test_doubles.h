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