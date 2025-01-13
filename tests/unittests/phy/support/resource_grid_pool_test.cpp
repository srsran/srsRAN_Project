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
    // Get grid.
    shared_resource_grid grid = pool->allocate_resource_grid({0, slot});
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
