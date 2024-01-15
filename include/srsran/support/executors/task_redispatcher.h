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

#include "srsran/adt/unique_function.h"
#include "srsran/support/compiler.h"

namespace srsran {

/// Executes a task n times, where "n" increases with the failures to dispatch the task.
/// Dispatches a task to an executor, and if it fails to dispatch (e.g. due to the task queue being full), it reattempts
/// on the next call.
template <typename Executor>
class task_redispatcher
{
public:
  template <typename Exec, typename CallToExec>
  task_redispatcher(Exec&& executor_, CallToExec&& task) :
    executor(std::forward<Exec>(executor_)), dispatch_task(get_dispatch_task(std::forward<CallToExec>(task)))
  {
  }

  SRSRAN_NODISCARD bool execute()
  {
    if (not dispatch_task(true)) {
      dispatch_fail_count.fetch_add(1U, std::memory_order_relaxed);
      return false;
    }
    return true;
  }

  SRSRAN_NODISCARD bool defer()
  {
    if (not dispatch_task(false)) {
      dispatch_fail_count.fetch_add(1U, std::memory_order_relaxed);
      return false;
    }
    return true;
  }

private:
  template <typename CallToExec>
  void run_task_n_times(CallToExec& task)
  {
    // we invoke the "task" a number of times equal to "1 + failed dispatches"
    task();
    uint32_t remaining_calls = dispatch_fail_count.load(std::memory_order_relaxed);
    while (remaining_calls > 0) {
      for (unsigned i = 0; i != remaining_calls; ++i) {
        task();
      }
      remaining_calls = dispatch_fail_count.fetch_sub(remaining_calls, std::memory_order_acq_rel) - remaining_calls;
    }
  }

  template <typename CallToExec>
  unique_function<bool(bool)> get_dispatch_task(CallToExec&& task)
  {
    return [this, t = std::forward<CallToExec>(task)](bool is_execute) {
      if (is_execute) {
        return executor.execute([this, &t]() { run_task_n_times(t); });
      }
      return executor.defer([this, &t]() { run_task_n_times(t); });
    };
  }

  Executor                    executor;
  unique_function<bool(bool)> dispatch_task;

  std::atomic<uint32_t> dispatch_fail_count{0};
};

} // namespace srsran
