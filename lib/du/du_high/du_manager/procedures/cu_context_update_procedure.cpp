/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_context_update_procedure.h"
#include "../du_cell_manager.h"

using namespace srsran;
using namespace srs_du;

cu_context_update_procedure::cu_context_update_procedure(const gnbcu_config_update_request& request_,
                                                         du_cell_manager&                   cell_mng_) :
  request(request_), cell_mng(cell_mng_)
{
}

void cu_context_update_procedure::operator()(coro_context<async_task<gnbcu_config_update_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Activate respective cells.
  for (list_index = 0; list_index != request.cells_to_activate.size(); ++list_index) {
    CORO_AWAIT_VALUE(bool success, cell_mng.start(cell_mng.get_cell_index(request.cells_to_activate[list_index].cgi)));
    if (not success) {
      resp.cells_failed_to_activate.push_back(request.cells_to_activate[list_index].cgi);
    }
  }

  CORO_RETURN(resp);
}
