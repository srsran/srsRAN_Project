/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1u_session_manager_impl.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_cu_up;

f1u_session_manager_impl::f1u_session_manager_impl(
    const std::vector<std::unique_ptr<f1u_cu_up_udp_gateway>>& f1u_gws_) :
  f1u_gws(f1u_gws_)
{
  srsran_assert(not f1u_gws.empty(), "F1-U gateways cannot be empty");
}

f1u_cu_up_udp_gateway& f1u_session_manager_impl::get_next_f1u_gateway()
{
  uint32_t index = next_gw % f1u_gws.size();
  next_gw++;
  return *f1u_gws[index];
}
