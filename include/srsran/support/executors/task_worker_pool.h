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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/unique_thread.h"

namespace srsran {

namespace detail {

// Pool of workers with no associated task enqueueing policy.
class base_worker_pool
{
public:
  base_worker_pool(unsigned                              nof_workers_,
                   const std::string&                    worker_pool_name,
                   const std::function<void()>&          run_tasks_job,
                   os_thread_realtime_priority           prio      = os_thread_realtime_priority::no_realtime(),
                   span<const os_sched_affinity_bitmask> cpu_masks = {});
  base_worker_pool(const base_worker_pool&)            = delete;
  base_worker_pool(base_worker_pool&&)                 = delete;
  base_worker_pool& operator=(const base_worker_pool&) = delete;
  base_worker_pool& operator=(base_worker_pool&&)      = delete;

  /// Name given to the pool.
  const char* name() const { return this->pool_name.c_str(); }

  /// \brief Number of workers that compose the worker pool.
  unsigned nof_workers() const { return worker_threads.size(); }

  /// Determines whether the caller is inside the pool.
  bool is_in_thread_pool() const;

  std::string pool_name;

  // List of workers belonging to the worker pool.
  std::vector<unique_thread> worker_threads;
};

// Worker pool functionality that varies with the task enqueueing policies used.
template <concurrent_queue_policy... QueuePolicies>
class base_worker_pool_with_policy;

// Specialization for single queue.
template <concurrent_queue_policy QueuePolicy>
class base_worker_pool_with_policy<QueuePolicy>
{
  using queue_type = concurrent_queue<unique_task,
                                      QueuePolicy,
                                      QueuePolicy != concurrent_queue_policy::locking_mpmc
                                          ? concurrent_queue_wait_policy::sleep
                                          : concurrent_queue_wait_policy::condition_variable>;

public:
  template <concurrent_queue_policy P                                          = QueuePolicy,
            std::enable_if_t<P == concurrent_queue_policy::lockfree_mpmc, int> = 0>
  base_worker_pool_with_policy(unsigned queue_size, std::chrono::microseconds wait_sleep_time) :
    pending_tasks(queue_size, wait_sleep_time)
  {
  }
  template <concurrent_queue_policy P                                          = QueuePolicy,
            std::enable_if_t<P != concurrent_queue_policy::lockfree_mpmc, int> = 0>
  base_worker_pool_with_policy(unsigned queue_size, std::chrono::microseconds /**/) : pending_tasks(queue_size)
  {
  }

  /// \brief Push a new task to be processed by the worker pool. If the task queue is full, it skips the task and
  /// return false.
  /// \param task Task to be run in the thread pool.
  /// \return True if task was successfully enqueued to be processed. False, if task queue is full.
  SRSRAN_NODISCARD bool push_task(unique_task task) { return pending_tasks.try_push(std::move(task)); }

  /// \brief Push a new task to be processed by the worker pool. If the task queue is full, blocks.
  /// \param task Task to be run in the thread pool.
  bool push_task_blocking(unique_task task) { return pending_tasks.push_blocking(std::move(task)); }

  size_t capacity() const { return pending_tasks.capacity(); }

protected:
  queue_type pending_tasks;
};

template <concurrent_queue_policy QueuePolicy, concurrent_queue_policy... QueuePolicies>
class base_worker_pool_with_policy<QueuePolicy, QueuePolicies...>
{
  using queue_type = concurrent_priority_queue<unique_task, QueuePolicy, QueuePolicies...>;

public:
  base_worker_pool_with_policy(const std::array<unsigned, sizeof...(QueuePolicies) + 1>& task_queue_sizes,
                               std::chrono::microseconds                                 wait_sleep_time) :
    pending_tasks(task_queue_sizes, wait_sleep_time)
  {
  }

  /// \brief Push a new task to be processed by the worker pool. If the task queue is full, it skips the task and
  /// return false.
  /// \param task Task to be run in the thread pool.
  /// \return True if task was successfully enqueued to be processed. False, if task queue is full.
  template <enqueue_priority Priority>
  SRSRAN_NODISCARD bool push_task(unique_task task)
  {
    return pending_tasks.template try_push<Priority>(std::move(task));
  }

