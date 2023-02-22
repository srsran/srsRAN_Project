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

#include "srsran/du_high/du_high_cell_executor_mapper.h"
#include "srsran/du_high/du_high_ue_executor_mapper.h"
#include "srsran/pcap/pcap.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/scheduler_metrics.h"

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
