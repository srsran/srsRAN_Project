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

#include "srsran/adt/span.h"
#include "srsran/du/du_test_config.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/mac/mac.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

struct mac_control_config {
  srslog::basic_logger&         logger;
  mac_ul_ccch_notifier&         event_notifier;
  du_high_ue_executor_mapper&   ue_exec_mapper;
  du_high_cell_executor_mapper& cell_exec_mapper;
  task_executor&                ctrl_exec;

  mac_control_config(mac_ul_ccch_notifier&         event_notifier_,
                     du_high_ue_executor_mapper&   ul_exec_,
                     du_high_cell_executor_mapper& dl_exec_,
                     task_executor&                ctrl_exec_) :
    logger(srslog::fetch_basic_logger("MAC", true)),
    event_notifier(event_notifier_),
    ue_exec_mapper(ul_exec_),
    cell_exec_mapper(dl_exec_),
    ctrl_exec(ctrl_exec_)
  {
  }
};

} // namespace srsran
