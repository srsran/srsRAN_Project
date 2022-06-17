
#ifndef SRSGNB_DU_HIGH_CONFIGURATION_H
#define SRSGNB_DU_HIGH_CONFIGURATION_H

#include "srsgnb/du/du_cell_config.h"
#include "srsgnb/du_high/du_l2_dl_executor_mapper.h"
#include "srsgnb/du_high/du_l2_ul_executor_mapper.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/mac/mac_cell_result.h"

namespace srsgnb {
namespace srs_du {

/// Configuration passed to DU-High.
struct du_high_configuration {
  task_executor*              du_mng_executor = nullptr;
  du_l2_ul_executor_mapper*   ul_executors    = nullptr;
  du_l2_dl_executor_mapper*   dl_executors    = nullptr;
  f1c_message_notifier*       f1c_notifier    = nullptr;
  mac_result_notifier*        phy_adapter     = nullptr;
  std::vector<du_cell_config> cells;
};

} // namespace srs_du
} // namespace srsgnb

#endif // SRSGNB_DU_HIGH_CONFIGURATION_H
