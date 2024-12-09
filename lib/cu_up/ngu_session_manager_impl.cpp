/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngu_session_manager_impl.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_cu_up;

ngu_session_manager_impl::ngu_session_manager_impl(const std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>& ngu_gws_) :
  ngu_gws(ngu_gws_)
{
  srsran_assert(not ngu_gws.empty(), "NG-U gateways cannot be empty");
}

gtpu_tnl_pdu_session& ngu_session_manager_impl::get_next_ngu_gateway()
{
  uint32_t index = next_gw % ngu_gws.size();
  next_gw++;
  return *ngu_gws[index];
}
