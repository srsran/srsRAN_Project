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

#include "srsgnb/du_high/du_high_cell_executor_mapper.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include "srsgnb/pcap/pcap.h"
#include "srsgnb/scheduler/config/scheduler_expert_config.h"
#include "srsgnb/scheduler/scheduler_metrics.h"

namespace srsran {

struct mac_config {
  mac_pcap&                      pcap;
  mac_ul_ccch_notifier&          ul_ccch_notifier;
  du_high_ue_executor_mapper&    ul_exec_mapper;
  du_high_cell_executor_mapper&  dl_exec_mapper;
  task_executor&                 ctrl_exec;
  mac_result_notifier&           phy_notifier;
  scheduler_expert_config        sched_cfg;
  scheduler_ue_metrics_notifier& metric_notifier;
};

} // namespace srsran
