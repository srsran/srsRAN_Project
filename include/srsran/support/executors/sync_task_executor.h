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

#include "srsran/support/executors/task_executor.h"
#include <condition_variable>
#include <mutex>

namespace srsran {

/// \brief Blocks the thread calling execute() or defer() until the execution of the pushed task has been completed.
/// \note This class is intended to be used by tests and not by production code.
template <typename Executor>
class sync_task_executor final : public task_executor
{
  Executor                executor;
  std::mutex              mutex;
  std::condition_variable cvar;

public:
  template <typename E>
  explicit sync_task_executor(E&& executor_) : executor(std::forward<E>(executor_))
  {
  }

  bool execute(unique_task task) override
  {
    bool done = false;
    bool ret  = get(executor).execute([this, &done, task = std::move(task)]() {
      task();

      mutex.lock();
      done = true;
      mutex.unlock();
      cvar.notify_one();
    });

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

} // namespace srsran
