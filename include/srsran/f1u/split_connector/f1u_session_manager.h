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
#include <map>

namespace srsran {

struct f1u_session_maps {
  std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>                      default_gw_sessions;
  std::map<five_qi_t, std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>> five_qi_gw_sessions;
};

class f1u_session_manager
{
public:
  virtual ~f1u_session_manager()                                        = default;
  virtual gtpu_tnl_pdu_session& get_next_f1u_gateway(five_qi_t five_qi) = 0;
};

} // namespace srsran
