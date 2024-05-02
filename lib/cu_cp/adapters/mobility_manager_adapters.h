/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cu_cp_impl_interface.h"
#include "../mobility_manager/mobility_manager_impl.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between mobility manager and CU-CP to trigger handover.
class mobility_manager_adapter : public mobility_manager_cu_cp_notifier
{
public:
  mobility_manager_adapter() = default;

  void connect_cu_cp(cu_cp_mobility_manager_handler& cu_cp_handler_) { cu_cp_handler = &cu_cp_handler_; }

  async_task<cu_cp_inter_du_handover_response>
  on_inter_du_handover_required(const cu_cp_inter_du_handover_request& request,
                                du_index_t                             source_du_index,
                                du_index_t                             target_du_index) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_handler->handle_inter_du_handover_request(request, source_du_index, target_du_index);
  }

private:
  cu_cp_mobility_manager_handler* cu_cp_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
