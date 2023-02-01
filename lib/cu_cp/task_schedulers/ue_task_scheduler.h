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

#include "srsgnb/adt/slotted_array.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/timers.h"
#include <unordered_map>

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Service provided by CU-CP to schedule async tasks for a given UE.
class ue_task_scheduler
{
public:
  explicit ue_task_scheduler(timer_manager& timers_);
  ~ue_task_scheduler() = default;

  // UE task scheduler
  void handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task);

  unique_timer   make_unique_timer();
  timer_manager& get_timer_manager();

private:
  timer_manager& timers;

  // task event loops indexed by ue_index
  std::unordered_map<ue_index_t, async_task_sequencer> ue_ctrl_loop;
};

} // namespace srs_cu_cp
} // namespace srsgnb
