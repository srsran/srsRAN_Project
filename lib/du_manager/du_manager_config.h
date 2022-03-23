
#ifndef SRSGNB_DU_MANAGER_CONFIG_H
#define SRSGNB_DU_MANAGER_CONFIG_H

#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/support/task_executor.h"

namespace srsgnb {

struct du_manager_config_t {
  rlc_sdu_rx_notifier*  rlc_ul_notifier;
  mac_configurer*       mac;
  f1ap_du_configurer*   f1ap;
  f1ap_du_ul_interface* f1ap_ul;
  srslog::basic_logger& logger = srslog::fetch_basic_logger("DU-MNG");
  task_executor*        du_mng_exec;
  du_setup_params       setup_params; /// Will be merged with top-level config struct
};

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_CONFIG_H
