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

#include "srsgnb/du_high/du_high_cell_executor_mapper.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include "srsgnb/scheduler/config/scheduler_config.h"

namespace srsgnb {

struct mac_config {
  mac_ul_ccch_notifier&         ul_ccch_notifier;
  du_high_ue_executor_mapper&   ul_exec_mapper;
  du_high_cell_executor_mapper& dl_exec_mapper;
  task_executor&                ctrl_exec;
  mac_result_notifier&          phy_notifier;
  scheduler_config              sched_cfg;
};

} // namespace srsgnb