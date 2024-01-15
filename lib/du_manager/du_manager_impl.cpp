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

#include "du_manager_impl.h"
#include "procedures/du_stop_procedure.h"
#include "procedures/du_ue_ric_configuration_procedure.h"
#include "procedures/initial_du_setup_procedure.h"
#include <condition_variable>
#include <future>
#include <thread>

using namespace srsran;
using namespace srs_du;

du_manager_impl::du_manager_impl(const du_manager_params& params_) :
  params(params_),
  logger(srslog::fetch_basic_logger("DU-MNG")),
  cell_mng(params),
  cell_res_alloc(params.ran.cells, params.ran.srbs, params.ran.qos),
  ue_mng(params, cell_res_alloc),
  main_ctrl_loop(128)
{
}

du_manager_impl::~du_manager_impl()
{
  stop();
}

void du_manager_impl::start()
{
  logger.info("DU manager starting...");

  std::unique_lock<std::mutex> lock(mutex);
  if (std::exchange(running, true)) {
    logger.warning("DU Manager already started. Ignoring start request.");
    return;
  }

  std::promise<void> p;
  std::future<void>  fut = p.get_future();

  if (not params.services.du_mng_exec.execute([this, &p]() {
        main_ctrl_loop.schedule([this, &p](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);

          // Connect to CU-CP and send F1 Setup Request and await for F1 setup response.
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

  logger.info("DU manager started successfully.");
}

void du_manager_impl::stop()
{
  std::unique_lock<std::mutex> lock(mutex);
  if (not std::exchange(running, false)) {
    return;
  }

  eager_async_task<void> main_loop;
  std::atomic<bool>      main_loop_stopped{false};

  auto stop_du_main_loop = [this, &main_loop, &main_loop_stopped]() mutable {
    if (main_loop.empty()) {
      // First call. Initiate shutdown operations.

      // Start DU disconnect procedure.
      schedule_async_task(launch_async<du_stop_procedure>(ue_mng));

      // Once the disconnection procedure is complete, stop main control loop and communicate back with the caller
      // thread.
      main_loop = main_ctrl_loop.request_stop();
    }

    if (main_loop.ready()) {
      // if the main loop finished, return back to the caller.
      main_loop_stopped = true;
    }
  };

  // Wait until the all tasks of the main loop are completed and main loop has stopped.
  while (not main_loop_stopped) {
    if (not params.services.du_mng_exec.execute(stop_du_main_loop)) {
      logger.error("Unable to stop DU Manager.");
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void du_manager_impl::handle_ul_ccch_indication(const ul_ccch_indication_message& msg)
{
  // Switch DU Manager exec context
  if (not params.services.du_mng_exec.execute([this, msg = std::move(msg)]() {
        // Start UE create procedure
        ue_mng.handle_ue_create_request(msg);
      })) {
    logger.warning("Discarding UL-CCCH message cell={} tc-rnti={} slot_rx={}. Cause: DU manager task queue is full",
                   msg.cell_index,
                   msg.tc_rnti,
                   msg.slot_rx);
  }
}

du_ue_index_t du_manager_impl::find_unused_du_ue_index()
{
  return ue_mng.find_unused_du_ue_index();
}

async_task<f1ap_ue_context_creation_response>
du_manager_impl::handle_ue_context_creation(const f1ap_ue_context_creation_request& request)
{
  return ue_mng.handle_ue_create_request(request);
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

void du_manager_impl::handle_ue_reestablishment(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index)
{
  ue_mng.handle_reestablishment_request(new_ue_index, old_ue_index);
}

size_t du_manager_impl::nof_ues()
{
  // TODO: This is temporary code.
  std::promise<size_t> p;
  std::future<size_t>  fut = p.get_future();
  if (not params.services.du_mng_exec.execute([this, &p]() { p.set_value(ue_mng.nof_ues()); })) {
    logger.warning("Unable to compute the number of UEs active in the DU");
    return std::numeric_limits<size_t>::max();
  }
  return fut.get();
}

async_task<du_mac_sched_control_config_response>
du_manager_impl::configure_ue_mac_scheduler(du_mac_sched_control_config reconf)
{
  return launch_async<srs_du::du_ue_ric_configuration_procedure>(reconf, ue_mng, params);
}
