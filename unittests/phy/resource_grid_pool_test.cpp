
#include "resource_grid_test_doubles.h"
#include "srsgnb/phy/resource_grid_pool.h"
#include <cassert>
#include <vector>

using namespace srsgnb;

void test(unsigned nof_slots, unsigned nof_sectors)
{
  resource_grid_pool_config cfg = {};
  cfg.nof_slots                 = nof_slots;
  cfg.nof_sectors               = nof_sectors;

  // Create grids
  std::vector<std::vector<resource_grid*> > grids;

  // Generate resource grids
  cfg.grids.reserve(nof_slots * nof_sectors);
  grids.resize(nof_slots);
  for (unsigned slot = 0; slot != nof_slots; ++slot) {
    grids[slot].resize(nof_sectors);
    for (unsigned sector = 0; sector != nof_sectors; ++sector) {
      grids[slot][sector] = new resource_grid_dummy;
      cfg.grids.emplace_back(grids[slot][sector]);
    }
  }

  // Create resource grid pool
  std::unique_ptr<resource_grid_pool> pool = create_resource_grid_pool(cfg);

  // Iterate all parameters and assert grid reference
  for (unsigned slot = 0; slot != nof_slots; ++slot) {
    for (unsigned sector = 0; sector != nof_sectors; ++sector) {
      // Create context
      resource_grid_context context = {};
      context.slot.slot             = slot;
      context.sector                = sector;

      // Get grid
      const resource_grid& grid = pool->get_resource_grid(context);

      // Verify grid referemnce match
      assert(&grid == grids[slot][sector]);
    }
  }
}

int main()
{
  for (unsigned nof_slots : {40}) {
    for (unsigned nof_sectors : {1, 2, 3}) {
      test(nof_slots, nof_sectors);
    }
  }

  return 0;
}
