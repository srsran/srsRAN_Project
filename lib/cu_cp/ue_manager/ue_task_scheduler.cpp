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

#include "ue_task_scheduler.h"
#include "srsran/cu_cp/cu_cp_types.h"

using namespace srsran;
using namespace srs_cu_cp;

/// Maximum number of pending procedures per UE.
static const size_t number_of_pending_ue_procedures = 64;

unique_timer ue_task_scheduler::create_timer()
{
  srsran_assert(parent != nullptr, "UE task scheduler not set");
  return parent->make_unique_timer();
}

timer_factory ue_task_scheduler::get_timer_factory()
{
  return timer_factory{parent->timers, parent->exec};
}

void ue_task_scheduler::stop()
{
  if (parent != nullptr) {
    parent->rem_ue_task_loop(ue_sched->first);
    parent = nullptr;
  }
}

ue_task_scheduler_manager::ue_task_scheduler_manager(timer_manager&        timers_,
                                                     task_executor&        exec_,
                                                     srslog::basic_logger& logger_) :
  timers(timers_), exec(exec_), logger(logger_), ues_to_rem(16)
{
}

void ue_task_scheduler_manager::stop()
{
  for (auto& p : ue_ctrl_loop) {
    auto ctrl_loop = p.second->request_stop();

    // let loop go out of scope.
  }
  ues_to_rem.request_stop();
}

ue_task_scheduler ue_task_scheduler_manager::create_ue_task_sched(ue_index_t ue_idx)
{
  if (ue_ctrl_loop.find(ue_idx) != ue_ctrl_loop.end()) {
    logger.error("ue={}: UE task scheduler already exists", ue_idx);
    return {};
  }
  auto ret = ue_ctrl_loop.emplace(ue_idx, std::make_unique<fifo_async_task_scheduler>(number_of_pending_ue_procedures));
  return ue_task_scheduler{*this, *ret.first};
}

void ue_task_scheduler_manager::clear_pending_tasks(ue_index_t ue_index)
{
  logger.debug("ue={}: Clearing pending tasks", ue_index);
  ue_ctrl_loop.at(ue_index)->clear_pending_tasks();
}

// UE task scheduler
void ue_task_scheduler_manager::handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task)
{
  logger.debug("ue={}: Scheduling async task", ue_index);
  ue_ctrl_loop.at(ue_index)->schedule(std::move(task));
}

async_task<bool> ue_task_scheduler_manager::dispatch_and_await_task_completion(ue_index_t ue_index, unique_task task)
{
  return when_completed_on_task_sched(*ue_ctrl_loop.at(ue_index), std::move(task));
}

unique_timer ue_task_scheduler_manager::make_unique_timer()
{
  return timers.create_unique_timer(exec);
}
timer_manager& ue_task_scheduler_manager::get_timer_manager()
{
  return timers;
}

void ue_task_scheduler_manager::rem_ue_task_loop(ue_index_t ue_idx)
{
  auto it = ue_ctrl_loop.find(ue_idx);
  if (it == ue_ctrl_loop.end()) {
    logger.warning("ue={}: UE task scheduler not found", ue_idx);
    return;
  }

  // Given that it might be a UE coroutine that calls the removal of the UE object, we defer the destruction of the UE
  // task scheduler to a separate task loop.
  //  eager_async_task<void> ue_task_loop = it->second.request_stop();
  ues_to_rem.schedule(launch_async([ue_sched = std::move(it->second)](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Cancel pending UE tasks and stop the task loop.
    CORO_AWAIT(ue_sched->request_stop());

    CORO_RETURN();
  }));

  // Remove UE and free its index.
  ue_ctrl_loop.erase(it);
}
