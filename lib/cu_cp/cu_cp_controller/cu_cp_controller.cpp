/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_controller.h"
#include "../cu_up_processor/cu_up_processor_repository.h"
#include "../routine_managers/cu_cp_routine_manager.h"
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

namespace {

template <typename TaskFactory>
void sync_run_async_task(task_executor&         exec_to_dispatch,
                         common_task_scheduler& task_sched,
                         TaskFactory&&          task_factory,
                         srslog::basic_logger&  logger)
{
  std::mutex              complete_mutex;
  std::condition_variable cvar;
  bool                    completed = false;

  // Dispatch task to cu-cp execution context.
  while (not exec_to_dispatch.defer([&, task_factory = std::move(task_factory)]() {
    // Schedule async task.
    task_sched.schedule_async_task(
        launch_async([&complete_mutex, &cvar, &completed, task_factory = std::move(task_factory)](
                         coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);

          // Create task.
          CORO_AWAIT(task_factory());

          // Notify that the task is complete.
          {
            std::lock_guard<std::mutex> lock(complete_mutex);
            completed = true;
            cvar.notify_one();
          }

          CORO_RETURN();
        }));
  })) {
    logger.warning("Failed to dispatch CU-CP task");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Wait for task to complete.
  std::unique_lock<std::mutex> lock(complete_mutex);
  cvar.wait(lock, [&completed] { return completed; });
}

} // namespace

cu_cp_controller::cu_cp_controller(const cu_cp_configuration&        config_,
                                   cu_cp_routine_manager&            routine_manager_,
                                   ue_manager&                       ue_mng_,
                                   const ngap_configuration&         ngap_cfg_,
                                   ngap_connection_manager&          ngap_conn_mng_,
                                   const cu_up_processor_repository& cu_ups_,
                                   du_processor_repository&          dus,
                                   task_executor&                    ctrl_exec_) :
  cfg(config_),
  ue_mng(ue_mng_),
  cu_ups(cu_ups_),
  routine_mng(routine_manager_),
  ctrl_exec(ctrl_exec_),
  logger(srslog::fetch_basic_logger("CU-CP")),
  amf_mng(routine_manager_, ngap_cfg_, ngap_conn_mng_),
  du_mng(cfg.max_nof_dus, dus, ctrl_exec, routine_manager_)
{
  (void)ue_mng;
}

void cu_cp_controller::stop()
{
  // Stop and delete DU connections.
  du_mng.stop();

  // Run to completion task handling AMF disconnection
  sync_run_async_task(
      ctrl_exec, routine_mng, [this]() { return amf_mng.stop(); }, logger);
}

bool cu_cp_controller::handle_du_setup_request(const du_setup_request& req)
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

  if (cu_ups.get_nof_cu_ups() == 0) {
    return false;
  }

  return true;
}
