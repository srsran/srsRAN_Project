
#ifndef SRSGNB_DU_MANAGER_CONTEXT_H
#define SRSGNB_DU_MANAGER_CONTEXT_H

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1ap/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"

namespace srsgnb {

struct du_ue_bearer_context {
  std::unique_ptr<rlc_ul_bearer>        ul_bearer;
  std::unique_ptr<mac_ul_dcch_notifier> mac_ul_notifier;
};

struct du_ue_context {
  du_cell_index_t                   pcell_index;
  du_ue_index_t                     ue_index;
  rnti_t                            crnti;
  slot_vector<du_ue_bearer_context> bearers;
};

struct du_manager_context {
  mac_config_interface*                  mac;
  du_manager_config_notifier*            f1ap_cfg_notifier;
  rlc_ul_sdu_notifier*                   rlc_ul_notifier;
  slot_array<du_ue_context, MAX_NOF_UES> ue_db;
};

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_CONTEXT_H
