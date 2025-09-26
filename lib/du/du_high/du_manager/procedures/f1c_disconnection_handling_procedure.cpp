/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1c_disconnection_handling_procedure.h"
#include "../du_cell_manager.h"
#include "du_cell_stop_procedure.h"
#include "du_setup_procedure.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srsran::srs_du;

f1c_disconnection_handling_procedure::f1c_disconnection_handling_procedure(
    const du_manager_params&            params_,
    du_cell_manager&                    cell_mng_,
    du_ue_manager&                      ue_mng_,
    du_manager_metrics_aggregator_impl& metrics_) :
  params(params_),
  cell_mng(cell_mng_),
  ue_mng(ue_mng_),
  metrics(metrics_),
  logger(srslog::fetch_basic_logger("DU-MNG")),
  proc_logger(logger, "F1-C Disconnection Handling")
{
}

void f1c_disconnection_handling_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  // Stop all cells.
  // Note: Do not delete the cells until the FAPI supports stop.
  for (; i != cell_mng.nof_cells(); ++i) {
    if (cell_mng.is_cell_active(to_du_cell_index(i))) {
      CORO_AWAIT(launch_async<du_cell_stop_procedure>(
          ue_mng, cell_mng, params, to_du_cell_index(i), du_cell_stop_procedure::ue_removal_mode::no_f1_triggers));
    }
  }

  proc_logger.log_progress("DU activity was stopped. Attempting to re-establish F1-C connection..");

  // Attempt a new F1 setup connection.
  CORO_AWAIT(launch_async<du_setup_procedure>(
      params, cell_mng, metrics, du_start_request{false, std::numeric_limits<unsigned>::max()}));

  proc_logger.log_proc_completed();

  CORO_RETURN();
}
