
#pragma once

#include "srsgnb/phy/upper/channel_processors/pdsch_modulator.h"
#include <vector>

namespace srsran {

class pdsch_modulator_spy : public pdsch_modulator
{
private:
  struct entry_t {
    config_t                config;
    std::vector<bit_buffer> codewords;
    resource_grid_writer*   grid_ptr;
  };
  std::vector<entry_t> entries;

public:
  void modulate(resource_grid_writer& grid, span<const bit_buffer> codewords, const config_t& config) override
  {
    entry_t e = {};
    e.config  = config;
    for (unsigned cw = 0; cw != codewords.size(); ++cw) {
      e.codewords.emplace_back(codewords[cw]);
    }
    e.grid_ptr = &grid;
    entries.emplace_back(e);
  }
  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};

PHY_SPY_FACTORY_TEMPLATE(pdsch_modulator);

} // namespace srsran
