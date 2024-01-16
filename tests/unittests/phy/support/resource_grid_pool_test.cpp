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

#include "resource_grid_test_doubles.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/support/srsran_test.h"
#include <vector>

using namespace srsran;

void test(unsigned nof_slots, unsigned nof_sectors)
{
  // Create grids
  std::vector<std::unique_ptr<resource_grid>> grid_pool;
  std::vector<std::vector<resource_grid*>>    grids;

  // Generate resource grids
  grids.reserve(nof_slots * nof_sectors);
  grids.resize(nof_slots);
  for (unsigned slot = 0; slot != nof_slots; ++slot) {
    grids[slot].resize(nof_sectors);
    for (unsigned sector = 0; sector != nof_sectors; ++sector) {
      grids[slot][sector] = new resource_grid_dummy;
      grid_pool.emplace_back(grids[slot][sector]);
    }
  }

  // Create resource grid pool
  std::unique_ptr<resource_grid_pool> pool = create_generic_resource_grid_pool(std::move(grid_pool));

  // Iterate all parameters and assert grid reference
  for (unsigned slot_count = 0; slot_count != nof_slots; ++slot_count) {
    for (unsigned sector = 0; sector != nof_sectors; ++sector) {
      // Create context
      resource_grid_context context = {};
      context.slot                  = {0, slot_count};
      context.sector                = sector;

      // Get grid
      const resource_grid& grid = pool->get_resource_grid(context);

      // Verify grid reference match
      TESTASSERT_EQ((void*)&grid, (void*)grids[slot_count][sector]);
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
