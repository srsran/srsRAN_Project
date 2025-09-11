/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "cu_configuration_procedure.h"
#include "../du_cell_manager.h"
#include "../du_metrics_aggregator_impl.h"
#include "../du_ue/du_ue_manager.h"
#include "du_cell_stop_procedure.h"
#include "du_ue_reset_procedure.h"
#include "srsran/support/async/async_no_op_task.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srs_du;

cu_configuration_procedure::cu_configuration_procedure(const gnbcu_config_update_request&  request_,
                                                       du_cell_manager&                    cell_mng_,
                                                       du_ue_manager&                      ue_mng_,
                                                       const du_manager_params&            du_params_,
                                                       du_manager_metrics_aggregator_impl& metrics_) :
  request(request_), cell_mng(cell_mng_), ue_mng(ue_mng_), du_params(du_params_), metrics(metrics_)
{
}

void cu_configuration_procedure::operator()(coro_context<async_task<gnbcu_config_update_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Deactivate cells.
  for (list_index = 0; list_index != request.cells_to_deactivate.size(); ++list_index) {
    CORO_AWAIT(stop_cell(request.cells_to_deactivate[list_index]));
  }

  // Activate cells.
  for (list_index = 0; list_index != request.cells_to_activate.size(); ++list_index) {
    CORO_AWAIT_VALUE(bool success, start_cell(request.cells_to_activate[list_index].cgi));
    if (not success) {
      // Failed to start cell.
      resp.cells_failed_to_activate.push_back(request.cells_to_activate[list_index].cgi);
    }

    // Add cell to metrics.
    metrics.handle_cell_start(cell_mng.get_cell_index(request.cells_to_activate[list_index].cgi));
  }

  CORO_RETURN(resp);
}

async_task<bool> cu_configuration_procedure::start_cell(const nr_cell_global_id_t& cgi)
{
  const du_cell_index_t cell_index = cell_mng.get_cell_index(cgi);
  if (cell_index == INVALID_DU_CELL_INDEX) {
    return launch_no_op_task(false);
  }
  return cell_mng.start(cell_index);
}

async_task<void> cu_configuration_procedure::stop_cell(const nr_cell_global_id_t& cgi)
{
  const du_cell_index_t cell_index = cell_mng.get_cell_index(cgi);

  return launch_async<du_cell_stop_procedure>(
      ue_mng, cell_mng, du_params, cell_index, du_cell_stop_procedure::ue_removal_mode::trigger_f1_ue_release_request);
}
