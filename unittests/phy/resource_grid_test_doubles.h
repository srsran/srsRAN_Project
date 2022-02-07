#ifndef SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_
#define SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_

#include "srsgnb/phy/resource_grid.h"
#include <vector>

namespace srsgnb {

class resource_grid_spy : public resource_grid
{
public:
  struct entry_t {
    unsigned l;     // Symbol index
    unsigned k;     // Subcarrier index
    cf_t     value; // Complex value of the resource element
  };

  resource_grid_spy()           = default;
  ~resource_grid_spy() override = default;

  void put(unsigned int k, unsigned int l, cf_t value) override { entries.push_back({k, l, value}); }

  const std::vector<entry_t>& get_entries() const { return entries; }
  unsigned                    get_nof_entries() const { return entries.size(); }

private:
  std::vector<entry_t> entries;
};

} // namespace srsgnb
#endif // SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_
