/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
