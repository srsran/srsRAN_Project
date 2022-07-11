/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "adapters/f1ap_adapters.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/rrc/rrc_config.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {

namespace srs_cu_cp {

struct cu_cp_manager_config_t {
  std::string                 name        = "srs_cu_cp";
  uint8_t                     rrc_version = 2;
  srslog::basic_logger&       logger      = srslog::fetch_basic_logger("CU-CP");
  f1c_du_management_notifier* f1c_du_mgmt_notifier;
  f1c_message_notifier*       f1c_notifier;
  timer_manager*              timers;
  task_executor*              cu_cp_mng_exec;
  rrc_cfg_t                   rrc_cfg; // TODO: do we put subcomponent configs here?
};

} // namespace srs_cu_cp

} // namespace srsgnb
