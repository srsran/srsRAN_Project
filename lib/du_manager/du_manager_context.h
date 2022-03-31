
#ifndef SRSGNB_DU_MANAGER_CONTEXT_H
#define SRSGNB_DU_MANAGER_CONTEXT_H

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"

namespace srsgnb {

struct du_manager_context {
  du_setup_result f1_setup_result;
};

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_CONTEXT_H
