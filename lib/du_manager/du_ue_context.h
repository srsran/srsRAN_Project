
#ifndef SRSGNB_DU_UE_CONTEXT_H
#define SRSGNB_DU_UE_CONTEXT_H

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {

struct du_logical_channel_context {
  lcid_t                               lcid;
  std::unique_ptr<rlc_pdu_handler>       ul_bearer;
  std::unique_ptr<mac_sdu_rx_notifier> mac_ul_notifier;
};

struct du_ue_context {
  du_ue_index_t                           ue_index;
  rnti_t                                  rnti;
  du_cell_index_t                         pcell_index;
  slot_vector<du_logical_channel_context> bearers;
};

} // namespace srsgnb

#endif // SRSGNB_DU_UE_CONTEXT_H
