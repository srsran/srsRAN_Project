
#ifndef UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_MODULATOR_TEST_DOUBLES_H
#define UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_MODULATOR_TEST_DOUBLES_H

#include "srsgnb/phy/upper/channel_processors/pdsch_modulator.h"
#include <vector>

namespace srsgnb {

class pdsch_modulator_spy : public pdsch_modulator
{
private:
  struct entry_t {
    config_t                           config;
    std::vector<std::vector<uint8_t> > codewords;
    resource_grid_writer*              grid_ptr;
  };
  std::vector<entry_t> entries;

public:
  void modulate(resource_grid_writer& grid, span<const span<const uint8_t> > codewords, const config_t& config) override
  {
    entry_t e = {};
    e.config  = config;
    e.codewords.resize(codewords.size());
    for (unsigned cw = 0; cw != codewords.size(); ++cw) {
      e.codewords[cw].resize(codewords[cw].size());
      srsvec::copy(e.codewords[cw], codewords[cw]);
    }
    e.grid_ptr = &grid;
    entries.emplace_back(e);
  }
  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};
} // namespace srsgnb
#endif // UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_MODULATOR_TEST_DOUBLES_H
