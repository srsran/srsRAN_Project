/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_processors/pdcch_modulator.h"
#include <vector>

namespace srsgnb {

class pdcch_modulator_spy : public pdcch_modulator
{
private:
  struct entry_t {
    config_t              config;
    std::vector<uint8_t>  bits;
    resource_grid_writer* grid_ptr;
  };
  std::vector<entry_t> entries;

public:
  void modulate(resource_grid_writer& grid, span<const uint8_t> data, const config_t& config) override
  {
    entry_t e = {};
    e.config  = config;
    e.bits.resize(data.size());
    e.grid_ptr = &grid;
    std::copy(data.begin(), data.end(), e.bits.begin());
    entries.emplace_back(e);
  }
  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};

PHY_SPY_FACTORY_TEMPLATE(pdcch_modulator);

} // namespace srsgnb
