
#ifndef SRSGNB_DU_MANAGER_CONTEXT_H
#define SRSGNB_DU_MANAGER_CONTEXT_H

#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1ap/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"

namespace srsgnb {

struct du_ue_context {
  du_cell_index_t pcell_index;
  du_ue_index_t   ue_index;
  rnti_t          crnti;
};

struct du_manager_context {
  rlc_config_interface*       rlc;
  mac_config_interface*       mac;
  du_manager_config_notifier* f1ap_notifier;
  std::vector<du_ue_context>  ue_db;
};

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_CONTEXT_H