  /// \brief Push a new task to be processed by the worker pool. If the task queue is full, blocks.
  /// \param task Task to be run in the thread pool.
  template <enqueue_priority Priority>
  bool push_task_blocking(unique_task task)
  {
    return pending_tasks.template push_blocking<Priority>(std::move(task));
  }

  template <enqueue_priority Priority>
  size_t capacity() const
  {
    return pending_tasks.template capacity<Priority>();
  }

  /// \brief Get enqueuer object for a given Priority.
  template <enqueue_priority Priority>
  auto get_enqueuer()
  {
    return pending_tasks.template get_enqueuer<Priority>();
  }

protected:
  queue_type pending_tasks;
};

} // namespace detail

/// \brief Simple pool of task workers/threads. The workers share the same queue of task and do not perform
/// work-stealing.
template <concurrent_queue_policy... QueuePolicies>
class task_worker_pool : public detail::base_worker_pool_with_policy<QueuePolicies...>, public detail::base_worker_pool
{
  static_assert(sizeof...(QueuePolicies) >= 1, "Invalid number of queues");

  using pool_type  = detail::base_worker_pool;
  using queue_type = detail::base_worker_pool_with_policy<QueuePolicies...>;

public:
  /// \brief Creates a task worker pool.
  /// \param nof_workers Number of workers of the worker pool.
  /// \param queue_size Size of the task queue.
  /// \param pool_name String with the name for the worker pool. Individual workers of the pool will be assigned the
  /// name "<pool_name>#<worker index>". E.g. for pool_name="Pool", the second worker will be called "Pool#1".
  /// \param prio Workers realtime thread priority.
  template <typename QueueSizes>
  task_worker_pool(unsigned                              nof_workers_,
                   const QueueSizes&                     queue_sizes,
                   std::string                           worker_pool_name,
                   std::chrono::microseconds             wait_sleep_time = std::chrono::microseconds{100},
                   os_thread_realtime_priority           prio            = os_thread_realtime_priority::no_realtime(),
                   span<const os_sched_affinity_bitmask> cpu_masks       = {}) :
    queue_type(queue_sizes, wait_sleep_time),
    pool_type(nof_workers_, std::move(worker_pool_name), create_pop_loop_task(), prio, cpu_masks)
  {
  }
  ~task_worker_pool();

  /// Stop task worker pool, if running.
  void stop();

  /// \brief Number of tasks currently enqueued.
  unsigned nof_pending_tasks() const { return this->pending_tasks.size(); }

  /// \brief Wait for all the currently enqueued tasks to complete. If more tasks get enqueued after this function call
  /// those tasks are not accounted for in the waiting.
  void wait_pending_tasks();

private:
  std::function<void()> create_pop_loop_task();

  srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");
};

extern template class task_worker_pool<concurrent_queue_policy::lockfree_mpmc>;
extern template class task_worker_pool<concurrent_queue_policy::locking_mpmc>;
extern template class task_worker_pool<concurrent_queue_policy::lockfree_mpmc, concurrent_queue_policy::lockfree_mpmc>;
extern template class task_worker_pool<concurrent_queue_policy::locking_mpmc, concurrent_queue_policy::lockfree_mpmc>;
extern template class task_worker_pool<concurrent_queue_policy::lockfree_mpmc, concurrent_queue_policy::locking_mpmc>;
extern template class task_worker_pool<concurrent_queue_policy::locking_mpmc, concurrent_queue_policy::locking_mpmc>;

template <concurrent_queue_policy QueuePolicy>
class task_worker_pool_executor final : public task_executor
{
public:
  task_worker_pool_executor() = default;
  task_worker_pool_executor(task_worker_pool<QueuePolicy>& worker_pool_) : worker_pool(&worker_pool_) {}

