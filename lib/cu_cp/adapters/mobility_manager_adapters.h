/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
