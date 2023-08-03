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

#include "srsran/support/executors/task_executor.h"
#include <condition_variable>
#include <mutex>

namespace srsran {

template <typename Exec>
class sync_task_executor final : public task_executor
{
  struct setter_deleter {
    sync_task_executor* parent;

    setter_deleter(sync_task_executor* parent_) : parent(parent_) {}
    void operator()(bool* p)
    {
      if (p != nullptr) {
        std::unique_lock<std::mutex> lock(parent->mutex);
        parent->done = true;
        parent->cvar.notify_one();
      }
    }
  };

public:
  explicit sync_task_executor(Exec exec_) : exec(std::move(exec_)) {}
  bool execute(unique_task task) override
  {
    done = false;
    std::unique_ptr<bool, setter_deleter> unique_setter(&done, setter_deleter{this});
    bool ret = get(exec).execute([task = std::move(task), token = std::move(unique_setter)]() { task(); });

    // wait for condition variable to be set.
    std::unique_lock<std::mutex> lock(mutex);
    while (not done) {
      cvar.wait(lock);
    }

    return ret;
  }
  bool defer(unique_task task) override { return execute(std::move(task)); }

private:
  template <typename U>
  U& get(U* u)
  {
    return *u;
  }

  template <typename U>
  U& get(std::unique_ptr<U>& u)
  {
    return *u;
  }

  template <typename U>
  U& get(U& u)
  {
    return u;
  }

  Exec exec;

  std::mutex              mutex;
  std::condition_variable cvar;
  bool                    done = false;
};

template <typename Exec>
std::unique_ptr<task_executor> make_sync_executor(Exec&& exec)
{
  return std::make_unique<sync_task_executor<Exec>>(std::forward<Exec>(exec));
}

} // namespace srsran
