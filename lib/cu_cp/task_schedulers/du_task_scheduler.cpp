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

#include "du_task_scheduler.h"

using namespace srsran;
using namespace srs_cu_cp;

du_task_scheduler::du_task_scheduler(timer_manager&        timers_,
                                     task_executor&        exec_,
                                     uint16_t              max_nof_dus,
                                     srslog::basic_logger& logger_) :
  timers(timers_), exec(exec_), logger(logger_)
{
  // init du control loops
  const size_t number_of_pending_du_procedures = 16;
  for (size_t i = 0; i < max_nof_dus; ++i) {
    du_ctrl_loop.emplace(uint_to_du_index(i), number_of_pending_du_procedures);
  }
}

// UE task scheduler
void du_task_scheduler::handle_du_async_task(du_index_t du_index, async_task<void>&& task)
{
  logger.debug("du={}: Scheduling async task", du_index);
  du_ctrl_loop.at(du_index).schedule(std::move(task));
}

unique_timer du_task_scheduler::make_unique_timer()
{
  return timers.create_unique_timer(exec);
}
timer_manager& du_task_scheduler::get_timer_manager()
{
  return timers;
}
