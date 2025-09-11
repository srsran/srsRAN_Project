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

#include "du_manager_impl.h"
#include "du_positioning_handler_factory.h"
#include "procedures/cu_configuration_procedure.h"
#include "procedures/du_mac_si_pdu_update_procedure.h"
#include "procedures/du_param_config_procedure.h"
#include "procedures/du_stop_procedure.h"
#include "procedures/du_ue_reset_procedure.h"
#include "procedures/du_ue_ric_configuration_procedure.h"
#include "procedures/initial_du_setup_procedure.h"
#include "srsran/support/executors/execute_until_success.h"
#include <condition_variable>
#include <future>
#include <thread>

using namespace srsran;
using namespace srs_du;

du_manager_impl::du_manager_impl(const du_manager_params& params_) :
  params(params_),
  logger(srslog::fetch_basic_logger("DU-MNG")),
  cell_mng(params),
  cell_res_alloc(params.ran.cells, params.mac.sched_cfg, params.ran.srbs, params.ran.qos, params.test_cfg),
  ue_mng(params, cell_res_alloc),
  positioning_handler(create_du_positioning_handler(params, cell_mng, ue_mng, logger)),
  metrics(params.metrics,
          params.services.du_mng_exec,
          params.services.timers,
          params.f1ap.metrics,
          params.mac.mac_metrics_notif),
  main_ctrl_loop(128)
{
}

du_manager_impl::~du_manager_impl()
{
  stop();
}

void du_manager_impl::start()
{
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (running) {
      logger.warning("Ignoring start request. Cause: DU Manager already started.");
      return;
    }
  }

  logger.info("DU manager starting...");

  if (not params.services.du_mng_exec.execute([this]() {
        main_ctrl_loop.schedule([this](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);

          // Connect to CU-CP and send F1 Setup Request and await for F1 setup response.
          CORO_AWAIT(launch_async<initial_du_setup_procedure>(params, cell_mng, metrics));

          // Signal start() caller thread that the operation is complete.
          std::lock_guard<std::mutex> lock(mutex);
          running = true;
          cvar.notify_all();

          CORO_RETURN();
        });
      })) {
    report_fatal_error("Unable to initiate DU setup procedure");
  }

  // Block waiting for DU setup to complete.
  std::unique_lock<std::mutex> lock(mutex);
  cvar.wait(lock, [this]() { return running; });

  logger.info("DU manager started successfully.");
}

void du_manager_impl::stop()
{
  {
    // Avoid stopping the DU Manager multiple times.
    std::lock_guard<std::mutex> lock(mutex);
    if (not running) {
      return;
    }
  }

  while (not params.services.du_mng_exec.execute([this]() { handle_du_stop_request(); })) {
    logger.error("Unable to dispatch DU Manager shutdown. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return;
  }

  // Wait for the DU Manager thread to signal that the stop was completed.
  std::unique_lock<std::mutex> lock(mutex);
  cvar.wait(lock, [this]() { return not running; });
}

void du_manager_impl::handle_ul_ccch_indication(const ul_ccch_indication_message& msg)
{
  // Switch DU Manager exec context
  if (not params.services.du_mng_exec.execute([this, msg = std::move(msg)]() {
        // Start UE create procedure
        ue_mng.handle_ue_create_request(msg);
      })) {
    logger.warning("Discarding UL-CCCH message cell={} tc-rnti={} slot_rx={}. Cause: DU manager task queue is full",
                   fmt::underlying(msg.cell_index),
                   msg.tc_rnti,
                   msg.slot_rx);
  }
}

void du_manager_impl::handle_du_stop_request()
{
  if (not running) {
    // Already stopped.
    return;
  }

  // Start DU stop procedure.
  schedule_async_task(launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    if (not running) {
      // Already stopped.
      CORO_EARLY_RETURN();
    }

    // Tear down activity in remaining layers.
    CORO_AWAIT(launch_async<du_stop_procedure>(ue_mng, cell_mng, params.f1ap.conn_mng));

    // DU stop successfully finished.
    // Dispatch main async task loop destruction via defer so that the current coroutine ends successfully.
    while (not params.services.du_mng_exec.defer([this]() {
      // Let main loop go out of scope and be destroyed.
      auto main_loop = main_ctrl_loop.request_stop();

      std::lock_guard<std::mutex> lock(mutex);
      running = false;
      cvar.notify_all();
    })) {
      logger.warning("Unable to stop DU Manager. Retrying...");
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    CORO_RETURN();
  }));
}

du_ue_index_t du_manager_impl::find_unused_du_ue_index()
{
  return ue_mng.find_unused_du_ue_index();
}

async_task<void> du_manager_impl::handle_f1_reset_request(const std::vector<du_ue_index_t>& ues_to_reset)
{
  return launch_async<du_ue_reset_procedure>(ues_to_reset, ue_mng, params, std::nullopt);
}

async_task<gnbcu_config_update_response>
du_manager_impl::handle_cu_context_update_request(const gnbcu_config_update_request& request)
{
  return launch_async<cu_configuration_procedure>(request, cell_mng, ue_mng, params, metrics);
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

async_task<void> du_manager_impl::handle_ue_deactivation_request(du_ue_index_t ue_index)
{
  return ue_mng.handle_ue_deactivation_request(ue_index);
}

void du_manager_impl::handle_ue_reestablishment(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index)
{
  ue_mng.handle_reestablishment_request(new_ue_index, old_ue_index);
}

void du_manager_impl::handle_ue_config_applied(du_ue_index_t ue_index)
{
  ue_mng.handle_ue_config_applied(ue_index);
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

mac_cell_time_mapper& du_manager_impl::get_time_mapper()
{
  return params.mac.cell_mng.get_time_mapper(to_du_cell_index(0));
}

async_task<du_mac_sched_control_config_response>
du_manager_impl::configure_ue_mac_scheduler(du_mac_sched_control_config reconf)
{
  return launch_async<du_ue_ric_configuration_procedure>(reconf, ue_mng, params);
}

du_param_config_response du_manager_impl::handle_operator_config_request(const du_param_config_request& req)
{
  std::promise<du_param_config_response> p;
  std::future<du_param_config_response>  fut = p.get_future();

  // Switch to DU manager execution context.
  execute_until_success(params.services.du_mng_exec, params.services.timers, [this, req, &p]() {
    // Dispatch common task.
    schedule_async_task(launch_async([&](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);

      // Launch config procedure.
      CORO_AWAIT_VALUE(auto resp, launch_async<du_param_config_procedure>(req, params, cell_mng));

      // signal back to caller.
      p.set_value(resp);

      CORO_RETURN();
    }));
  });

  return fut.get();
}

void du_manager_impl::handle_si_pdu_update(const du_si_pdu_update_request& req)
{
  schedule_async_task(launch_async([&req, this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    if (not running) {
      // Already stopped.
      CORO_EARLY_RETURN();
    }
    CORO_AWAIT(start_du_mac_si_pdu_update(req, params, cell_mng));

    CORO_RETURN();
  }));
}
