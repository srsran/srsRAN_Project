/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Service provided by CU-CP to schedule async tasks for a given CU-UP.
class cu_up_task_scheduler
{
public:
  explicit cu_up_task_scheduler(timer_manager& timers_);
  ~cu_up_task_scheduler() = default;

  // CU-UP task scheduler
  void handle_cu_up_async_task(cu_up_index_t cu_up_index, async_task<void>&& task);

  unique_timer   make_unique_timer();
  timer_manager& get_timer_manager();

private:
  timer_manager& timers;

  // task event loops indexed by cu_up_index
  slot_array<async_task_sequencer, MAX_NOF_CU_UPS> cu_up_ctrl_loop;
};

} // namespace srs_cu_cp
} // namespace srsgnb
