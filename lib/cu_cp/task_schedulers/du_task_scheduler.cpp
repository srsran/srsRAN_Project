/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_task_scheduler.h"

using namespace srsran;
using namespace srs_cu_cp;

du_task_scheduler::du_task_scheduler(timer_manager& timers_, task_executor& exec_) : timers(timers_), exec(exec_)
{
  // init du control loops
  const size_t number_of_pending_du_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_DUS; ++i) {
    du_ctrl_loop.emplace(i, number_of_pending_du_procedures);
  }
}

// UE task scheduler
void du_task_scheduler::handle_du_async_task(du_index_t du_index, async_task<void>&& task)
{
  du_ctrl_loop[du_index_to_uint(du_index)].schedule(std::move(task));
}

unique_timer du_task_scheduler::make_unique_timer()
{
  return timers.create_unique_timer(exec);
}
timer_manager& du_task_scheduler::get_timer_manager()
{
  return timers;
}
