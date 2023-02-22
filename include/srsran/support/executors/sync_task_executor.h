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

#include "srsran/support/executors/task_executor.h"

namespace srsran {

class sync_task_executor final : public task_executor
{
  struct setter_deleter {
    sync_task_executor* parent;

    setter_deleter(sync_task_executor* parent_) : parent(parent_) {}
    void operator()(bool* p)
    {
      std::unique_lock<std::mutex> lock(parent->mutex);
      parent->done = true;
      parent->cvar.notify_one();
    }
  };

public:
  sync_task_executor() = default;
  sync_task_executor(task_executor& exec_) : exec(&exec_) {}
  bool execute(unique_task task) override
  {
    std::unique_lock<std::mutex> lock(mutex);
    done = false;
    std::unique_ptr<bool, setter_deleter> unique_setter(&done, setter_deleter{this});
    exec->execute([task = std::move(task), token = std::move(unique_setter)]() { task(); });
    while (not done) {
      cvar.wait(lock);
    }
    return true;
  }
  bool defer(unique_task task) override { return execute(std::move(task)); }

private:
  task_executor* exec = nullptr;

  std::mutex              mutex;
  std::condition_variable cvar;
  bool                    done = false;
};

} // namespace srsran
