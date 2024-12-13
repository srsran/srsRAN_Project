/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