  SRSRAN_NODISCARD bool execute(unique_task task) override
  {
    // TODO: Shortpath if can_run_task_inline() returns true. This feature has been disabled while we don't correct the
    //  use of .execute in some places.
    return worker_pool->push_task(std::move(task));
  }

  SRSRAN_NODISCARD bool defer(unique_task task) override { return worker_pool->push_task(std::move(task)); }

  /// Determine whether the caller is in one of the threads of the worker pool.
  bool can_run_task_inline() const { return worker_pool->is_in_thread_pool(); }

private:
  task_worker_pool<QueuePolicy>* worker_pool = nullptr;
};

/// \brief Task executor that pushes tasks to worker pool with a given priority.
template <concurrent_queue_policy QueuePolicy>
class priority_task_worker_pool_executor final : public task_executor
{
public:
  priority_task_worker_pool_executor() = default;
  priority_task_worker_pool_executor(priority_enqueuer<unique_task, QueuePolicy> enqueuer_,
                                     enqueue_priority                            prio_,
                                     detail::base_worker_pool&                   workers_) :
    enqueuer(std::move(enqueuer_)), prio(prio_), workers(workers_)
  {
  }

  SRSRAN_NODISCARD bool execute(unique_task task) override
  {
    if (can_run_task_inline()) {
      task();
      return true;
    }
    return enqueuer.try_push(std::move(task));
  }

  SRSRAN_NODISCARD bool defer(unique_task task) override { return enqueuer.try_push(std::move(task)); }

  /// Determine whether the caller is in one of the threads of the worker pool and the the task can run without
  /// being dispatched.
  bool can_run_task_inline() const { return prio == enqueue_priority::max and workers.is_in_thread_pool(); }

private:
  priority_enqueuer<unique_task, QueuePolicy> enqueuer;
  enqueue_priority                            prio;
  detail::base_worker_pool&                   workers;
};

/// \brief Create task executor with \c Priority for \c task_worker_pool that supports multiple priorities.
template <enqueue_priority Priority, concurrent_queue_policy... QueuePolicies>
auto make_priority_task_worker_pool_executor(task_worker_pool<QueuePolicies...>& worker)
{
  return priority_task_worker_pool_executor<get_priority_queue_policy<QueuePolicies...>(Priority)>(
      worker.template get_enqueuer<Priority>(), Priority, worker);
}

/// \brief Create general task executor pointer with \c Priority for \c task_worker_pool that supports multiple
/// priorities.
template <enqueue_priority Priority, concurrent_queue_policy... QueuePolicies>
std::unique_ptr<task_executor> make_priority_task_worker_pool_executor_ptr(task_worker_pool<QueuePolicies...>& worker)
{
  return std::make_unique<priority_task_worker_pool_executor<get_priority_queue_policy<QueuePolicies...>(Priority)>>(
      worker.template get_enqueuer<Priority>(), Priority, worker.name());
}

namespace detail {

template <typename Func, concurrent_queue_policy... QueuePolicies, size_t... Is>
void visit_executor(task_worker_pool<QueuePolicies...>& w,
                    enqueue_priority                    priority,
                    const Func&                         func,
                    std::index_sequence<Is...> /*unused*/)
{
  const size_t idx = detail::enqueue_priority_to_queue_index(priority, sizeof...(QueuePolicies));
  (void)std::initializer_list<int>{[idx, &w, &func]() {
    if (idx == Is) {
      func(
          make_priority_task_worker_pool_executor<detail::queue_index_to_enqueue_priority(Is, sizeof...(QueuePolicies)),
                                                  QueuePolicies...>(w));
    }
    return 0;
  }()...};
}

} // namespace detail

/// \brief Create general task executor pointer with \c Priority for \c priority_task_worker_pool.
template <typename Func, concurrent_queue_policy... QueuePolicies>
void visit_executor(task_worker_pool<QueuePolicies...>& worker, enqueue_priority priority, const Func& func)
{
  detail::visit_executor(worker, priority, func, std::make_index_sequence<sizeof...(QueuePolicies)>{});
}

} // namespace srsran
