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

#include "../du_ue/du_ue.h"
#include "../du_ue/du_ue_manager_repository.h"
#include "procedure_logger.h"
#include "srsran/du_manager/du_manager.h"
#include "srsran/du_manager/du_manager_params.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

class ue_deletion_procedure
{
public:
  ue_deletion_procedure(const f1ap_ue_delete_request& msg_,
                        du_ue_manager_repository&     ue_mng_,
                        const du_manager_params&      du_params);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Delete"; }

private:
  async_task<mac_ue_delete_response> launch_mac_ue_delete();

  // Disconnects all the DRBs and SRBs associated with the UE. After the bearers have been disconnected, it is safe
  // to start the deletion of the associated bearer contexts.
  async_task<void> disconnect_inter_layer_interfaces();

  const f1ap_ue_delete_request msg;
  du_ue_manager_repository&    ue_mng;
  const du_manager_params&     du_params;
  ue_procedure_logger          proc_logger;

  du_ue* ue = nullptr;
};

} // namespace srs_du
} // namespace srsran
