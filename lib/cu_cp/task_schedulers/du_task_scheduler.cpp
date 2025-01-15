/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
