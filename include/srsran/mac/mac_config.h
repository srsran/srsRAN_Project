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

#include "srsran/du/du_test_config.h"
#include "srsran/du_high/du_high_cell_executor_mapper.h"
#include "srsran/du_high/du_high_ue_executor_mapper.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/mac/mac_pdu_handler.h"
#include "srsran/pcap/pcap.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {

/// \brief Implementation-specific parameters used to tune MAC operation.
struct mac_expert_config {
  /// \brief Maximum number of consecutive DL KOs before an RLF is reported.
  unsigned max_consecutive_dl_kos = 10000;
  /// \brief Maximum number of consecutive DL KOs before an RLF is reported.
  unsigned max_consecutive_ul_kos = 10000;
};

/// \brief Configuration passed to MAC during its instantiation.
struct mac_config {
  mac_ul_ccch_notifier&         ul_ccch_notifier;
  du_high_ue_executor_mapper&   ue_exec_mapper;
  du_high_cell_executor_mapper& cell_exec_mapper;
  task_executor&                ctrl_exec;
  mac_result_notifier&          phy_notifier;
  mac_expert_config             mac_cfg;
  mac_pcap&                     pcap;
  // Parameters passed to MAC scheduler.
  scheduler_expert_config        sched_cfg;
  scheduler_ue_metrics_notifier& metric_notifier;
};

} // namespace srsran
