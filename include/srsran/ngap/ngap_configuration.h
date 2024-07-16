/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/gnb_id.h"
#include <chrono>
#include <map>
#include <string>

namespace srsran {

namespace srs_cu_cp {

/// \brief NGAP configuration
struct ngap_configuration {
  gnb_id_t             gnb_id{0, 22};
  std::string          ran_node_name;
  plmn_identity        plmn = plmn_identity::test_value();
  unsigned             tac;
  std::chrono::seconds pdu_session_setup_timeout; // timeout for pdu session setup in seconds
};

} // namespace srs_cu_cp

} // namespace srsran
