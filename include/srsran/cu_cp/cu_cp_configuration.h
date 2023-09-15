/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ue_manager.h"
#include "srsran/cu_cp/cell_meas_manager_config.h"
#include "srsran/cu_cp/mobility_manager_config.h"
#include "srsran/e1ap/common/e1ap_common.h"
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
  timer_manager*         timers         = nullptr;
  ngap_configuration     ngap_config;
  rrc_cfg_t              rrc_config;
  ue_configuration       ue_config;
  mobility_configuration mobility_config;
  security_indication_t  default_security_indication; // default if not signaled via NGAP
};

} // namespace srs_cu_cp
} // namespace srsran
