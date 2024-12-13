/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_fapi_adaptor_impl.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace fapi_adaptor;

phy_fapi_adaptor_impl::phy_fapi_adaptor_impl(std::vector<std::unique_ptr<phy_fapi_sector_adaptor>> sector_adaptors_) :
  sector_adaptors(std::move(sector_adaptors_))
{
  srsran_assert(!sector_adaptors.empty(), "Cannot create a PHY-FAPI adaptor with zero cells");
}

phy_fapi_sector_adaptor& phy_fapi_adaptor_impl::get_sector_adaptor(unsigned cell_id)
{
  srsran_assert(cell_id < sector_adaptors.size(),
                "Invalid cell identifier '{}'. Valid cell id range '[0-{})'",
                cell_id,
                sector_adaptors.size());

  return *sector_adaptors[cell_id];
}
