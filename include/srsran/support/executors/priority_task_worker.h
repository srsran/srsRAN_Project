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
#include "srsran/adt/detail/tuple_utils.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/unique_thread.h"

namespace srsran {

/// \brief Priority level of an enqueued task in \c priority_multiqueue_task_worker.
using task_priority = enqueue_priority;

/// \brief Task worker that can handle tasks with different levels of priority.
///
/// Each priority level gets associated a separate task queue with queueing policy defined by
/// \c concurrent_queue_policy. The task worker will pop tasks starting from the highest priority queue and will
/// continue to the next priority level if the current queue is empty. If there are no tasks in any of the queues, the
/// task worker will wait for \c wait_interval before checking for new tasks.
///
/// \tparam QueuePolicies Queue policies for each priority level. The number of policies must match the number of
/// supported priority levels.
template <concurrent_queue_policy... QueuePolicies>
class priority_task_worker
{
public:
  /// \brief Construct a new priority multiqueue task worker object.
  /// \param thread_name The name of the thread used by the task worker.
  /// \param task_queue_sizes The sizes of the task queues for each priority level.
  /// \param wait_interval The amount of time to suspend the thread, when no tasks are pending.
  /// \param prio task worker OS thread priority level.
  /// \param mask thread OS affinity bitmask.
  priority_task_worker(std::string                                           thread_name,
                       const std::array<unsigned, sizeof...(QueuePolicies)>& task_queue_sizes,
                       std::chrono::microseconds                             wait_interval,
                       os_thread_realtime_priority      prio = os_thread_realtime_priority::no_realtime(),
                       const os_sched_affinity_bitmask& mask = {}) :
    task_queue(task_queue_sizes, wait_interval), t_handle(thread_name, prio, mask, [this]() { run_pop_task_loop(); })
  {
  }
  ~priority_task_worker() { stop(); }

  /// Stop task worker, if running.
  void stop()
  {
    if (t_handle.running()) {
      task_queue.request_stop();
      t_handle.join();
    }
  }

  /// \brief Push task to task queue with priority level \c Priority (lower integer represents a higher priority).
  template <task_priority Priority>
  SRSRAN_NODISCARD bool push_task(unique_task task)
  {
    return task_queue.template try_push<Priority>(std::move(task));
  }

  /// \brief Get specified priority task queue capacity.
  template <task_priority Priority>
  SRSRAN_NODISCARD size_t queue_capacity() const
  {
    return task_queue.template capacity<Priority>();
  }

  /// Get worker thread id.
  std::thread::id get_id() const { return t_handle.get_id(); }

  /// Get worker thread name.
  const char* worker_name() const { return t_handle.get_name(); }

  /// Number of priority levels supported by this worker.
  static constexpr size_t nof_priority_levels() { return sizeof...(QueuePolicies); }

private:
  void run_pop_task_loop()
  {
    while (task_queue.call_on_pop_blocking([](unique_task& task) { task(); })) {
    }
    srslog::fetch_basic_logger("ALL").info("Task worker \"{}\" finished.", this_thread_name());
  }

  // Task queues with different priorities. The first queue is the highest priority queue.
  concurrent_priority_queue<unique_task, QueuePolicies...> task_queue;

  // Worker thread
  unique_thread t_handle;
};

/// \brief Task executor with \c Priority (lower is higher) for \c priority_multiqueue_task_worker.
template <task_priority Priority, concurrent_queue_policy... QueuePolicies>
class priority_task_worker_executor : public task_executor
{
public:
  constexpr static task_priority priority_level = Priority;

  priority_task_worker_executor(priority_task_worker<QueuePolicies...>& worker_, bool report_on_push_failure = true) :
    worker(&worker_), report_on_failure(report_on_push_failure)
  {
  }

  SRSRAN_NODISCARD bool execute(unique_task task) override
  {
    if (Priority == task_priority::max and worker->get_id() == std::this_thread::get_id()) {
      // If same thread and highest priority task, run task right away.
      task();
      return true;
    }
    return defer(std::move(task));
  }

  SRSRAN_NODISCARD bool defer(unique_task task) override
  {
    if (not worker->template push_task<Priority>(std::move(task))) {
      if (report_on_failure) {
        srslog::fetch_basic_logger("ALL").warning(
            "Cannot push more tasks into the {} worker queue with priority={}. Maximum size is {}",
            worker->worker_name(),
            Priority,
            worker->template queue_capacity<Priority>());
      }
      return false;
    }
    return true;
  }

private:
  priority_task_worker<QueuePolicies...>* worker            = nullptr;
  bool                                    report_on_failure = true;
};

/// \brief Create task executor with \c Priority for \c priority_multiqueue_task_worker.
template <task_priority Priority, concurrent_queue_policy... QueuePolicies>
priority_task_worker_executor<Priority, QueuePolicies...>
make_priority_task_worker_executor(priority_task_worker<QueuePolicies...>& worker)
{
  return priority_task_worker_executor<Priority, QueuePolicies...>(worker);
}

/// \brief Create general task executor pointer with \c Priority for \c priority_multiqueue_task_worker.
template <task_priority Priority, concurrent_queue_policy... QueuePolicies>
std::unique_ptr<task_executor> make_priority_task_executor_ptr(priority_task_worker<QueuePolicies...>& worker)
{
  return std::make_unique<priority_task_worker_executor<Priority, QueuePolicies...>>(worker);
}

namespace detail {

template <typename Func, concurrent_queue_policy... QueuePolicies, size_t... Is>
void visit_executor(priority_task_worker<QueuePolicies...>& w,
                    task_priority                           priority,
                    const Func&                             func,
                    std::index_sequence<Is...> /*unused*/)
{
  const size_t idx = detail::enqueue_priority_to_queue_index(priority, sizeof...(QueuePolicies));
  (void)std::initializer_list<int>{[idx, &w, &func]() {
    if (idx == Is) {
      func(
          make_priority_task_worker_executor<detail::queue_index_to_enqueue_priority(Is, sizeof...(QueuePolicies))>(w));
    }
    return 0;
  }()...};
}

} // namespace detail

/// \brief Create general task executor pointer with \c Priority for \c priority_multiqueue_task_worker.
template <typename Func, concurrent_queue_policy... QueuePolicies>
void visit_executor(priority_task_worker<QueuePolicies...>& worker, task_priority priority, const Func& func)
{
  detail::visit_executor(worker, priority, func, std::make_index_sequence<sizeof...(QueuePolicies)>{});
}

/// \brief Create general task executor pointer with \c Priority for \c priority_multiqueue_task_worker.
template <concurrent_queue_policy... QueuePolicies>
std::unique_ptr<task_executor> make_priority_task_executor_ptr(priority_task_worker<QueuePolicies...>& worker,
                                                               task_priority                           priority)
{
  std::unique_ptr<task_executor> exec;
  detail::visit_executor(
      worker,
      priority,
      [&exec](const auto& e) { exec = std::make_unique<decltype(e)>(e); },
      std::make_index_sequence<sizeof...(QueuePolicies)>{});
  return exec;
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
