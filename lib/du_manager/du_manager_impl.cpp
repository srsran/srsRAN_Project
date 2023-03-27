/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_manager_impl.h"
#include "procedures/initial_du_setup_procedure.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include <condition_variable>
#include <future>

using namespace srsran;
using namespace srs_du;

du_manager_impl::du_manager_impl(const du_manager_params& params_) :
  params(params_),
  logger(srslog::fetch_basic_logger("DU-MNG")),
  cell_mng(params),
  cell_res_alloc(params.ran.cells, params.ran.qos),
  ue_mng(params, cell_res_alloc),
  main_ctrl_loop(128)
{
}

void du_manager_impl::start()
{
  std::promise<void> p;
  std::future<void>  fut = p.get_future();

  if (not params.services.du_mng_exec.execute([this, &p]() {
        // start F1 setup procedure.
        main_ctrl_loop.schedule([this, &p](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);

          // Send F1 Setup Request and await for F1 setup response.
          CORO_AWAIT(launch_async<initial_du_setup_procedure>(params, cell_mng));

          // Signal start() caller thread that the operation is complete.
          p.set_value();

          CORO_RETURN();
        });
      })) {
    report_fatal_error("Unable to initiate DU setup procedure");
  }

  // Block waiting for DU setup to complete.
  fut.wait();
}

void du_manager_impl::stop()
{
  // TODO.
}

void du_manager_impl::handle_ul_ccch_indication(const ul_ccch_indication_message& msg)
{
  // Switch DU Manager exec context
  if (not params.services.du_mng_exec.execute([this, msg = std::move(msg)]() {
        // Start UE create procedure
        ue_mng.handle_ue_create_request(msg);
      })) {
    logger.warning("Discarding UL-CCCH message cell={} c-rnti={:#x} slot_rx={}. Cause: DU manager task queue is full",
                   msg.cell_index,
                   msg.crnti,
                   msg.slot_rx);
  }
}

async_task<f1ap_ue_context_update_response>
du_manager_impl::handle_ue_context_update(const f1ap_ue_context_update_request& request)
{
  return ue_mng.handle_ue_config_request(request);
}

async_task<void> du_manager_impl::handle_ue_delete_request(const f1ap_ue_delete_request& request)
{
  return ue_mng.handle_ue_delete_request(request);
}

size_t du_manager_impl::nof_ues()
{
  // TODO: This is temporary code.
  static std::mutex              mutex;
  static std::condition_variable cvar;
  size_t                         result = MAX_NOF_DU_UES;
  if (not params.services.du_mng_exec.execute([this, &result]() {
        std::unique_lock<std::mutex> lock(mutex);
        result = ue_mng.get_ues().size();
        cvar.notify_one();
      })) {
    logger.warning("Unable to compute the number of UEs active in the DU");
    return std::numeric_limits<size_t>::max();
  }
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (result == MAX_NOF_DU_UES) {
      cvar.wait(lock);
    }
  }
  return result;
}
