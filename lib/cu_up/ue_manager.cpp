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

#include "ue_manager.h"
#include "srsran/support/async/execute_on_blocking.h"

using namespace srsran;
using namespace srs_cu_up;

ue_manager::ue_manager(const ue_manager_config& config, const ue_manager_dependencies& dependencies) :
  n3_config(config.n3_config),
  test_mode_config(config.test_mode_config),
  e1ap(dependencies.e1ap),
  f1u_gw(dependencies.f1u_gw),
  ngu_session_mngr(dependencies.ngu_session_mngr),
  gtpu_rx_demux(dependencies.gtpu_rx_demux),
  n3_teid_allocator(dependencies.n3_teid_allocator),
  f1u_teid_allocator(dependencies.f1u_teid_allocator),
  exec_pool(dependencies.exec_pool),
  ctrl_executor(exec_pool.ctrl_executor()),
  gtpu_pcap(dependencies.gtpu_pcap),
  timers(dependencies.timers),
  logger(dependencies.logger)
{
  // Initialize a ue task schedulers for all UE indexes.
  for (size_t i = 0; i < MAX_NOF_UES; ++i) {
    ue_task_schedulers.emplace(i, UE_TASK_QUEUE_SIZE);
  }
}

async_task<void> ue_manager::stop()
{
  // Stopping all UE's. Stop GTP-U demux first.
  gtpu_rx_demux.stop();

  // Routine to stop all UEs
  return remove_all_ues();
}

async_task<void> ue_manager::remove_all_ues()
{
  // Routine to stop all UEs
  auto ue_it = ue_db.begin();
  return launch_async([this, ue_it](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Remove all UEs.
    while (ue_it != ue_db.end()) {
      CORO_AWAIT(schedule_and_wait_ue_removal((ue_it++)->first));
    }

    CORO_RETURN();
  });
}

ue_context* ue_manager::find_ue(ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index={}", fmt::underlying(ue_index));
  return ue_db.find(ue_index) != ue_db.end() ? ue_db[ue_index].get() : nullptr;
}

ue_context* ue_manager::add_ue(const ue_context_cfg& ue_cfg)
{
  if (ue_db.size() >= MAX_NOF_UES) {
    logger.error("Can't add new UE. Max number of UEs reached.");
    return nullptr;
  }

  ue_index_t new_idx = get_next_ue_index();
  if (new_idx == INVALID_UE_INDEX) {
    logger.error("No free ue_index available");
    return nullptr;
  }

  // Create UE executors
  // TODO, these should be created within the same function, so that UL, DL and CTRL executors
  // can point to the same executor.
  std::unique_ptr<ue_executor_mapper> ue_exec_mapper = exec_pool.create_ue_executor_mapper();

  // Create executor-specific timer factories
  timer_factory ue_dl_timer_factory   = {timers, ue_exec_mapper->dl_pdu_executor()};
  timer_factory ue_ul_timer_factory   = {timers, ue_exec_mapper->ul_pdu_executor()};
  timer_factory ue_ctrl_timer_factory = {timers, ue_exec_mapper->ctrl_executor()};

  // Create UE object
  std::unique_ptr<ue_context> new_ctx = std::make_unique<ue_context>(new_idx,
                                                                     ue_cfg,
                                                                     e1ap,
                                                                     n3_config,
                                                                     test_mode_config,
                                                                     std::move(ue_exec_mapper),
                                                                     ue_task_schedulers[new_idx],
                                                                     ue_dl_timer_factory,
                                                                     ue_ul_timer_factory,
                                                                     ue_ctrl_timer_factory,
                                                                     f1u_gw,
                                                                     ngu_session_mngr,
                                                                     n3_teid_allocator,
                                                                     f1u_teid_allocator,
                                                                     gtpu_rx_demux,
                                                                     gtpu_pcap);

  // add to DB
  ue_db.emplace(new_idx, std::move(new_ctx));
  return ue_db[new_idx].get();
}

async_task<void> ue_manager::remove_ue(ue_index_t ue_index)
{
  logger.debug("ue={}: Scheduling UE deletion", fmt::underlying(ue_index));
  srsran_assert(
      ue_db.find(ue_index) != ue_db.end(), "Remove UE called for nonexistent ue_index={}", fmt::underlying(ue_index));

  // Move UE context out from ue_db and erase the slot (from CU-UP shared ctrl executor)
  std::unique_ptr<ue_context> ue_ctxt = std::move(ue_db[ue_index]);
  ue_db.erase(ue_index);

  // Dispatch the stopping and deletion of the UE context to UE-specific ctrl executor
  return launch_async([this, ue_ctxt = std::move(ue_ctxt)](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    CORO_AWAIT(ue_ctxt->stop(ctrl_executor, timers));

    CORO_RETURN();
  });
}

ue_index_t ue_manager::get_next_ue_index()
{
  // Search unallocated UE index
  for (int i = 0; i < MAX_NOF_UES; i++) {
    if (ue_db.find(static_cast<ue_index_t>(i)) == ue_db.end()) {
      return int_to_ue_index(i);
      break;
    }
  }
  return INVALID_UE_INDEX;
}

void ue_manager::schedule_ue_async_task(ue_index_t ue_index, async_task<void> task)
{
  ue_context* ue_ctx = find_ue(ue_index);
  if (ue_ctx == nullptr) {
    logger.error("Cannot schedule UE task, could not find UE. ue_index={}", fmt::underlying(ue_index));
    return;
  }
  ue_ctx->task_sched.schedule(std::move(task));
}

async_task<bool> ue_manager::schedule_and_wait_ue_removal(ue_index_t ue_index)
{
  ue_context* ue_ctx = find_ue(ue_index);
  if (ue_ctx == nullptr) {
    logger.error("Cannot schedule UE removal, could not find UE. ue_index={}", fmt::underlying(ue_index));
    return launch_async([](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(false);
    });
  }

  auto fn = [this, ue_index](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT(remove_ue(ue_index));
    CORO_RETURN();
  };

  return when_coroutine_completed_on_task_sched(ue_ctx->task_sched, std::move(fn));
}
