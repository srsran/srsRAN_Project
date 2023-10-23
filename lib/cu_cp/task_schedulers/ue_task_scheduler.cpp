/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

using namespace srsran;
using namespace srs_cu_cp;

ue_task_scheduler::ue_task_scheduler(timer_manager& timers_, task_executor& exec_, srslog::basic_logger& logger_) :
  timers(timers_), exec(exec_), logger(logger_)
{
  // init ue control loops
  const size_t number_of_pending_ue_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_CU_UES; ++i) {
    ue_ctrl_loop.emplace(uint_to_ue_index(i), number_of_pending_ue_procedures);
  }
}

void ue_task_scheduler::clear_pending_tasks(ue_index_t ue_index)
{
  logger.debug("ue={}: Clearing pending tasks", ue_index);
  ue_ctrl_loop.at(ue_index).clear_pending_tasks();
}

// UE task scheduler
void ue_task_scheduler::handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task)
{
  logger.debug("ue={}: Scheduling async task", ue_index);
  ue_ctrl_loop.at(ue_index).schedule(std::move(task));
}

async_task<bool> ue_task_scheduler::dispatch_and_await_task_completion(ue_index_t ue_index, unique_task task)
{
  return when_completed_on_task_sched(ue_ctrl_loop.at(ue_index), std::move(task));
}

unique_timer ue_task_scheduler::make_unique_timer()
{
  return timers.create_unique_timer(exec);
}
timer_manager& ue_task_scheduler::get_timer_manager()
{
  return timers;
}
