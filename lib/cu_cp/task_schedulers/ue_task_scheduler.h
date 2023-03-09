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
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

/// \brief Service provided by CU-CP to schedule async tasks for a given UE.
class ue_task_scheduler
{
public:
  explicit ue_task_scheduler(timer_manager& timers_, task_executor& exec_);
  ~ue_task_scheduler() = default;

  // UE task scheduler
  void handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task);

  unique_timer   make_unique_timer();
  timer_manager& get_timer_manager();

private:
  timer_manager& timers;
  task_executor& exec;

  // task event loops indexed by ue_index
  std::unordered_map<ue_index_t, async_task_sequencer> ue_ctrl_loop;
};

} // namespace srs_cu_cp
} // namespace srsran
