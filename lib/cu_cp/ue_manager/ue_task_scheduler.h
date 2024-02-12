/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

class ue_task_scheduler_manager;

/// \brief Async FIFO task scheduler for a single UE managed by the CU-CP.
class ue_task_scheduler
{
  using ue_element = std::pair<const ue_index_t, std::unique_ptr<fifo_async_task_scheduler>>;

public:
  ue_task_scheduler() = default;
  ue_task_scheduler(ue_task_scheduler_manager& parent_, ue_element& ue_sched_) : parent(&parent_), ue_sched(&ue_sched_)
  {
  }
  ue_task_scheduler(ue_task_scheduler&& other) noexcept :
    parent(std::exchange(other.parent, nullptr)), ue_sched(std::exchange(other.ue_sched, nullptr))
  {
  }
  ue_task_scheduler& operator=(ue_task_scheduler&& other) noexcept
  {
    stop();
    parent   = std::exchange(other.parent, nullptr);
    ue_sched = std::exchange(other.ue_sched, nullptr);
    return *this;
  }
  ~ue_task_scheduler() { stop(); }

  /// \brief Schedules an async task for the given UE. The task will run after the remaining tasks in the queue are
  /// completed.
  ///
  /// \param task Task to schedule
  /// \return true if the task was successfully enqueued. False, otherwise.
  bool schedule_async_task(async_task<void> task)
  {
    srsran_assert(parent != nullptr, "UE task scheduler not set");
    return ue_sched->second->schedule(std::move(task));
  }

  /// Create a timer for the UE.
  unique_timer create_timer();

  timer_factory get_timer_factory();

  /// \brief Stops the UE task scheduler and discards all the pending tasks that haven't started running yet.
  void stop();

private:
  ue_task_scheduler_manager* parent   = nullptr;
  ue_element*                ue_sched = nullptr;
};

/// \brief Service provided by CU-CP to schedule async tasks for a given UE.
class ue_task_scheduler_manager
{
public:
  explicit ue_task_scheduler_manager(timer_manager& timers_, task_executor& exec_, srslog::basic_logger& logger_);

  void stop();

  ue_task_scheduler create_ue_task_sched(ue_index_t ue_idx);

  // UE task scheduler
  void handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task);

  void clear_pending_tasks(ue_index_t ue_index);

  async_task<bool> dispatch_and_await_task_completion(ue_index_t ue_index, unique_task task);

  unique_timer   make_unique_timer();
  timer_manager& get_timer_manager();

private:
  friend class ue_task_scheduler;

  void rem_ue_task_loop(ue_index_t ue_idx);

  timer_manager&        timers;
  task_executor&        exec;
  srslog::basic_logger& logger;

  // task event loops indexed by ue_index
  std::unordered_map<ue_index_t, std::unique_ptr<fifo_async_task_scheduler>> ue_ctrl_loop;

  fifo_async_task_scheduler ues_to_rem;
};

} // namespace srs_cu_cp
} // namespace srsran
