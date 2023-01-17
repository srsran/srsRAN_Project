/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "task_executor.h"
#include "srsgnb/adt/blocking_queue.h"
#include "srsgnb/adt/unique_function.h"
#include "srsgnb/srslog/srslog.h"
#include "srsgnb/support/unique_thread.h"
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

namespace srsgnb {

/// Class used to create a single worker with an input task queue with a single reader
class task_worker
{
  using task_t = unique_task;

public:
  task_worker(std::string                      thread_name_,
              uint32_t                         queue_size,
              bool                             start_postponed = false,
              os_thread_realtime_priority      prio_           = os_thread_realtime_priority::NO_REALTIME,
              const os_sched_affinity_bitmask& mask_           = {});
  task_worker(const task_worker&)            = delete;
  task_worker(task_worker&&)                 = delete;
  task_worker& operator=(const task_worker&) = delete;
  task_worker& operator=(task_worker&&)      = delete;
  ~task_worker();

  /// Stop task worker, if running.
  void stop();

  /// Initialize task worker, if not yet running.
  void start(os_thread_realtime_priority      prio_ = os_thread_realtime_priority::NO_REALTIME,
             const os_sched_affinity_bitmask& mask_ = {});

  void push_task(task_t&& task)
  {
    auto ret = pending_tasks.try_push(std::move(task));
    if (ret.is_error()) {
      logger.error("Cannot push anymore tasks into the {} worker queue. maximum size is {}",
                   worker_name,
                   uint32_t(pending_tasks.max_size()));
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
  os_thread_realtime_priority prio = os_thread_realtime_priority::NO_REALTIME;
  os_sched_affinity_bitmask   mask = {};
  srslog::basic_logger&       logger;

  // Queue of tasks.
  srsgnb::blocking_queue<task_t> pending_tasks;

  unique_thread t_handle;
};

/// Executor for single-thread task worker.
class task_worker_executor : public task_executor
{
public:
  task_worker_executor() = default;
  task_worker_executor(task_worker& worker_) : worker(&worker_) {}
  void execute(unique_task task) override
  {
    if (worker->get_id() == std::this_thread::get_id()) {
      // Same thread. Run task right away.
      task();
    } else {
      worker->push_task(std::move(task));
    }
  }
  void defer(unique_task task) override { worker->push_task(std::move(task)); }

private:
  task_worker* worker = nullptr;
};

inline std::unique_ptr<task_executor> make_task_executor(task_worker& w)
{
  return std::make_unique<task_worker_executor>(task_worker_executor{w});
}

} // namespace srsgnb
