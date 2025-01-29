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

#pragma once

#include "srsran/f1u/split_connector/f1u_session_manager.h"

namespace srsran {

class f1u_session_manager_impl : public f1u_session_manager
{
public:
  ~f1u_session_manager_impl() override = default;
  explicit f1u_session_manager_impl(const f1u_session_maps& f1u_sessions_);

  gtpu_tnl_pdu_session& get_next_f1u_gateway(five_qi_t five_qi) override;

private:
  srslog::basic_logger&         logger;
  const f1u_session_maps&       f1u_sessions;
  uint32_t                      next_gw = 0;
  std::map<five_qi_t, uint32_t> five_qi_next_gw_map;
};

} // namespace srsran
