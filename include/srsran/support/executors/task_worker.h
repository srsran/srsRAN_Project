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

/// \brief Single thread worker with a locking MPSC input task queue. This worker type is ideal for the cases where
/// there is low contention between task producers.
class task_worker
{
  using task_t = unique_task;

public:
  task_worker(std::string                      thread_name,
              uint32_t                         queue_size,
              os_thread_realtime_priority      prio = os_thread_realtime_priority::no_realtime(),
              const os_sched_affinity_bitmask& mask = {});
  task_worker(const task_worker&)            = delete;
  task_worker(task_worker&&)                 = delete;
  task_worker& operator=(const task_worker&) = delete;
  task_worker& operator=(task_worker&&)      = delete;
  ~task_worker();

  /// Stop task worker, if running.
  void stop();

  /// \brief Push a new task to FIFO to be processed by the task worker. If the task FIFO is full, enqueueing fails.
  /// \return true if task was successfully enqueued. False if task FIFO was full.
  bool push_task(task_t&& task, bool log_on_failure = true)
  {
    auto ret = pending_tasks.try_push(std::move(task));
    if (ret.is_error()) {
      if (log_on_failure) {
        logger.error("Cannot push more tasks into the {} worker queue. Maximum size is {}",
                     t_handle.get_name(),
                     uint32_t(pending_tasks.max_size()));
      }
      return false;
    }
    return true;
  }

  /// \brief Push a new task to FIFO to be processed by the task worker. If the task FIFO is full, this call blocks,
  /// until the FIFO has space to enqueue the task.
  void push_task_blocking(task_t&& task)
  {
    auto ret = pending_tasks.push_blocking(std::move(task));
    if (ret.is_error()) {
      logger.debug("Cannot push more tasks into the {} worker queue because it was closed", t_handle.get_name());
      return;
    }
  }

  /// \brief Wait for all the currently enqueued tasks to complete.
  void wait_pending_tasks();

  /// Number of pending tasks. It requires locking mutex.
  unsigned nof_pending_tasks() const { return pending_tasks.size(); }

  /// Maximum number of pending tasks the task FIFO can hold.
  unsigned max_pending_tasks() const { return pending_tasks.max_size(); }

  /// Get worker thread id.
  std::thread::id get_id() const { return t_handle.get_id(); }

  /// Get worker thread name.
  const char* worker_name() const { return t_handle.get_name(); }

private:
  // args
  srslog::basic_logger& logger;

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
