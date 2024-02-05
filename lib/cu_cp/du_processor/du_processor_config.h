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

#include "../cu_cp_controller/node_connection_notifier.h"
#include "srsran/rrc/rrc_config.h"

namespace srsran {
namespace srs_cu_cp {

struct du_processor_config_t {
  std::string             name           = "srs_cu_cp";
  du_index_t              du_index       = du_index_t::invalid;
  uint8_t                 rrc_version    = 2;
  srslog::basic_logger&   logger         = srslog::fetch_basic_logger("CU-CP");
  du_connection_notifier* du_setup_notif = nullptr;
  rrc_cfg_t               rrc_cfg;                     // TODO: do we put subcomponent configs here?
  security_indication_t   default_security_indication; // default if not signaled via NGAP
};

} // namespace srs_cu_cp

} // namespace srsran
