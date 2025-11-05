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

#include "f1u_session_manager_impl.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

f1u_session_manager_impl::f1u_session_manager_impl(const f1u_session_maps& f1u_sessions_) :
  logger(srslog::fetch_basic_logger("CU-F1-U")), f1u_sessions(f1u_sessions_)
{
  srsran_assert(not f1u_sessions.default_gw_sessions.empty(), "F1-U gateways cannot be empty");
  for (const auto& [s_nssai, s_nssai_sessions] : f1u_sessions.session_maps) {
    for (const auto& [five_qi, sessions] : s_nssai_sessions) {
      srsran_assert(not sessions.empty(), "There are no GWs for configured 5QI. {}", five_qi);
      next_gw_map[s_nssai]          = {};
      next_gw_map[s_nssai][five_qi] = 0;
    }
  }
}

gtpu_tnl_pdu_session& f1u_session_manager_impl::get_next_f1u_gateway(s_nssai_t s_nssai, five_qi_t five_qi)
{
  // Try to select S-NSSAI/5QI specific session.
  auto s_nssai_it = f1u_sessions.session_maps.find(s_nssai);
  if (s_nssai_it != f1u_sessions.session_maps.end()) {
    auto five_qi_it = s_nssai_it->second.find(five_qi);
    if (five_qi_it != s_nssai_it->second.end()) {
      uint32_t index = next_gw_map[s_nssai][five_qi] % five_qi_it->second.size();
      next_gw_map[s_nssai][five_qi]++;
      logger.debug("Found F1-U gateway for DRB. {}, {}", s_nssai, five_qi);
      return *five_qi_it->second[index];
    }
  }

  // Could not find any S-NSSAI/5QI specific session, use default sessions.
  uint32_t index = next_gw % f1u_sessions.default_gw_sessions.size();
  next_gw++;
  logger.debug("Found F1-U default gateway for DRB. {}, {}", s_nssai, five_qi);
  return *f1u_sessions.default_gw_sessions[index];
}
