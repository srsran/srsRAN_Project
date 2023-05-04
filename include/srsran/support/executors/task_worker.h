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

#include "srsran/adt/blocking_queue.h"
#include "srsran/adt/unique_function.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/compiler.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/unique_thread.h"

namespace srsran {

/// \brief Single thread worker with a locking MPSC input task queue. This worker type is ideal for the cases where
/// there is low contention between task producers.
class task_worker
{
  using task_t = unique_task;

public:
  /// \brief Creates a task worker instance.
  ///
  /// \param thread_name Name of the thread instantiated by this task worker.
  /// \param queue_size Number of pending tasks that this task worker can hold.
  /// \param prio OS thread realtime priority.
  /// \param mask OS scheduler thread affinity mask.
  /// \param pop_wait_timeout Timeout for popping tasks from the task queue. By default, no timeout is set and
  /// the task worker thread will stay blocked while the queue is empty. If a timeout is set, the wait for tasks to pop
  /// will be a combination of busy and passive waiting. This may help offset some experienced delays associated with
  /// using condition variables.
  task_worker(std::string                      thread_name,
              unsigned                         queue_size,
              os_thread_realtime_priority      prio             = os_thread_realtime_priority::no_realtime(),
              const os_sched_affinity_bitmask& mask             = {},
              std::chrono::microseconds        pop_wait_timeout = std::chrono::microseconds{0});
  task_worker(const task_worker&)            = delete;
  task_worker(task_worker&&)                 = delete;
  task_worker& operator=(const task_worker&) = delete;
  task_worker& operator=(task_worker&&)      = delete;
  ~task_worker();

  /// Stop task worker, if running.
  void stop();

  /// \brief Push a new task to FIFO to be processed by the task worker. If the task FIFO is full, enqueueing fails.
  /// \return true if task was successfully enqueued. False if task FIFO was full.
  SRSRAN_NODISCARD bool push_task(task_t&& task) { return pending_tasks.try_push(std::move(task)).has_value(); }

  /// \brief Push a new task to FIFO to be processed by the task worker. If the task FIFO is full, this call blocks,
  /// until the FIFO has space to enqueue the task.
  void push_task_blocking(task_t&& task)
  {
    auto ret = pending_tasks.push_blocking(std::move(task));
    if (ret.is_error()) {
      srslog::fetch_basic_logger("ALL").debug("Cannot push more tasks into the {} worker queue because it was closed",
                                              t_handle.get_name());
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
    return defer(std::move(task));
  }

  bool defer(unique_task task) override
  {
    if (not worker->push_task(std::move(task))) {
      if (report_on_failure) {
        srslog::fetch_basic_logger("ALL").error("Cannot push more tasks into the {} worker queue. Maximum size is {}",
                                                worker->worker_name(),
                                                worker->max_pending_tasks());
      }
      return false;
    }
    return true;
  }

private:
  task_worker* worker            = nullptr;
  bool         report_on_failure = true;
};

inline std::unique_ptr<task_executor> make_task_executor(task_worker& w)
{
  return std::make_unique<task_worker_executor>(task_worker_executor{w});
}

} // namespace srsran
