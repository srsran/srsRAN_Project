/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_DU_ADAPTERS_H
#define SRSGNB_CU_CP_DU_ADAPTERS_H

#include "srsgnb/cu_cp/cu_cp_manager.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between F1AP and DU processor, DU specific procedure outcomes (e.g. F1 Remove)
class cu_cp_manager_f1ap_event_indicator : public f1c_du_management_notifier
{
public:
  void connect(cu_cp_manager_du_handler& cu_cp_mng_) { du_handler = &cu_cp_mng_; }
  void on_du_remove_request_received(const du_index_t du_index) override
  {
    srsran_assert(du_handler != nullptr, "DU handler must not be nullptr");
    du_handler->handle_du_remove_request(du_index);
  }

private:
  cu_cp_manager_du_handler* du_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_CU_CP_DU_ADAPTERS_H
