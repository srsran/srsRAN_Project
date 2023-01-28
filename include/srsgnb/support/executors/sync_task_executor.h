/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {

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
  void execute(unique_task task) override
  {
    fmt::print("-- SLOT IND START\n");
    std::unique_lock<std::mutex> lock(mutex);
    done = false;
    std::unique_ptr<bool, setter_deleter> unique_setter(&done, setter_deleter{this});
    exec->execute([task = std::move(task), token = std::move(unique_setter)]() { task(); });
    while (not done) {
      cvar.wait(lock);
    }
    fmt::print("-- SLOT IND END\n");
  }
  void defer(unique_task task) override { execute(std::move(task)); }

private:
  task_executor* exec = nullptr;

  std::mutex              mutex;
  std::condition_variable cvar;
  bool                    done = false;
};

} // namespace srsgnb
