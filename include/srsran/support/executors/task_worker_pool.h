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

#include "unique_thread.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/adt/mutexed_mpmc_queue.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/detail/priority_task_queue.h"
#include "srsran/support/executors/task_executor.h"

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

class base_priority_task_queue
{
protected:
  base_priority_task_queue(span<const concurrent_queue_params> queue_params,
                           std::chrono::microseconds           wait_sleep_time) :
    queue(queue_params, wait_sleep_time)
  {
  }

  // Queue of pending tasks.
  detail::priority_task_queue queue;
};

template <concurrent_queue_policy QueuePolicy>
class base_task_queue;

template <>
class base_task_queue<concurrent_queue_policy::lockfree_mpmc>
{
protected:
  base_task_queue(size_t qsize, std::chrono::microseconds wait_sleep_time) : queue(qsize, wait_sleep_time) {}

  // Queue of pending tasks.
  concurrent_queue<unique_task, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep> queue;
};

template <>
class base_task_queue<concurrent_queue_policy::locking_mpmc>
{
protected:
  base_task_queue(size_t qsize, std::chrono::microseconds wait_sleep_time = std::chrono::microseconds{0}) : queue(qsize)
  {
  }

  // Queue of pending tasks.
  concurrent_queue<unique_task, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::condition_variable>
      queue;
};

} // namespace detail

class priority_task_worker_pool : public detail::base_priority_task_queue, public detail::base_worker_pool
{
public:
  /// \brief Creates a task worker pool that supports multiple priority levels for the dispatched tasks.
  /// \param worker_pool_name String with the name for the worker pool. Individual workers of the pool will be assigned
  /// a thread name equal to "worker_pool_name#<worker index>". E.g. for pool_name="Pool", the second worker will be
  /// called "Pool#1".
  /// \param nof_workers_ Number of workers of the worker pool.
  /// \param queue_params Parameters of the task queue.
  /// \param wait_sleep_time Time each worker waits sleeping before polling the task queue for new tasks.
  /// \param prio Workers realtime thread priority.
  /// \param cpu_masks A list of CPU indexes to which the threads of the worker pool get pinned to.
  priority_task_worker_pool(std::string                           worker_pool_name,
                            unsigned                              nof_workers_,
                            span<const concurrent_queue_params>   queue_params,
                            std::chrono::microseconds             wait_sleep_time = std::chrono::microseconds{100},
                            os_thread_realtime_priority           prio = os_thread_realtime_priority::no_realtime(),
                            span<const os_sched_affinity_bitmask> cpu_masks = {});
  ~priority_task_worker_pool();

  /// \brief Push a new task to be processed by the worker pool. If the task queue is full, it skips the task and
  /// return false.
  /// \param prio Priority of the dispatched task.
  /// \param task Task to be run in the thread pool.
  /// \return True if task was successfully enqueued to be processed. False, if task queue is full.
  SRSRAN_NODISCARD bool push_task(enqueue_priority prio, unique_task task)
  {
    return queue.try_push(prio, std::move(task));
  }

  /// \brief Push a new task to be processed by the worker pool. If the task queue is full, blocks.
  /// \param prio Priority of the dispatched task.
  /// \param task Task to be run in the thread pool.
  bool push_task_blocking(enqueue_priority prio, unique_task task)
  {
    return queue.push_blocking(prio, std::move(task));
  }

  /// Stop task worker pool, if running.
  void stop();

  /// \brief Number of tasks currently enqueued.
  unsigned nof_pending_tasks() const { return this->queue.size(); }

  /// \brief Get how many jobs of a given priority can be enqueued without filling the queue.
  size_t capacity(enqueue_priority prio) const { return queue.queue_capacity(prio); }

  /// \brief Wait for all the currently enqueued tasks to complete. If more tasks get enqueued after this function call
  /// those tasks are not accounted for in the waiting.
  void wait_pending_tasks();

private:
  std::function<void()> create_pop_loop_task();

  srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");
};

