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

using namespace srsgnb;
using namespace srs_cu_cp;

ue_task_scheduler::ue_task_scheduler(timer_manager& timers_) : timers(timers_)
{
  // init ue control loops
  const size_t number_of_pending_ue_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_CU_UES; ++i) {
    ue_ctrl_loop.emplace(i, number_of_pending_ue_procedures);
  }
}

// UE task scheduler
void ue_task_scheduler::handle_ue_async_task(du_index_t du_index, ue_index_t ue_index, async_task<void>&& task)
{
  ue_ctrl_loop[(du_index * MAX_NOF_UES) + ue_index].schedule(std::move(task));
}

unique_timer ue_task_scheduler::make_unique_timer()
{
  return timers.create_unique_timer();
}
timer_manager& ue_task_scheduler::get_timer_manager()
{
  return timers;
}
