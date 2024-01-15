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
