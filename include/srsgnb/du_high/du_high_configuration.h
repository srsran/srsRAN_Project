
#ifndef SRSGNB_DU_HIGH_CONFIGURATION_H
#define SRSGNB_DU_HIGH_CONFIGURATION_H

#include "srsgnb/du_high/du_l2_dl_executor_mapper.h"
#include "srsgnb/du_high/du_l2_ul_executor_mapper.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/mac/mac_cell_result.h"

namespace srsgnb {

/// Configuration passed to DU-High.
struct du_high_configuration {
  task_executor*            du_mng_executor = nullptr;
  du_l2_ul_executor_mapper* ul_executors    = nullptr;
  du_l2_dl_executor_mapper* dl_executors    = nullptr;
  f1c_pdu_handler*          f1c_pdu_hdl     = nullptr;
  mac_result_notifier*      phy_adapter     = nullptr;
};

} // namespace srsgnb

#endif // SRSGNB_DU_HIGH_CONFIGURATION_H
