
#ifndef SRSGNB_F1AP_CONTEXT_H
#define SRSGNB_F1AP_CONTEXT_H

#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/f1ap_du.h"

namespace srsgnb {

struct f1ap_du_ue_context {
  du_ue_index_t gnb_du_f1ap_ue_id = -1;
  du_ue_index_t gnb_cu_f1ap_ue_id = -1;
  rnti_t        crnti             = 0;
};

struct f1ap_du_cell_context {};

struct f1ap_du_context {
  std::vector<f1ap_du_cell_context> cell_ctxt_manager;
  std::vector<f1ap_du_ue_context>   ue_ctxt_manager;
};

} // namespace srsgnb

#endif // SRSGNB_F1AP_CONTEXT_H
