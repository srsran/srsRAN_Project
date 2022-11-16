
#pragma once

#include "srsgnb/du/du_cell_config.h"
#include "srsgnb/du_high/du_high_cell_executor_mapper.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include "srsgnb/f1c/du/f1ap_du.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/scheduler/config/scheduler_expert_config.h"

namespace srsgnb {
namespace srs_du {

/// Configuration passed to DU-High.
struct du_high_configuration {
  task_executor*                du_mng_executor = nullptr;
  du_high_ue_executor_mapper*   ul_executors    = nullptr;
  du_high_cell_executor_mapper* dl_executors    = nullptr;
  f1c_message_notifier*         f1c_notifier    = nullptr;
  mac_result_notifier*          phy_adapter     = nullptr;
  std::vector<du_cell_config>   cells;
  scheduler_expert_config       sched_cfg;
};

} // namespace srs_du
} // namespace srsgnb
