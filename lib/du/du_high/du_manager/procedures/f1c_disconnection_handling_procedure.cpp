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

#include "f1c_disconnection_handling_procedure.h"
#include "../du_cell_manager.h"
#include "du_cell_stop_procedure.h"
#include "du_setup_procedure.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srsran::srs_du;

f1c_disconnection_handling_procedure::f1c_disconnection_handling_procedure(du_proc_context_view ctxt_) :
  ctxt(ctxt_), proc_logger(ctxt.logger, "F1-C Disconnection Handling")
{
}

void f1c_disconnection_handling_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  // Stop all cells.
  // Note: Do not delete the cells until the FAPI supports stop.
  for (; i != ctxt.cell_mng.nof_cells(); ++i) {
    if (ctxt.cell_mng.is_cell_active(to_du_cell_index(i))) {
      CORO_AWAIT(launch_async<du_cell_stop_procedure>(ctxt.ue_mng,
                                                      ctxt.cell_mng,
                                                      ctxt.params,
                                                      to_du_cell_index(i),
                                                      du_cell_stop_procedure::ue_removal_mode::no_f1_triggers));
    }
  }

  proc_logger.log_progress("DU activity was stopped. Attempting to re-establish F1-C connection..");

  // Attempt a new F1 setup connection.
  CORO_AWAIT(launch_async<du_setup_procedure>(ctxt, du_start_request{false, std::numeric_limits<unsigned>::max()}));

  proc_logger.log_proc_completed();

  CORO_RETURN();
}
