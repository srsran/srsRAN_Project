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

#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include <vector>

namespace srsran {

class pbch_modulator_spy : public pbch_modulator
{
private:
  struct entry_t {
    config_t              config;
    std::vector<uint8_t>  bits;
    resource_grid_writer* grid_ptr;
  };
  std::vector<entry_t> entries;

public:
  void put(span<const uint8_t> bits, resource_grid_writer& grid, const config_t& config) override
  {
    entry_t e = {};
    e.config  = config;
    e.bits.resize(bits.size());
    e.grid_ptr = &grid;
    std::copy(bits.begin(), bits.end(), e.bits.begin());
    entries.emplace_back(e);
  }
  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};

PHY_SPY_FACTORY_TEMPLATE(pbch_modulator);

} // namespace srsran
