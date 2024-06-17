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

#include "srsran/ran/gnb_du_id.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

/// Context of the DU that has setup an F1 connection to the CU-CP.
struct f1ap_du_context {
  gnb_du_id_t gnb_du_id = gnb_du_id_t::invalid;
  std::string gnb_du_name;
  uint8_t     gnb_du_rrc_version;
};

} // namespace srs_cu_cp
} // namespace srsran