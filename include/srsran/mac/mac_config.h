/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du_high/du_high_executor_mapper.h"
#include "srsran/du/du_high/du_test_mode_config.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/mac/mac_pdu_handler.h"
#include "srsran/pcap/mac_pcap.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {

/// \brief Implementation-specific parameters used to tune MAC operation.
struct mac_expert_config {
  /// Initial C-RNTI to assign to created UEs.
  rnti_t initial_crnti = to_rnti(0x4601);
  /// \brief Implementation-specific parameters used to tune MAC operation per cell.
  struct mac_expert_cell_config {
    /// \brief Maximum number of consecutive DL KOs before an RLF is reported.
    unsigned max_consecutive_dl_kos = 100;
    /// \brief Maximum number of consecutive UL KOs before an RLF is reported.
    unsigned max_consecutive_ul_kos = 100;
    /// \brief Maximum number of consecutive non-decoded CSI before an RLF is reported.
    unsigned max_consecutive_csi_dtx = 100;
  };
  std::vector<mac_expert_cell_config> configs;
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
  scheduler_expert_config     sched_cfg;
  scheduler_metrics_notifier& metric_notifier;
};

} // namespace srsran
