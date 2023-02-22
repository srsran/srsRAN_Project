/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "resource_grid_pool_impl.h"

using namespace srsran;

resource_grid_pool_impl::resource_grid_pool_impl(unsigned                                      nof_sectors_,
                                                 unsigned                                      nof_slots_,
                                                 std::vector<std::unique_ptr<resource_grid>>&& grids_) :
  nof_slots(nof_slots_), nof_sectors(nof_sectors_), grids(std::move(grids_))
{
  assert(nof_slots_ * nof_sectors_ == grids.size());
}

resource_grid& resource_grid_pool_impl::get_resource_grid(const resource_grid_context& context)
{
  assert(context.sector < nof_sectors);

  // Select slot from the absolute system slot
  unsigned slot = context.slot.system_slot() % nof_slots;

  // Select resource grid and return the reference
  return *grids[slot * nof_sectors + context.sector];
}
