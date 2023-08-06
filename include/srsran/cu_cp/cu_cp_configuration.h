/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ue_manager.h"
#include "srsran/cu_cp/cell_meas_manager_config.h"
#include "srsran/cu_cp/mobility_manager_config.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_configuration.h"
#include "srsran/rrc/rrc_config.h"
#include "srsran/support/executors/task_executor.h"
#include <map>

namespace srsran {
namespace srs_cu_cp {

struct mobility_configuration {
  cell_meas_manager_cfg meas_manager_config;
  mobility_manager_cfg  mobility_manager_config;
};

/// Configuration passed to CU-CP.
struct cu_cp_configuration {
  task_executor*         cu_cp_executor = nullptr;
  task_executor*         cu_cp_e2_exec  = nullptr;
  e1ap_message_notifier* e1ap_notifier  = nullptr; ///> Callback for incoming E1AP messages.
  ngap_message_notifier* ngap_notifier  = nullptr; ///> Callack for outgoing NGAP messages.
  ngap_configuration     ngap_config;
  rrc_cfg_t              rrc_config;
  ue_configuration       ue_config;
  mobility_configuration mobility_config;
};

} // namespace srs_cu_cp
} // namespace srsran
