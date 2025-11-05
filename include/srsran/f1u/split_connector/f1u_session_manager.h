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

#include "srsran/gtpu/gtpu_gateway.h"
#include "srsran/ran/qos/five_qi.h"
#include "srsran/ran/s_nssai.h"
#include <map>

namespace srsran {

using f1u_session_list        = std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>;
using f1u_five_qi_session_map = std::map<five_qi_t, f1u_session_list>;
using f1u_s_nssai_session_map = std::map<s_nssai_t, f1u_five_qi_session_map>;

struct f1u_session_maps {
  f1u_session_list        default_gw_sessions;
  f1u_s_nssai_session_map session_maps;
};

class f1u_session_manager
{
public:
  virtual ~f1u_session_manager()                                                           = default;
  virtual gtpu_tnl_pdu_session& get_next_f1u_gateway(s_nssai_t s_nssai, five_qi_t five_qi) = 0;
};

} // namespace srsran
