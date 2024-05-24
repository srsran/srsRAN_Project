/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

/// \brief Async FIFO task scheduler for a single UE managed by the CU-CP.
class ue_task_scheduler
{
  using ue_element = std::pair<const ue_index_t, std::unique_ptr<fifo_async_task_scheduler>>;

public:
  virtual ~ue_task_scheduler() = default;

  /// \brief Schedules an async task for the given UE. The task will run after the remaining tasks in the queue are
  /// completed.
  ///
  /// \param task Task to schedule
  /// \return true if the task was successfully enqueued. False, otherwise.
  virtual bool schedule_async_task(async_task<void> task) = 0;

  /// Create a timer for the UE.
  virtual unique_timer create_timer() = 0;

  virtual timer_factory get_timer_factory() = 0;

  virtual task_executor& get_executor() = 0;

  /// \brief Stops the UE task scheduler and discards all the pending tasks that haven't started running yet.
  virtual void stop() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
