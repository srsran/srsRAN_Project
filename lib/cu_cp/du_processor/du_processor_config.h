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
#include "du_configuration_handler.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/f1ap/cu_cp/f1ap_configuration.h"

namespace srsran {
namespace srs_cu_cp {

struct du_processor_config_t {
  du_index_t                                du_index = du_index_t::invalid;
  const cu_cp_configuration&                cu_cp_cfg;
  srslog::basic_logger&                     logger         = srslog::fetch_basic_logger("CU-CP");
  du_connection_notifier*                   du_setup_notif = nullptr;
  std::unique_ptr<du_configuration_handler> du_cfg_hdlr;
};

} // namespace srs_cu_cp
} // namespace srsran
