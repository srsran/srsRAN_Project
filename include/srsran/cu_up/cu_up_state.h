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

#include "srsran/cu_up/cu_up_types.h"
#include <map>
#include <set>

namespace srsran::srs_cu_up {

/// Useful typedef's to get the current state of the CU-UP.
using drb_state_t         = std::map<drb_id_t, std::set<qos_flow_id_t>>;
using pdu_session_state_t = std::map<pdu_session_id_t, drb_state_t>;
using up_state_t          = std::map<ue_index_t, pdu_session_state_t>;

} // namespace srsran::srs_cu_up