/// \brief Simple pool of task workers/threads. The workers share the same queue of task and do not perform
/// work-stealing.
template <concurrent_queue_policy QueuePolicy = concurrent_queue_policy::lockfree_mpmc>
class task_worker_pool : public detail::base_task_queue<QueuePolicy>, public detail::base_worker_pool
{
public:
  /// \brief Creates a task worker pool with no prioritization of tasks.
  /// \param pool_name String with the name for the worker pool. Individual workers of the pool will be assigned the
  /// \param nof_workers Number of workers of the worker pool.
  /// \param queue_size Size of the task queue.
  /// name "<pool_name>#<worker index>". E.g. for pool_name="Pool", the second worker will be called "Pool#1".
  /// \param prio Workers realtime thread priority.
  task_worker_pool(std::string                           worker_pool_name,
                   unsigned                              nof_workers_,
                   unsigned                              qsize_,
                   std::chrono::microseconds             wait_sleep_time = std::chrono::microseconds{100},
                   os_thread_realtime_priority           prio            = os_thread_realtime_priority::no_realtime(),
                   span<const os_sched_affinity_bitmask> cpu_masks       = {}) :
    detail::base_task_queue<QueuePolicy>(qsize_, wait_sleep_time),
    detail::base_worker_pool(nof_workers_, std::move(worker_pool_name), create_pop_loop_task(), prio, cpu_masks)
  {
  }
  ~task_worker_pool();

  /// \brief Push a new task to be processed by the worker pool. If the task queue is full, it skips the task and
  /// return false.
  /// \param task Task to be run in the thread pool.
  /// \return True if task was successfully enqueued to be processed. False, if task queue is full.
  SRSRAN_NODISCARD bool push_task(unique_task task) { return this->queue.try_push(std::move(task)); }

  /// \brief Push a new task to be processed by the worker pool. If the task queue is full, blocks.
  /// \param task Task to be run in the thread pool.
  bool push_task_blocking(unique_task task) { return this->queue.push_blocking(std::move(task)); }

  /// Stop task worker pool, if running.
  void stop();

  /// \brief Number of tasks currently enqueued.
  unsigned nof_pending_tasks() const { return this->queue.size(); }

  /// \brief Maximum number of jobs that can be pending.
  size_t capacity() const { return this->queue.capacity(); }

  /// \brief Wait for all the currently enqueued tasks to complete. If more tasks get enqueued after this function call
  /// those tasks are not accounted for in the waiting.
  void wait_pending_tasks();

private:
  std::function<void()> create_pop_loop_task();

  srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");
};

extern template class task_worker_pool<concurrent_queue_policy::lockfree_mpmc>;
extern template class task_worker_pool<concurrent_queue_policy::locking_mpmc>;

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

/// \brief Create task executor for basic \c task_worker_pool.
template <concurrent_queue_policy QueuePolicy>
auto make_task_worker_pool_executor(task_worker_pool<QueuePolicy>& worker)
{
  return task_worker_pool_executor<QueuePolicy>(worker);
}

/// \brief Create task executor for basic \c task_worker_pool.
template <concurrent_queue_policy QueuePolicy>
std::unique_ptr<task_executor> make_task_worker_pool_executor_ptr(task_worker_pool<QueuePolicy>& worker)
{
  return std::make_unique<task_worker_pool_executor<QueuePolicy>>(worker);
}

/// \brief Task executor that pushes tasks to worker pool with a given priority.
class priority_task_worker_pool_executor final : public task_executor
{
public:
  priority_task_worker_pool_executor(enqueue_priority prio_, priority_task_worker_pool& pool) :
    prio(prio_), workers(pool)
  {
  }

  SRSRAN_NODISCARD bool execute(unique_task task) override
  {
    if (can_run_task_inline()) {
      task();
      return true;
    }
    return workers.push_task(prio, std::move(task));
  }

  SRSRAN_NODISCARD bool defer(unique_task task) override { return workers.push_task(prio, std::move(task)); }

  /// Determine whether the caller is in one of the threads of the worker pool and the the task can run without
  /// being dispatched.
  bool can_run_task_inline() const { return prio == enqueue_priority::max and workers.is_in_thread_pool(); }

private:
  enqueue_priority           prio;
  priority_task_worker_pool& workers;
};

/// \brief Create task executor with \c Priority for \c task_worker_pool that supports multiple priorities.
inline auto make_priority_task_worker_pool_executor(enqueue_priority prio, priority_task_worker_pool& worker)
{
  return priority_task_worker_pool_executor(prio, worker);
}

/// \brief Create general task executor pointer with \c Priority for \c task_worker_pool that supports multiple
/// priorities.
inline std::unique_ptr<task_executor> make_priority_task_worker_pool_executor_ptr(enqueue_priority           prio,
                                                                                  priority_task_worker_pool& worker)
{
  return std::make_unique<priority_task_worker_pool_executor>(prio, worker);
}

} // namespace srsran
