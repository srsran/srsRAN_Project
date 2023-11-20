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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/unique_thread.h"

namespace srsran {

namespace detail {

// Pool of workers with no associated task enqueueing policy.
class worker_pool_impl
{
public:
  worker_pool_impl(unsigned                              nof_workers_,
                   const std::string&                    worker_pool_name,
                   const std::function<void()>&          run_tasks_job,
                   os_thread_realtime_priority           prio      = os_thread_realtime_priority::no_realtime(),
                   span<const os_sched_affinity_bitmask> cpu_masks = {});
  worker_pool_impl(const worker_pool_impl&)            = delete;
  worker_pool_impl(worker_pool_impl&&)                 = delete;
  worker_pool_impl& operator=(const worker_pool_impl&) = delete;
  worker_pool_impl& operator=(worker_pool_impl&&)      = delete;

  /// Name given to the pool.
  const std::string& name() const { return this->pool_name; }

  /// \brief Number of workers that compose the worker pool.
  unsigned nof_workers() const { return worker_threads.size(); }

  /// Determines whether the caller is inside the pool.
  bool is_in_thread_pool() const;

  std::string pool_name;

  // List of workers belonging to the worker pool.
  std::vector<unique_thread> worker_threads;
};

template <concurrent_queue_policy... QueuePolicies>
class base_task_queue;

// Specialization for single queue.
template <concurrent_queue_policy QueuePolicy>
class base_task_queue<QueuePolicy>
{
  using queue_type = concurrent_queue<unique_task,
                                      QueuePolicy,
                                      QueuePolicy != concurrent_queue_policy::locking_mpmc
                                          ? concurrent_queue_wait_policy::sleep
                                          : concurrent_queue_wait_policy::condition_variable>;

public:
  template <concurrent_queue_policy P                                          = QueuePolicy,
            std::enable_if_t<P == concurrent_queue_policy::lockfree_mpmc, int> = 0>
  base_task_queue(std::string pool_name_, unsigned queue_size, std::chrono::microseconds wait_sleep_time) :
    pool_name(std::move(pool_name_)),
    logger(srslog::fetch_basic_logger("ALL")),
    pending_tasks(queue_size, wait_sleep_time)
  {
  }
  template <concurrent_queue_policy P                                          = QueuePolicy,
            std::enable_if_t<P != concurrent_queue_policy::lockfree_mpmc, int> = 0>
  base_task_queue(std::string pool_name_, unsigned queue_size, std::chrono::microseconds /**/) :
    pool_name(std::move(pool_name_)), logger(srslog::fetch_basic_logger("ALL")), pending_tasks(queue_size)
  {
  }

  /// \brief Push a new task to be processed by the worker pool. If the task queue is full, it skips the task and
  /// return false.
  /// \param task Task to be run in the thread pool.
  /// \return True if task was successfully enqueued to be processed. False, if task queue is full.
  SRSRAN_NODISCARD bool push_task(unique_task task)
  {
    bool success = pending_tasks.try_push(std::move(task));
    if (not success) {
      logger.error("Cannot push anymore tasks into the {} worker pool queue. Maximum size is {}",
                   pool_name,
                   pending_tasks.capacity());
      return false;
    }
    return true;
  }

  /// \brief Push a new task to be processed by the worker pool. If the task queue is full, blocks.
  /// \param task Task to be run in the thread pool.
  void push_task_blocking(unique_task task)
  {
    bool success = pending_tasks.push_blocking(std::move(task));
    if (not success) {
      logger.debug("Cannot push anymore tasks into the {} worker queue because it was closed", pool_name);
      return;
    }
  }

protected:
  std::string           pool_name;
  srslog::basic_logger& logger;

  queue_type pending_tasks;
};

template <concurrent_queue_policy QueuePolicy, concurrent_queue_policy... QueuePolicies>
class base_task_queue<QueuePolicy, QueuePolicies...>
{
  using queue_type = concurrent_priority_queue<unique_task, QueuePolicy, QueuePolicies...>;

public:
  base_task_queue(std::string                                               pool_name_,
                  const std::array<unsigned, sizeof...(QueuePolicies) + 1>& task_queue_sizes,
                  std::chrono::microseconds                                 wait_sleep_time) :
    pool_name(std::move(pool_name_)),
    logger(srslog::fetch_basic_logger("ALL")),
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
    bool success = pending_tasks.template try_push<Priority>(std::move(task));
    if (not success) {
      logger.error("Cannot push anymore tasks into the {} worker pool queue. Maximum size is {}",
                   pool_name,
                   pending_tasks.template capacity<Priority>());
      return false;
    }
    return true;
  }

  /// \brief Push a new task to be processed by the worker pool. If the task queue is full, blocks.
  /// \param task Task to be run in the thread pool.
  template <enqueue_priority Priority>
  void push_task_blocking(unique_task task)
  {
    bool success = pending_tasks.template push_blocking<Priority>(std::move(task));
    if (not success) {
      logger.debug("Cannot push anymore tasks into the {} worker queue because it was closed", pool_name);
      return;
    }
  }

protected:
  std::string           pool_name;
  srslog::basic_logger& logger;

  queue_type pending_tasks;
};

} // namespace detail

/// \brief Simple pool of task workers/threads. The workers share the same queue of task and do not perform
/// work-stealing.
template <concurrent_queue_policy... QueuePolicies>
class task_worker_pool : public detail::base_task_queue<QueuePolicies...>, public detail::worker_pool_impl
{
  static_assert(sizeof...(QueuePolicies) >= 1, "Invalid number of queues");

  using pool_type  = detail::worker_pool_impl;
  using queue_type = detail::base_task_queue<QueuePolicies...>;

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
    queue_type(worker_pool_name, queue_sizes, wait_sleep_time),
    pool_type(nof_workers_, std::move(worker_pool_name), create_pop_loop_task(), prio, cpu_masks)
  {
  }
  ~task_worker_pool();

  /// Stop task worker pool, if running.
  void stop();

  /// \brief Number of tasks currently enqueued.
  uint32_t nof_pending_tasks() const { return this->pending_tasks.size(); }

  /// \brief Wait for all the currently enqueued tasks to complete. If more tasks get enqueued after this function call
  /// those tasks are not accounted for in the waiting.
  void wait_pending_tasks();

private:
  std::function<void()> create_pop_loop_task();
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

  bool execute(unique_task task) override { return worker_pool->push_task(std::move(task)); }

  bool defer(unique_task task) override { return worker_pool->push_task(std::move(task)); }

private:
  task_worker_pool<QueuePolicy>* worker_pool = nullptr;
};

/// \brief Task executor that pushes tasks to worker pool.
template <enqueue_priority Priority, concurrent_queue_policy... QueuePolicies>
class prio_task_worker_pool_executor final : public task_executor
{
public:
  prio_task_worker_pool_executor() = default;
  prio_task_worker_pool_executor(task_worker_pool<QueuePolicies...>& worker_pool_) : worker_pool(&worker_pool_) {}

  bool execute(unique_task task) override { return worker_pool->push_task<Priority>(std::move(task)); }

  bool defer(unique_task task) override { return worker_pool->push_task<Priority>(std::move(task)); }

private:
  task_worker_pool<QueuePolicies...>* worker_pool = nullptr;
};

} // namespace srsran
