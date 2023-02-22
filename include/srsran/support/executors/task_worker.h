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

#include "task_executor.h"
#include "srsran/adt/blocking_queue.h"
#include "srsran/adt/unique_function.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/unique_thread.h"
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <stack>
#include <string>
#include <vector>

namespace srsran {

/// Class used to create a single worker with an input task queue with a single reader
class task_worker
{
  using task_t = unique_task;

public:
  task_worker(std::string                      thread_name_,
              uint32_t                         queue_size,
              bool                             start_postponed = false,
              os_thread_realtime_priority      prio_           = os_thread_realtime_priority::no_realtime(),
              const os_sched_affinity_bitmask& mask_           = {});
  task_worker(const task_worker&)            = delete;
  task_worker(task_worker&&)                 = delete;
  task_worker& operator=(const task_worker&) = delete;
  task_worker& operator=(task_worker&&)      = delete;
  ~task_worker();

  /// Stop task worker, if running.
  void stop();

  /// Initialize task worker, if not yet running.
  void start(os_thread_realtime_priority      prio_ = os_thread_realtime_priority::no_realtime(),
             const os_sched_affinity_bitmask& mask_ = {});

  bool push_task(task_t&& task, bool log_on_failure = true)
  {
    auto ret = pending_tasks.try_push(std::move(task));
    if (ret.is_error()) {
      if (log_on_failure) {
        logger.error("Cannot push anymore tasks into the {} worker queue. maximum size is {}",
                     worker_name,
                     uint32_t(pending_tasks.max_size()));
      }
      return false;
    }
    return true;
  }

  void push_task_blocking(task_t&& task)
  {
    auto ret = pending_tasks.push_blocking(std::move(task));
    if (ret.is_error()) {
      logger.debug("Cannot push anymore tasks into the {} worker queue because it was closed", worker_name);
      return;
    }
  }

  /// \brief Wait for all the currently enqueued tasks to complete.
  void wait_pending_tasks()
  {
    std::packaged_task<void()> pkg_task([]() { /* do nothing */ });
    std::future<void>          fut = pkg_task.get_future();
    push_task(std::move(pkg_task));
    // blocks for enqueued task to complete.
    fut.get();
  }

  uint32_t nof_pending_tasks() const { return pending_tasks.size(); }

  std::thread::id get_id() const { return t_handle.get_id(); }

private:
  unique_thread make_thread();

  // args
  std::string                 worker_name;
  os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime();
  os_sched_affinity_bitmask   mask = {};
  srslog::basic_logger&       logger;

  // Queue of tasks.
  srsran::blocking_queue<task_t> pending_tasks;

  unique_thread t_handle;
};

/// Executor for single-thread task worker.
class task_worker_executor : public task_executor
{
public:
  task_worker_executor() = default;

  task_worker_executor(task_worker& worker_, bool report_on_push_failure = true) :
    worker(&worker_), report_on_failure(report_on_push_failure)
  {
  }

  bool execute(unique_task task) override
  {
    if (worker->get_id() == std::this_thread::get_id()) {
      // Same thread. Run task right away.
      task();
      return true;
    }
    return worker->push_task(std::move(task), report_on_failure);
  }

  bool defer(unique_task task) override { return worker->push_task(std::move(task), report_on_failure); }

private:
  task_worker* worker            = nullptr;
  bool         report_on_failure = true;
};

inline std::unique_ptr<task_executor> make_task_executor(task_worker& w)
{
  return std::make_unique<task_worker_executor>(task_worker_executor{w});
}

} // namespace srsran
