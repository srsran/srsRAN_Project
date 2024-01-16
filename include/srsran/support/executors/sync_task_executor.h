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
    bool ret = get(exec).execute([task = std::move(task), token = std::move(unique_setter)]() mutable {
      task();
      // We manually reset the token here, as the unique_task being popped may be deleted in an undetermined time (it
      // is up to the caller).
      token.reset();
    });

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
