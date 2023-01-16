/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_task_scheduler.h"

using namespace srsgnb;
using namespace srs_cu_cp;

cu_cp_task_scheduler::cu_cp_task_scheduler(timer_manager& timers_) : timers(timers_), main_ctrl_loop(128) {}

// UE task scheduler
void cu_cp_task_scheduler::schedule_async_task(async_task<void>&& task)
{
  main_ctrl_loop.schedule(std::move(task));
}

unique_timer cu_cp_task_scheduler::make_unique_timer()
{
  return timers.create_unique_timer();
}

timer_manager& cu_cp_task_scheduler::get_timer_manager()
{
  return timers;
}
