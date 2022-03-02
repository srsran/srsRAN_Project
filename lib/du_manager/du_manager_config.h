
#ifndef SRSGNB_DU_MANAGER_CONFIG_H
#define SRSGNB_DU_MANAGER_CONFIG_H

#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/support/task_executor.h"

namespace srsgnb {

struct du_manager_config_t {
  du_manager_config_notifier* f1ap_cfg_notifier;
  rlc_ul_sdu_notifier*        rlc_ul_notifier;
  mac_config_interface*       mac;
  srslog::basic_logger&       logger = srslog::fetch_basic_logger("DU-MNG");
  task_executor*              du_mng_exec;
};

class ue_manager_ctrl_configurer
{
public:
  virtual ~ue_manager_ctrl_configurer()          = default;
  virtual void remove_ue(du_ue_index_t ue_index) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_CONFIG_H
