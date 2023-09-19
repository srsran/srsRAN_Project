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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/adt/unique_function.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/compiler.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/unique_thread.h"

namespace srsran {

/// \brief Single thread worker with a locking MPSC input task queue. This worker type is ideal for the cases where
/// there is low contention between task producers.
template <concurrent_queue_policy      QueuePolicy = concurrent_queue_policy::locking_mpsc,
          concurrent_queue_wait_policy WaitPolicy  = concurrent_queue_wait_policy::condition_variable>
class general_task_worker
{
  using task_t = unique_task;

public:
  /// \brief Creates a task worker instance that uses a condition variable to notify pushes of new tasks.
  ///
  /// \param thread_name Name of the thread instantiated by this task worker.
  /// \param queue_size Number of pending tasks that this task worker can hold.
  /// \param prio OS thread realtime priority.
  /// \param mask OS scheduler thread affinity mask.
  template <concurrent_queue_wait_policy W                                               = WaitPolicy,
            std::enable_if_t<W == concurrent_queue_wait_policy::condition_variable, int> = 0>
  general_task_worker(std::string                      thread_name,
                      unsigned                         queue_size,
                      os_thread_realtime_priority      prio = os_thread_realtime_priority::no_realtime(),
                      const os_sched_affinity_bitmask& mask = {}) :
    pending_tasks(queue_size), t_handle(thread_name, prio, mask, make_blocking_pop_task())
  {
  }
  /// \brief Creates a task worker instance that uses a condition variable to notify pushes of new tasks.
  ///
  /// \param thread_name Name of the thread instantiated by this task worker.
  /// \param queue_size Number of pending tasks that this task worker can hold.
  /// \param wait_sleep_time Time the worker spends sleeping when there are no enqueued tasks.
  /// \param prio OS thread realtime priority.
  /// \param mask OS scheduler thread affinity mask.
  template <concurrent_queue_wait_policy W                                  = WaitPolicy,
            std::enable_if_t<W == concurrent_queue_wait_policy::sleep, int> = 0>
  general_task_worker(std::string                      thread_name,
                      unsigned                         queue_size,
                      std::chrono::microseconds        wait_sleep_time,
                      os_thread_realtime_priority      prio = os_thread_realtime_priority::no_realtime(),
                      const os_sched_affinity_bitmask& mask = {}) :
    pending_tasks(queue_size, wait_sleep_time), t_handle(thread_name, prio, mask, make_blocking_pop_task())
  {
  }
  general_task_worker(const general_task_worker&)            = delete;
  general_task_worker(general_task_worker&&)                 = delete;
  general_task_worker& operator=(const general_task_worker&) = delete;
  general_task_worker& operator=(general_task_worker&&)      = delete;
  ~general_task_worker();

  /// Stop task worker, if running.
  void stop();

  /// \brief Push a new task to FIFO to be processed by the task worker. If the task FIFO is full, enqueueing fails.
  /// \return true if task was successfully enqueued. False if task FIFO was full.
  SRSRAN_NODISCARD bool push_task(task_t&& task) { return pending_tasks.try_push(std::move(task)); }

  /// \brief Push a new task to FIFO to be processed by the task worker. If the task FIFO is full, this call blocks,
  /// until the FIFO has space to enqueue the task.
  void push_task_blocking(task_t&& task)
  {
    if (not pending_tasks.push_blocking(std::move(task))) {
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
  unsigned max_pending_tasks() const { return pending_tasks.capacity(); }

  /// Get worker thread id.
  std::thread::id get_id() const { return t_handle.get_id(); }

  /// Get worker thread name.
  const char* worker_name() const { return t_handle.get_name(); }

private:
  unique_function<void()> make_blocking_pop_task();

  // Queue of tasks.
  concurrent_queue<task_t, QueuePolicy, WaitPolicy> pending_tasks;

  // Thread that runs the enqueued tasks.
  unique_thread t_handle;
};

extern template class general_task_worker<concurrent_queue_policy::locking_mpsc,
                                          concurrent_queue_wait_policy::condition_variable>;
extern template class general_task_worker<concurrent_queue_policy::locking_mpsc, concurrent_queue_wait_policy::sleep>;
extern template class general_task_worker<concurrent_queue_policy::locking_mpmc,
                                          concurrent_queue_wait_policy::condition_variable>;
extern template class general_task_worker<concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>;

/// Default task worker type.
using task_worker = general_task_worker<>;

/// Executor for single-thread task worker.
template <concurrent_queue_policy      QueuePolicy = concurrent_queue_policy::locking_mpsc,
          concurrent_queue_wait_policy WaitPolicy  = concurrent_queue_wait_policy::condition_variable>
class general_task_worker_executor : public task_executor
{
public:
  general_task_worker_executor() = default;

  general_task_worker_executor(general_task_worker<QueuePolicy, WaitPolicy>& worker_,
                               bool                                          report_on_push_failure = true) :
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
        srslog::fetch_basic_logger("ALL").warning("Cannot push more tasks into the {} worker queue. Maximum size is {}",
                                                  worker->worker_name(),
                                                  worker->max_pending_tasks());
      }
      return false;
    }
    return true;
  }

private:
  general_task_worker<QueuePolicy, WaitPolicy>* worker            = nullptr;
  bool                                          report_on_failure = true;
};

using task_worker_executor = general_task_worker_executor<>;

template <concurrent_queue_policy QueuePolicy, concurrent_queue_wait_policy WaitPolicy>
inline std::unique_ptr<task_executor> make_task_executor(general_task_worker<QueuePolicy, WaitPolicy>& w)
{
  return std::make_unique<general_task_worker_executor<QueuePolicy, WaitPolicy>>(w);
}

} // namespace srsran
