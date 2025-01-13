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

#include "mac_fapi_adaptor_impl.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace fapi_adaptor;

mac_fapi_adaptor_impl::mac_fapi_adaptor_impl(std::vector<std::unique_ptr<mac_fapi_sector_adaptor>> sector_adaptors_) :
  sector_adaptors(std::move(sector_adaptors_))
{
  srsran_assert(!sector_adaptors.empty(), "Cannot create a MAC FAPI adaptor with zero cells");
}

mac_fapi_sector_adaptor& mac_fapi_adaptor_impl::get_sector_adaptor(unsigned cell_id)
{
  srsran_assert(cell_id < sector_adaptors.size(),
                "Invalid cell identifier '{}'. Valid cell id range '[0-{})'",
                cell_id,
                sector_adaptors.size());

  return *sector_adaptors[cell_id];
}
