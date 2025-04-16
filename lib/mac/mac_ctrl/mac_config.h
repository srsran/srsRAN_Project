/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    std::chrono::milliseconds period{1000};
    mac_metrics_notifier&     notifier;
  };

  mac_ul_ccch_notifier&                 event_notifier;
  srs_du::du_high_ue_executor_mapper&   ue_exec_mapper;
  srs_du::du_high_cell_executor_mapper& cell_exec_mapper;
  task_executor&                        ctrl_exec;
  timer_manager&                        timers;
  std::optional<metrics_config>         metrics;
  srslog::basic_logger&                 logger = srslog::fetch_basic_logger("MAC", true);
};

} // namespace srsran
