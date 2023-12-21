/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "resource_grid_pool_generic_impl.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

resource_grid_pool_generic_impl::resource_grid_pool_generic_impl(std::vector<resource_grid_ptr> grids_) :
  grids(std::move(grids_))
{
  srsran_assert(std::all_of(grids.begin(), grids.end(), [](const auto& ptr) { return ptr != nullptr; }),
                "Detected invalid grid pointer.");
}

resource_grid& resource_grid_pool_generic_impl::get_resource_grid(const resource_grid_context& context)
{
  unsigned index = count;

  count = (count + 1) % grids.size();

  return *grids[index];
}
