/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1u_session_manager_impl.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

f1u_session_manager_impl::f1u_session_manager_impl(const f1u_session_maps& f1u_sessions_) :
  logger(srslog::fetch_basic_logger("CU-F1-U")), f1u_sessions(f1u_sessions_)
{
  srsran_assert(not f1u_sessions.default_gw_sessions.empty(), "F1-U gateways cannot be empty");
  for (const auto& [five_qi, gws] : f1u_sessions.five_qi_gw_sessions) {
    srsran_assert(not gws.empty(), "There are no GWs for configured 5QI. {}", five_qi);
    five_qi_next_gw_map[five_qi] = 0;
  }
}

gtpu_tnl_pdu_session& f1u_session_manager_impl::get_next_f1u_gateway(five_qi_t five_qi)
{
  // Try to select 5QI specific session.
  auto it = f1u_sessions.five_qi_gw_sessions.find(five_qi);
  if (it != f1u_sessions.five_qi_gw_sessions.end()) {
    uint32_t index = five_qi_next_gw_map[five_qi] % it->second.size();
    five_qi_next_gw_map[five_qi]++;
    logger.debug("Found F1-U gateway for DRB. {}", five_qi);
    return *it->second[index];
  }

  // Could not find any 5QI specific session, use default sessions.
  uint32_t index = next_gw % f1u_sessions.default_gw_sessions.size();
  next_gw++;
  logger.debug("Found F1-U default gateway for DRB. {}", five_qi);
  return *f1u_sessions.default_gw_sessions[index];
}
