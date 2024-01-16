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

#include <string>

namespace srsran {

/// \brief E2AP configuration
struct e2ap_configuration {
  unsigned    gnb_id = 0;
  std::string ran_node_name;
  std::string plmn; /// Full PLMN as string (without possible filler digit) e.g. "00101"
  unsigned    max_setup_retries = 5;
  bool        e2sm_kpm_enabled  = false;
  bool        e2sm_rc_enabled   = false;
};

} // namespace srsran
