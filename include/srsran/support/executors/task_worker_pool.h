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

/// \brief Simple pool of task workers/threads. The workers share the same queue of task and do not perform
/// work-stealing.
template <bool UseLockfreeMPMC = false>
class task_worker_pool
{
  // Queue type.
  using queue_type =
      concurrent_queue<unique_task,
                       UseLockfreeMPMC ? concurrent_queue_policy::lockfree_mpmc : concurrent_queue_policy::locking_mpmc,
                       UseLockfreeMPMC ? concurrent_queue_wait_policy::sleep
                                       : concurrent_queue_wait_policy::condition_variable>;

public:
  /// \brief Creates a task worker pool.
  /// \param nof_workers Number of workers of the worker pool.
  /// \param queue_size Size of the task queue.
  /// \param pool_name String with the name for the worker pool. Individual workers of the pool will be assigned the
  /// name "<pool_name>#<worker index>". E.g. for pool_name="Pool", the second worker will be called "Pool#1".
  /// \param prio Workers realtime thread priority.
  template <bool use_lockfree = UseLockfreeMPMC, std::enable_if_t<use_lockfree, int> = 0>
  task_worker_pool(unsigned                              nof_workers,
                   unsigned                              queue_size,
                   std::string                           worker_pool_name,
                   std::chrono::microseconds             wait_sleep_time,
                   os_thread_realtime_priority           prio      = os_thread_realtime_priority::no_realtime(),
                   span<const os_sched_affinity_bitmask> cpu_masks = {}) :
    pool_name(std::move(worker_pool_name)),
    logger(srslog::fetch_basic_logger("ALL")),
    workers(nof_workers),
    pending_tasks(queue_size, wait_sleep_time)
  {
    start_impl(prio, cpu_masks);
  }
  template <bool use_lockfree = UseLockfreeMPMC, std::enable_if_t<not use_lockfree, int> = 0>
  task_worker_pool(unsigned                              nof_workers,
                   unsigned                              queue_size,
                   std::string                           worker_pool_name,
                   os_thread_realtime_priority           prio      = os_thread_realtime_priority::no_realtime(),
                   span<const os_sched_affinity_bitmask> cpu_masks = {}) :
    pool_name(std::move(worker_pool_name)),
    logger(srslog::fetch_basic_logger("ALL")),
    workers(nof_workers),
    pending_tasks(queue_size)
  {
    start_impl(prio, cpu_masks);
  }
  task_worker_pool(const task_worker_pool&)            = delete;
  task_worker_pool(task_worker_pool&&)                 = delete;
  task_worker_pool& operator=(const task_worker_pool&) = delete;
  task_worker_pool& operator=(task_worker_pool&&)      = delete;
  ~task_worker_pool();

  /// Stop task worker pool, if running.
  void stop();

  /// \brief Number of workers that compose the worker pool.
  unsigned nof_workers() const { return workers.size(); }

  /// \brief Number of tasks currently enqueued.
  uint32_t nof_pending_tasks() const { return pending_tasks.size(); }

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

  /// \brief Wait for all the currently enqueued tasks to complete. If more tasks get enqueued after this function call
  /// those tasks are not accounted for in the waiting.
  void wait_pending_tasks();

  /// Name given to the pool.
  const std::string& name() const { return pool_name; }

  /// Determines whether the caller is inside the pool.
  bool is_in_thread_pool() const
  {
    thread_local const bool inside_pool_flag = [this, id = std::this_thread::get_id()]() {
      for (const worker& w : workers) {
        if (w.t_handle.get_id() == id) {
          return true;
        }
      }
      return false;
    }();
    return inside_pool_flag;
  }

private:
  struct worker {
    unique_thread t_handle;
  };

  void start_impl(os_thread_realtime_priority prio_, span<const os_sched_affinity_bitmask> cpu_masks);

  std::string           pool_name;
  srslog::basic_logger& logger;

  // List of workers belonging to the worker pool.
  std::vector<worker> workers;

  // Queue of tasks.
  queue_type pending_tasks;
};

extern template class task_worker_pool<true>;
extern template class task_worker_pool<false>;

/// \brief Task executor that pushes tasks to worker pool.
template <bool UseLockfreeMPMC = false>
class task_worker_pool_executor final : public task_executor
{
public:
  task_worker_pool_executor() = default;
  task_worker_pool_executor(task_worker_pool<UseLockfreeMPMC>& worker_pool_) : worker_pool(&worker_pool_) {}

  bool execute(unique_task task) override { return worker_pool->push_task(std::move(task)); }

  bool defer(unique_task task) override { return worker_pool->push_task(std::move(task)); }

private:
  task_worker_pool<UseLockfreeMPMC>* worker_pool = nullptr;
};

} // namespace srsran
