/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"

namespace srsran {

/// Execute task in the given task executor. If dispatch fails, postpone execution via timers.
///
/// This function is useful for tasks whose dispatch can never fail.
template <typename TaskExecutor, typename CopyableTask>
void execute_until_success(TaskExecutor& exec, timer_manager& timers, CopyableTask&& task)
{
  static_assert(std::is_copy_constructible_v<CopyableTask>, "CopyableTask must be copyable");

  // Try to dispatch right away.
  if (not exec.execute(task)) {
    // If it fails, resort to timers.
    auto timer = timers.create_unique_timer(exec);
    timer.set(timer_duration{0}, [task](timer_id_t tid) mutable { task(); });
    timer.run();
  }
}

/// Defer task in the given task executor. If dispatch fails, postpone defer.
///
/// This function is useful for tasks whose dispatch can never fail.
template <typename TaskExecutor, typename CopyableTask>
void defer_until_success(TaskExecutor& exec, timer_manager& timers, CopyableTask&& task)
{
  static_assert(std::is_copy_constructible_v<CopyableTask>, "CopyableTask must be copyable");

  // Try to dispatch right away.
  if (not exec.defer(task)) {
    // If it fails, resort to timers.
    auto timer = timers.create_unique_timer(exec);
    timer.set(timer_duration{0}, [task](timer_id_t tid) mutable { task(); });
    timer.run();
  }
}

} // namespace srsran
