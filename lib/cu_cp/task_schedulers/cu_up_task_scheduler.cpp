/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_task_scheduler.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_up_task_scheduler::cu_up_task_scheduler(timer_manager& timers_, task_executor& exec_) : timers(timers_), exec(exec_)
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
