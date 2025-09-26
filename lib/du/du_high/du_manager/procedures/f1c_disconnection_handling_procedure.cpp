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
#include "initial_du_setup_procedure.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srsran::srs_du;

f1c_disconnection_handling_procedure::f1c_disconnection_handling_procedure(
    const du_manager_params&            params_,
    du_cell_manager&                    cell_mng_,
    du_ue_manager&                      ue_mng_,
    du_manager_metrics_aggregator_impl& metrics_) :
  params(params_), cell_mng(cell_mng_), ue_mng(ue_mng_), metrics(metrics_), logger(srslog::fetch_basic_logger("DU-MNG"))
{
}

void f1c_disconnection_handling_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("Stopping DU activity. Cause: F1-C connection to CU-CP lost.");

  // Tear down all cells.
  for (; i != cell_mng.nof_cells(); ++i) {
    if (cell_mng.is_cell_active(to_du_cell_index(i))) {
      CORO_AWAIT(launch_async<du_cell_stop_procedure>(
          ue_mng, cell_mng, params, to_du_cell_index(i), du_cell_stop_procedure::ue_removal_mode::no_f1_triggers));
    }
  }

  // TODO: Remove timeout once FAPI supports cell stop.
  CORO_AWAIT(async_wait_for(params.services.timers.create_unique_timer(params.services.du_mng_exec),
                            std::chrono::milliseconds(32)));
  cell_mng.remove_all_cells();

  logger.info("Attempting new F1-C connection to CU-CP...");

  // Attempt a new F1 setup connection.
  CORO_AWAIT(launch_async<initial_du_setup_procedure>(params, cell_mng, metrics));

  CORO_RETURN();
}
