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

#include "du_low_impl.h"

using namespace srsran;

du_low_impl::du_low_impl(std::vector<std::unique_ptr<upper_phy>> upper_) : upper(std::move(upper_))
{
  srsran_assert(!upper.empty(), "Invalid upper PHY");
  for (auto& up : upper)
    upper_ptrs.push_back(up.get());
}

upper_phy& du_low_impl::get_upper_phy(unsigned cell_id)
{
  srsran_assert(cell_id < upper.size(), "Invalid cell index '{}'. Valid cell indexes [0-{})", cell_id, upper.size());

  return *upper[cell_id];
}

void du_low_impl::stop()
{
  for (auto& cell : upper) {
    cell->stop();
  }
}
span<upper_phy*> du_low_impl::get_all_upper_phys()
{
  return upper_ptrs;
}
