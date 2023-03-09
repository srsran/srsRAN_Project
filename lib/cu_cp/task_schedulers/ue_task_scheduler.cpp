/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_task_scheduler.h"

using namespace srsran;
using namespace srs_cu_cp;

ue_task_scheduler::ue_task_scheduler(timer_manager2& timers_, task_executor& exec_) : timers(timers_), exec(exec_)
{
  // init ue control loops
  const size_t number_of_pending_ue_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_CU_UES; ++i) {
    ue_ctrl_loop.emplace(uint_to_ue_index(i), number_of_pending_ue_procedures);
  }
}

// UE task scheduler
void ue_task_scheduler::handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task)
{
  ue_ctrl_loop.at(ue_index).schedule(std::move(task));
}

unique_timer2 ue_task_scheduler::make_unique_timer()
{
  return timers.create_unique_timer(exec);
}
timer_manager2& ue_task_scheduler::get_timer_manager()
{
  return timers;
}
