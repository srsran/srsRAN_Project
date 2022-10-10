
#pragma once

#include "mac.h"
#include "mac_cell_result.h"
#include "srsgnb/du_high/du_high_cell_executor_mapper.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include <memory>

namespace srsgnb {

std::unique_ptr<mac_interface> create_mac(mac_ul_ccch_notifier&         event_notifier,
                                          du_high_ue_executor_mapper&   ul_exec_mapper,
                                          du_high_cell_executor_mapper& dl_exec_mapper,
                                          task_executor&                ctrl_exec,
                                          mac_result_notifier&          phy_notifier);
}
