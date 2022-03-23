
#ifndef SRSGNB_DU_MANAGER_CONTEXT_H
#define SRSGNB_DU_MANAGER_CONTEXT_H

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"

namespace srsgnb {

struct du_manager_context {
  mac_configurer*      mac;
  f1ap_du_configurer*  f1ap;
  rlc_sdu_rx_notifier* rlc_ul_notifier;
};

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_CONTEXT_H
