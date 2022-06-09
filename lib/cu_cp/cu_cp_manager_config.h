/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_MANAGER_CONFIG_H
#define SRSGNB_CU_CP_MANAGER_CONFIG_H

#include "srsgnb/f1_interface/f1ap_cu.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {

namespace srs_cu_cp {

struct cu_cp_manager_config_t {
  std::string              name        = "srs_cu_cp";
  uint8_t                  rrc_version = 2;
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-CP-MNG");
  f1ap_connection_manager* f1ap_conn_mng;
  timer_manager*        timers;
  task_executor*        cu_cp_mng_exec;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_MANAGER_CONFIG_H
