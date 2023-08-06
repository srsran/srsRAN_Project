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
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/unique_thread.h"

namespace srsran {

/// \brief Task queue priority used to map to specific queue of the \c priority_multiqueue_task_worker. The higher the
/// priority, the lower its integer value representation.
enum class task_queue_priority : size_t { max = 0, min = std::numeric_limits<size_t>::max() };

/// Reduce priority by \c dec amount.
constexpr task_queue_priority operator-(task_queue_priority lhs, size_t dec)
{
  return (task_queue_priority)((size_t)lhs + dec);
}

namespace detail {

template <size_t N, size_t... Is>
constexpr auto as_tuple(const std::array<unsigned, N>& arr, std::index_sequence<Is...> /*unused*/)
{
  return std::make_tuple(arr[Is]...);
}

template <size_t N>
constexpr auto as_tuple(const std::array<unsigned, N>& arr)
{
  return as_tuple(arr, std::make_index_sequence<N>{});
}

template <typename T, typename F, size_t... Is>
constexpr void for_each_impl(T&& t, const F& f, std::index_sequence<Is...>)
{
  (void)std::initializer_list<int>{(f(std::get<Is>(t)), 0)...};
}

template <typename T, typename F>
constexpr void for_each(T&& t, const F& f)
{
  for_each_impl(t, f, std::make_index_sequence<std::tuple_size<std::decay_t<T>>::value>{});
}

template <typename Tuple, typename Pred>
constexpr bool any_of_impl(Tuple&&, Pred&&, std::index_sequence<>)
{
  return false;
}

template <typename Tuple, typename Pred, size_t FirstIs, size_t... Is>
constexpr bool any_of_impl(Tuple&& t, Pred&& pred, std::index_sequence<FirstIs, Is...>)
{
  return pred(std::get<FirstIs>(t)) || any_of_impl(t, std::forward<Pred>(pred), std::index_sequence<Is...>{});
}

template <typename... Elements, typename Pred, size_t... is>
constexpr bool any_of(std::tuple<Elements...>& t, Pred&& pred)
{
  return any_of_impl(t, std::forward<Pred>(pred), std::index_sequence_for<Elements...>{});
}

} // namespace detail

/// \brief Task worker that can handle tasks with different priorities. Each priority level gets associated a separate
/// task queue with queueing policy defined by \c concurrent_queue_policy. The task worker will pop tasks starting from
/// the highest priority queue and will continue to the next priority level if the current queue is empty. If there are
/// no tasks in any of the queues, the task worker will wait for \c wait_for_task_sleep before checking for new tasks.
///
/// \tparam QueuePolicies Queue policies for each priority level. The number of policies must match the number of
/// supported priority levels.
template <concurrent_queue_policy... QueuePolicies>
class priority_multiqueue_task_worker
{
public:
  /// \brief Construct a new priority multiqueue task worker object.
  /// \param thread_name The name of the thread used by the task worker.
  /// \param task_queue_sizes The sizes of the task queues for each priority level.
  /// \param wait_for_task_sleep_ The amount of time to suspend the thread, when no tasks are pending.
  /// \param prio task worker OS thread priority level.
  /// \param mask thread OS affinity bitmask.
  priority_multiqueue_task_worker(std::string                                           thread_name,
                                  const std::array<unsigned, sizeof...(QueuePolicies)>& task_queue_sizes,
                                  std::chrono::microseconds                             wait_for_task_sleep_,
                                  os_thread_realtime_priority      prio = os_thread_realtime_priority::no_realtime(),
                                  const os_sched_affinity_bitmask& mask = {}) :
    wait_for_task_sleep(wait_for_task_sleep_),
    task_queues(detail::as_tuple(task_queue_sizes)),
    t_handle(thread_name, prio, mask, [this]() { run_pop_task_loop(); })
  {
  }
  ~priority_multiqueue_task_worker() { stop(); }

  /// Stop task worker, if running.
  void stop()
  {
    if (t_handle.running()) {
      running = false;
      detail::for_each(task_queues, [](auto& queue) { queue.request_stop(); });
      t_handle.join();
    }
  }

  /// \brief Push task to task queue with priority level \c Priority (lower integer represents a higher priority).
  template <task_queue_priority Priority>
  SRSRAN_NODISCARD bool push_task(unique_task task)
  {
    return std::get<task_queue_priority_to_index(Priority)>(task_queues).try_push(std::move(task));
  }

  /// \brief Get specified priority task queue capacity.
  template <task_queue_priority Priority>
  SRSRAN_NODISCARD size_t queue_capacity() const
  {
    return std::get<task_queue_priority_to_index(Priority)>(task_queues).capacity();
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
    while (running.load(std::memory_order_relaxed)) {
      if (not detail::any_of(task_queues,
                             [](auto& queue) mutable { return queue.try_pop([](const unique_task& t) { t(); }); })) {
        // If no task was run, sleep for defined time interval.
        std::this_thread::sleep_for(wait_for_task_sleep);
      }
    }
    srslog::fetch_basic_logger("ALL").info("Task worker \"{}\" finished.", this_thread_name());
  }

  static constexpr size_t task_queue_priority_to_index(task_queue_priority prio)
  {
    return static_cast<size_t>(prio) < nof_priority_levels() ? static_cast<size_t>(prio) : nof_priority_levels() - 1;
  }

  // Time that thread spends sleeping when there are no pending tasks.
  const std::chrono::microseconds wait_for_task_sleep;

  // Task queues with different priorities. The first queue is the highest priority queue.
  std::tuple<concurrent_queue<unique_task, QueuePolicies, concurrent_queue_wait_policy::sleep>...> task_queues;

  // Status of the task worker.
  std::atomic<bool> running{true};

  // Worker thread
  unique_thread t_handle;
};

/// \brief Task executor with \c Priority (lower is higher) for \c priority_multiqueue_task_worker.
template <task_queue_priority Priority, concurrent_queue_policy... QueuePolicies>
class priority_task_worker_executor : public task_executor
{
public:
  priority_task_worker_executor(priority_multiqueue_task_worker<QueuePolicies...>& worker_,
                                bool                                               report_on_push_failure = true) :
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
    if (not worker->template push_task<Priority>(std::move(task))) {
      if (report_on_failure) {
        srslog::fetch_basic_logger("ALL").error(
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
  priority_multiqueue_task_worker<QueuePolicies...>* worker            = nullptr;
  bool                                               report_on_failure = true;
};

/// \brief Create task executor with \c Priority for \c priority_multiqueue_task_worker.
template <task_queue_priority Priority, concurrent_queue_policy... QueuePolicies>
priority_task_worker_executor<Priority, QueuePolicies...>
make_priority_task_worker_executor(priority_multiqueue_task_worker<QueuePolicies...>& worker)
{
  return priority_task_worker_executor<Priority, QueuePolicies...>(worker);
}

/// \brief Create general task executor pointer with \c Priority for \c priority_multiqueue_task_worker.
template <task_queue_priority Priority, concurrent_queue_policy... QueuePolicies>
std::unique_ptr<task_executor>
make_priority_task_executor_ptr(priority_multiqueue_task_worker<QueuePolicies...>& worker)
{
  return std::make_unique<priority_task_worker_executor<Priority, QueuePolicies...>>(worker);
}

} // namespace srsran