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

#include "srsgnb/adt/span.h"
#include "srsgnb/du_high/du_high_cell_executor_mapper.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/pcap/mac_pcap.h"

namespace srsgnb {

struct mac_common_config_t {
  srslog::basic_logger&         logger;
  mac_pcap&                     pcap;
  mac_ul_ccch_notifier&         event_notifier;
  du_high_ue_executor_mapper&   ue_exec_mapper;
  du_high_cell_executor_mapper& cell_exec_mapper;
  task_executor&                ctrl_exec;
  mac_result_notifier&          phy_notifier;

  mac_common_config_t(mac_ul_ccch_notifier&         event_notifier_,
                      du_high_ue_executor_mapper&   ul_exec_,
                      du_high_cell_executor_mapper& dl_exec_,
                      task_executor&                ctrl_exec_,
                      mac_result_notifier&          phy_notifier_,
                      mac_pcap&                     pcap_) :
    logger(srslog::fetch_basic_logger("MAC", true)),
    pcap(pcap_),
    event_notifier(event_notifier_),
    ue_exec_mapper(ul_exec_),
    cell_exec_mapper(dl_exec_),
    ctrl_exec(ctrl_exec_),
    phy_notifier(phy_notifier_)
  {
  }
};

} // namespace srsgnb
