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

#include "rigtorp/SPSCQueue.h"
#include "srsran/adt/blocking_queue.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/unique_thread.h"

namespace srsran {

/// \brief Different types of task queues supported in multi_priority_task_worker.
enum class task_queue_policy { spsc, blocking };

namespace detail {

/// \brief Wrapper for SPSC task queue.
template <task_queue_policy QueuePolicy = task_queue_policy::spsc>
struct TaskQueueWrapper {
  TaskQueueWrapper(unsigned queue_size) : queue(queue_size) {}

  void stop()
  {
    // do nothing.
  }

  bool push(unique_task& task) { return queue.try_push(std::move(task)); }

  bool pop_and_run()
  {
    unique_task* task = queue.front();
    if (task != nullptr) {
      (*task)();
      queue.pop();
      return true;
    }
    return false;
  }

  size_t capacity() const { return queue.capacity(); }

private:
  rigtorp::SPSCQueue<unique_task> queue;
};

/// \brief Wrapper for blocking task queue.
template <>
struct TaskQueueWrapper<task_queue_policy::blocking> {
  TaskQueueWrapper(unsigned queue_size) : queue(queue_size) {}

  void stop() { queue.stop(); }

  bool push(unique_task& task) { return queue.try_push(std::move(task)).has_value(); }

  bool pop_and_run()
  {
    unique_task task;
    if (queue.try_pop(task)) {
      task();
      return true;
    }
    return false;
  }

  size_t capacity() const { return queue.max_size(); }

private:
  blocking_queue<unique_task> queue;
};

template <size_t N, size_t... Is>
auto as_tuple(const std::array<unsigned, N>& arr, std::index_sequence<Is...> /*unused*/)
{
  return std::make_tuple(arr[Is]...);
}

template <size_t N>
auto as_tuple(const std::array<unsigned, N>& arr)
{
  return as_tuple(arr, std::make_index_sequence<N>{});
}

template <typename T, typename F, size_t... Is>
void for_each_impl(T&& t, const F& f, std::index_sequence<Is...>)
{
  (void)std::initializer_list<int>{(f(std::get<Is>(t)), 0)...};
}

template <typename T, typename F>
void for_each(T&& t, const F& f)
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

/// \brief Task worker that can handle tasks with different priorities.
template <task_queue_policy... QueuePolicies>
class multi_priority_task_worker
{
public:
  multi_priority_task_worker(std::string                                           thread_name,
                             const std::array<unsigned, sizeof...(QueuePolicies)>& task_queue_sizes,
                             std::chrono::microseconds                             wait_for_task_sleep_,
                             os_thread_realtime_priority      prio = os_thread_realtime_priority::no_realtime(),
                             const os_sched_affinity_bitmask& mask = {}) :
    wait_for_task_sleep(wait_for_task_sleep_),
    task_queues(detail::as_tuple(task_queue_sizes)),
    t_handle(thread_name, prio, mask, [this]() { run_pop_task_loop(); })
  {
  }
  ~multi_priority_task_worker() { stop(); }

  /// Stop task worker, if running.
  void stop()
  {
    if (running.exchange(false)) {
      detail::for_each(task_queues, [](auto& queue) { queue.stop(); });
      t_handle.join();
    }
  }

  /// \brief Push task to task queue with priority level \c Priority (lower integer represents a higher priority).
  template <size_t Priority>
  SRSRAN_NODISCARD bool push_task(unique_task task)
  {
    return std::get<Priority>(task_queues).push(task);
  }

  /// \brief Get specified priority task capacity.
  template <size_t Priority>
  SRSRAN_NODISCARD size_t queue_capacity() const
  {
    return std::get<Priority>(task_queues).capacity();
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
      if (not detail::any_of(task_queues, [](auto& queue) mutable { return queue.pop_and_run(); })) {
        // If no task was run, sleep for defined time interval.
        std::this_thread::sleep_for(wait_for_task_sleep);
      }
    }
    srslog::fetch_basic_logger("ALL").info("Task worker \"{}\" finished.", this_thread_name());
  }

  // Time that thread spends sleeping when there are no pending tasks.
  const std::chrono::microseconds wait_for_task_sleep;

  // Task queues with different priorities. The first queue is the highest priority queue.
  std::tuple<detail::TaskQueueWrapper<QueuePolicies>...> task_queues;

  // Status of the task worker.
  std::atomic<bool> running{true};

  // Worker thread
  unique_thread t_handle;
};

/// \brief Task executor with \c Priority (lower is higher) for \c multi_priority_task_worker.
template <size_t Priority, task_queue_policy... QueuePolicies>
class priority_task_worker_executor : public task_executor
{
public:
  priority_task_worker_executor(multi_priority_task_worker<QueuePolicies...>& worker_,
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
  multi_priority_task_worker<QueuePolicies...>* worker            = nullptr;
  bool                                          report_on_failure = true;
};

/// \brief Create task executor with \c Priority (lower is higher) for \c multi_priority_task_worker.
template <size_t Priority, task_queue_policy... QueuePolicies>
priority_task_worker_executor<Priority, QueuePolicies...>
make_priority_task_worker_executor(multi_priority_task_worker<QueuePolicies...>& worker)
{
  return priority_task_worker_executor<Priority, QueuePolicies...>(worker);
}

/// \brief Create general task executor with \c Priority (lower is higher) for \c multi_priority_task_worker.
template <size_t Priority, task_queue_policy... QueuePolicies>
std::unique_ptr<task_executor> make_task_executor(multi_priority_task_worker<QueuePolicies...>& worker)
{
  return std::make_unique<priority_task_worker_executor<Priority, QueuePolicies...>>(worker);
}

} // namespace srsran