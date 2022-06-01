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

#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {

struct cu_cp_manager_config_t {
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-CP-MNG");
  task_executor*        cu_cp_mng_exec;
};

} // namespace srsgnb

#endif // SRSGNB_CU_CP_MANAGER_CONFIG_H
