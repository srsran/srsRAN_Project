/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/mac/mac_factory.h"
#include "mac_impl.h"
#include "srsgnb/du_high/du_high_cell_executor_mapper.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"

using namespace srsgnb;

std::unique_ptr<mac_interface> srsgnb::create_mac(mac_ul_ccch_notifier&         event_notifier,
                                                  du_high_ue_executor_mapper&   ul_exec_mapper,
                                                  du_high_cell_executor_mapper& dl_exec_mapper,
                                                  task_executor&                ctrl_exec,
                                                  mac_result_notifier&          phy_notifier)
{
  std::unique_ptr<mac_interface> macobj =
      std::make_unique<mac_impl>(event_notifier, ul_exec_mapper, dl_exec_mapper, ctrl_exec, phy_notifier);
  return macobj;
}
