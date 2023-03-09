/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/slotted_array.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_task_loop.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/timers2.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Service provided by CU-CP to schedule async tasks for a given DU.
class du_task_scheduler
{
public:
  explicit du_task_scheduler(timer_manager& timers_, task_executor& exec_);
  ~du_task_scheduler() = default;

  // CU-UP task scheduler
  void handle_du_async_task(du_index_t du_index, async_task<void>&& task);

  unique_timer   make_unique_timer();
  timer_manager& get_timer_manager();

private:
  timer_manager& timers;
  task_executor& exec;

  // task event loops indexed by du_index
  slotted_array<async_task_sequencer, MAX_NOF_DUS> du_ctrl_loop;
};

} // namespace srs_cu_cp
} // namespace srsran
