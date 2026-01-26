/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "phy_fapi_sector_fastpath_adaptor_impl.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace fapi_adaptor;

phy_fapi_sector_fastpath_adaptor_impl::phy_fapi_sector_fastpath_adaptor_impl(
    std::unique_ptr<phy_fapi_p7_sector_fastpath_adaptor> p7_adaptor_) :
  p7_adaptor(std::move(p7_adaptor_))
{
  srsran_assert(p7_adaptor, "Invalid PHY-FAPI P7 sector adaptor");
}

phy_fapi_p7_sector_fastpath_adaptor& phy_fapi_sector_fastpath_adaptor_impl::get_p7_sector_adaptor()
{
  return *p7_adaptor;
}
