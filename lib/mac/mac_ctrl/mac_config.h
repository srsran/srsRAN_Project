/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/span.h"
#include "srsran/du/du_high/du_high_executor_mapper.h"
#include "srsran/du/du_high/du_test_mode_config.h"
#include "srsran/mac/mac.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

class mac_metrics_notifier;
class timer_manager;

/// Config for MAC controller.
struct mac_control_config {
  struct metrics_config {
    std::chrono::milliseconds   period{1000};
    mac_metrics_notifier&       mac_notifier;
    scheduler_metrics_notifier* sched_notifier = nullptr;
    unsigned                    max_nof_ue_events;
  };

  mac_ul_ccch_notifier&                 event_notifier;
  srs_du::du_high_ue_executor_mapper&   ue_exec_mapper;
  srs_du::du_high_cell_executor_mapper& cell_exec_mapper;
  task_executor&                        ctrl_exec;
  task_executor&                        timer_exec;
  timer_manager&                        timers;
  metrics_config                        metrics;
  srslog::basic_logger&                 logger = srslog::fetch_basic_logger("MAC", true);
};

} // namespace srsran
