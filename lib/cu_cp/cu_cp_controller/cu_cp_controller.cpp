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

#include "cu_cp_controller.h"
#include "../cu_up_processor/cu_up_processor_repository.h"
#include "../du_processor/du_processor_repository.h"
#include "../routine_managers/cu_cp_routine_manager.h"
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_controller::cu_cp_controller(const cu_cp_configuration&  config_,
                                   cu_cp_routine_manager&      routine_manager_,
                                   ue_manager&                 ue_mng_,
                                   ngap_connection_manager&    ngap_conn_mng_,
                                   cu_up_processor_repository& cu_ups_,
                                   du_processor_repository&    dus_,
                                   task_executor&              ctrl_exec_) :
  cfg(config_),
  ue_mng(ue_mng_),
  routine_mng(routine_manager_),
  ctrl_exec(ctrl_exec_),
  logger(srslog::fetch_basic_logger("CU-CP")),
  amf_mng(routine_manager_, cfg, ngap_conn_mng_),
  du_mng(cfg.admission.max_nof_dus, dus_, ctrl_exec, routine_manager_),
  cu_up_mng(cfg.admission.max_nof_cu_ups, cu_ups_, ctrl_exec, routine_manager_)
{
  (void)ue_mng;
}

void cu_cp_controller::stop()
{
  // Note: Called from separate outer thread.
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (not running) {
      return;
    }
  }

  // Stop and delete DU connections.
  du_mng.stop();

  // Stop and delete CU-UP connections.
  cu_up_mng.stop();

  // Stop AMF connection.
  while (not ctrl_exec.defer([this]() { stop_impl(); })) {
    logger.warning("Failed to dispatch CU-CP stop task. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Wait for stop_impl() to signal completion.
  std::unique_lock<std::mutex> lock(mutex);
  cvar.wait(lock, [this]() { return not running; });
}

void cu_cp_controller::stop_impl()
{
  routine_mng.schedule_async_task(launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    // Stop AMF connection.
    CORO_AWAIT(amf_mng.stop());

    // CU-CP stop successfully finished.
    // Dispatch main async task loop destruction via defer so that the current coroutine ends successfully.
    while (not ctrl_exec.defer([this]() {
      std::lock_guard<std::mutex> lock(mutex);
      running = false;
      cvar.notify_one();
    })) {
      logger.warning("Unable to stop DU Manager. Retrying...");
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    CORO_RETURN();
  }));
}

bool cu_cp_controller::handle_du_setup_request(du_index_t du_idx, const du_setup_request& req)
{
  if (not amf_mng.is_amf_connected()) {
    // If AMF is not connected, it either means that the CU-CP is not operational state or there is a CU-CP failure.
    return false;
  }
  return true;
}

bool cu_cp_controller::request_ue_setup() const
{
  if (not amf_mng.is_amf_connected()) {
    return false;
  }

  if (cu_up_mng.nof_cu_ups() == 0) {
    return false;
  }

  return true;
}
