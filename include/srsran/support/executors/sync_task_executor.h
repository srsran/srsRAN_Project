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

#include "srsran/support/executors/task_executor.h"
#include <condition_variable>
#include <mutex>

namespace srsran {

/// \brief Blocks the thread calling execute() or defer() until the execution of the pushed task has been completed.
/// \note This class is intended to be used by tests and not by production code.
template <typename Executor>
class sync_task_executor final : public task_executor
{
  Executor executor;

public:
  template <typename E>
  explicit sync_task_executor(E&& executor_) : executor(std::forward<E>(executor_))
  {
  }

  bool execute(unique_task task) override
  {
    std::mutex              mutex;
    std::condition_variable cvar;
    bool                    done = false;

    bool ret = get(executor).execute([&mutex, &cvar, &done, task = std::move(task)]() {
      task();

      mutex.lock();
      done = true;
      cvar.notify_one();
      mutex.unlock();
    });

    // Nothing to wait for if the task was not enqueued.
    if (not ret) {
      return false;
    }

    // Wait for the condition variable to be set.
    std::unique_lock<std::mutex> lock(mutex);
    while (not done) {
      cvar.wait(lock);
    }

    return ret;
  }

  bool defer(unique_task task) override { return execute(std::move(task)); }

private:
  template <typename U>
  static U& get(U* u)
  {
    return *u;
  }

  template <typename U>
  static U& get(std::unique_ptr<U>& u)
  {
    return *u;
  }

  template <typename U>
  static U& get(U& u)
  {
    return u;
  }
};

template <typename Executor>
std::unique_ptr<task_executor> make_sync_executor(Executor&& executor)
{
  return std::make_unique<sync_task_executor<Executor>>(std::forward<Executor>(executor));
}

/// \brief Forces the dispatch of a task and its execution to completion.
///
/// The user should be only use this call in exceptional situations where a blocking call is required.
template <typename Exec, typename Task, typename OnTaskDispatchFailure>
void force_blocking_execute(Exec&&                  exec,
                            Task&&                  task,
                            OnTaskDispatchFailure&& fail_func,
                            unsigned                max_attempts = std::numeric_limits<unsigned>::max())
{
  static_assert(std::is_copy_constructible<Task>::value, "Task must be copy assignable");

  // Create a sync executor that will block the thread until the task is executed.
  sync_task_executor<Exec> sync_task_exec(std::forward<Exec>(exec));

  for (unsigned count = 0; count < max_attempts and not sync_task_exec.execute(task); ++count) {
    fail_func();
  }
}

} // namespace srsran
