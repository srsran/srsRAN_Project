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

#include "cu_up_task_scheduler.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_up_task_scheduler::cu_up_task_scheduler(timer_manager&        timers_,
                                           task_executor&        exec_,
                                           srslog::basic_logger& logger_) :
  timers(timers_), exec(exec_), logger(logger_)
{
  // init cu_up control loops
  const size_t number_of_pending_cu_up_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_CU_UPS; ++i) {
    cu_up_ctrl_loop.emplace(i, number_of_pending_cu_up_procedures);
  }
}

// UE task scheduler
void cu_up_task_scheduler::handle_cu_up_async_task(cu_up_index_t cu_up_index, async_task<void>&& task)
{
  logger.debug("cu-up={} Scheduling async task", cu_up_index);
  cu_up_ctrl_loop[cu_up_index_to_uint(cu_up_index)].schedule(std::move(task));
}

unique_timer cu_up_task_scheduler::make_unique_timer()
{
  return timers.create_unique_timer(exec);
}
timer_manager& cu_up_task_scheduler::get_timer_manager()
{
  return timers;
}
