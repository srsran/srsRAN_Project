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

#include "detail/priority_task_queue.h"
#include "unique_thread.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// \brief Priority level of an enqueued task in \c priority_multiqueue_task_worker.
using task_priority = enqueue_priority;

/// \brief Task worker that can handle tasks with different levels of priority.
///
/// Each priority level gets associated a separate task queue with queueing policy defined by
/// \c concurrent_queue_policy. The task worker will pop tasks starting from the highest priority queue and will
/// continue to the next priority level if the current queue is empty. If there are no tasks in any of the queues, the
/// task worker will wait for \c wait_interval before checking for new tasks.
class priority_task_worker
{
public:
  /// \brief Construct a new priority multiqueue task worker object.
  /// \param thread_name The name of the thread used by the task worker.
  /// \param task_queue_sizes The sizes of the task queues for each priority level.
  /// \param wait_interval The amount of time to suspend the thread, when no tasks are pending.
  /// \param prio task worker OS thread priority level.
  /// \param mask thread OS affinity bitmask.
  priority_task_worker(std::string                         thread_name,
                       span<const concurrent_queue_params> task_queue_params,
                       std::chrono::microseconds           wait_interval,
                       os_thread_realtime_priority         prio = os_thread_realtime_priority::no_realtime(),
                       const os_sched_affinity_bitmask&    mask = {});

  ~priority_task_worker() { stop(); }

  /// \brief Push task to task queue with priority level \c priority (lower integer represents a higher priority).
  SRSRAN_NODISCARD bool push_task(task_priority prio, unique_task task)
  {
    return task_queue.try_push(prio, std::move(task));
  }

  /// Get worker thread id.
  std::thread::id get_id() const { return t_handle.get_id(); }

  /// Get worker thread name.
  const char* worker_name() const { return t_handle.get_name(); }

  /// \brief Get specified priority task queue capacity.
  SRSRAN_NODISCARD size_t queue_capacity(task_priority prio) const { return task_queue.queue_capacity(prio); }

  /// Number of priority levels supported by this worker.
  size_t nof_priority_levels() const { return task_queue.nof_priority_levels(); }

  /// Stop task worker, if running.
  void stop();

private:
  /// \brief Task run in instantiated thread. It will keep popping tasks until the worker is stopped.
  ///
  /// The prioritization is achieved via multiple queues. The pop functions will always start with the highest priority
  /// queue until it is depleted, and then move to the second highest priority queue, and so on.
  void run_pop_task_loop();

  // Task queues with different priorities. The first queue is the highest priority queue.
  detail::priority_task_queue task_queue;

  // Worker thread
  unique_thread t_handle;
};

/// \brief Task executor with a predefined priority (lower is higher) for \c priority_task_worker.
class priority_task_worker_executor : public task_executor
{
public:
  priority_task_worker_executor(task_priority prio_, priority_task_worker& worker_) : prio(prio_), worker(worker_) {}

  SRSRAN_NODISCARD bool execute(unique_task task) override
  {
    if (can_run_task_inline()) {
      // If same thread and highest priority task, run task right away.
      task();
      return true;
    }
    return worker.push_task(prio, std::move(task));
  }

  SRSRAN_NODISCARD bool defer(unique_task task) override { return worker.push_task(prio, std::move(task)); }

  // Check whether task can be run inline or it needs to be dispatched to a queue.
  bool can_run_task_inline() const
  {
    return prio == task_priority::max and worker.get_id() == std::this_thread::get_id();
  }

private:
  /// Enqueuer interface with the provided Priority.
  task_priority         prio;
  priority_task_worker& worker;
};

/// \brief Create task executor with \c priority for \c priority_task_worker.
inline priority_task_worker_executor make_priority_task_worker_executor(task_priority         prio,
                                                                        priority_task_worker& worker)
{
  return {prio, worker};
}

/// \brief Create general task executor pointer with \c priority for \c priority_task_worker.
inline std::unique_ptr<task_executor> make_priority_task_executor_ptr(task_priority prio, priority_task_worker& worker)
{
  return std::make_unique<priority_task_worker_executor>(prio, worker);
}

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::task_priority> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::task_priority& prio, FormatContext& ctx)
  {
    fmt::format_to(ctx.out(), "max");
    if (prio != srsran::task_priority::max) {
      fmt::format_to(ctx.out(), "-{}", static_cast<size_t>(srsran::task_priority::max) - static_cast<size_t>(prio));
    }
    return ctx.out();
  }
};

} // namespace fmt
