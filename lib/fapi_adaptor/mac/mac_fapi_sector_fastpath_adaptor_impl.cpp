/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_fapi_sector_fastpath_adaptor_impl.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace fapi_adaptor;

mac_fapi_sector_fastpath_adaptor_impl::mac_fapi_sector_fastpath_adaptor_impl(
    std::unique_ptr<mac_fapi_p7_sector_fastpath_adaptor> p7_adaptor_) :
  p7_adaptor(std::move(p7_adaptor_))
{
  srsran_assert(p7_adaptor, "Invalid MAC-FAPI P7 sector adaptor");
}

mac_fapi_p7_sector_fastpath_adaptor& mac_fapi_sector_fastpath_adaptor_impl::get_p7_sector_adaptor()
{
  return *p7_adaptor;
}
