/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../du_ue/du_ue.h"
#include "../du_ue/du_ue_manager_repository.h"
#include "procedure_logger.h"
#include "srsran/du/du_high/du_manager/du_manager.h"
#include "srsran/du/du_high/du_manager/du_manager_params.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

class ue_deletion_procedure
{
public:
  ue_deletion_procedure(du_ue_index_t ue_index_, du_ue_manager_repository& ue_mng_, const du_manager_params& du_params);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Delete"; }

private:
  async_task<mac_ue_delete_response> launch_mac_ue_delete();

  // Disconnects all the DRBs and SRBs associated with the UE. After the bearers have been disconnected, it is safe
  // to start the deletion of the associated bearer contexts.
  async_task<void> stop_ue_bearer_traffic();

  const du_ue_index_t       ue_index;
  du_ue_manager_repository& ue_mng;
  const du_manager_params&  du_params;
  ue_procedure_logger       proc_logger;

  du_ue* ue = nullptr;
};

} // namespace srs_du
} // namespace srsran
