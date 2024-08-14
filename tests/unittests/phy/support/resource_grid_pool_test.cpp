/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "resource_grid_test_doubles.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/support/srsran_test.h"
#include <vector>

using namespace srsran;

void test(unsigned nof_slots)
{
  // Create grids.
  std::vector<std::unique_ptr<resource_grid>> grid_pool;
  std::vector<resource_grid*>                 expected_grids;

  // Generate resource grids
  for (unsigned slot = 0; slot != nof_slots; ++slot) {
    grid_pool.emplace_back(std::make_unique<resource_grid_dummy>());
    expected_grids.emplace_back(grid_pool.back().get());
  }

  // Create resource grid pool
  std::unique_ptr<resource_grid_pool> pool = create_generic_resource_grid_pool(std::move(grid_pool));

  // Iterate all parameters and assert grid reference
  std::vector<shared_resource_grid> reserved_grids;
  for (unsigned slot = 0; slot != nof_slots; ++slot) {
    // Create context
    resource_grid_context context = {};
    context.slot                  = {0, slot};
    context.sector                = 0;

    // Get grid.
    shared_resource_grid grid = pool->allocate_resource_grid(context);
    TESTASSERT(grid.is_valid());

    // Verify grid reference match
    TESTASSERT_EQ(reinterpret_cast<const void*>(&grid.get()), reinterpret_cast<const void*>(expected_grids[slot]));

    // Move grid to the reserved list to avoid being released.
    reserved_grids.emplace_back(std::move(grid));
  }
}

int main()
{
  for (unsigned nof_slots : {40}) {
    test(nof_slots);
  }

  return 0;
}
