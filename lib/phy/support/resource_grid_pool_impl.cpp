/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
