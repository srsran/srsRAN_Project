/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/slotted_array.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

/// \brief Service provided by CU-CP to schedule async tasks for a given UE.
class ue_task_scheduler
{
public:
  explicit ue_task_scheduler(timer_manager& timers_, task_executor& exec_, srslog::basic_logger& logger_);
  ~ue_task_scheduler() = default;

  // UE task scheduler
  void handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task);

  void clear_pending_tasks(ue_index_t ue_index);

  async_task<bool> dispatch_and_await_task_completion(ue_index_t ue_index, unique_task task);

  unique_timer   make_unique_timer();
  timer_manager& get_timer_manager();

private:
  timer_manager&        timers;
  task_executor&        exec;
  srslog::basic_logger& logger;

  // task event loops indexed by ue_index
  std::unordered_map<ue_index_t, fifo_async_task_scheduler> ue_ctrl_loop;
};

} // namespace srs_cu_cp
} // namespace srsran
