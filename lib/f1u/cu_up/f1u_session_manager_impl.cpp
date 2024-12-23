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

f1u_session_manager_impl::f1u_session_manager_impl(const f1u_session_maps& f1u_sessions_) : f1u_sessions(f1u_sessions_)
{
  srsran_assert(not f1u_sessions.default_gw_sessions.empty(), "F1-U gateways cannot be empty");
}

gtpu_tnl_pdu_session& f1u_session_manager_impl::get_next_f1u_gateway()
{
  uint32_t index = next_gw % f1u_sessions.default_gw_sessions.size();
  next_gw++;
  return *f1u_sessions.default_gw_sessions[index];
}
