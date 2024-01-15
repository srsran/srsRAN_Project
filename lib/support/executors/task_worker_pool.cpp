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

#include "srsran/support/executors/task_worker_pool.h"
#include <future>

using namespace srsran;

detail::base_worker_pool::base_worker_pool(unsigned                              nof_workers_,
                                           const std::string&                    worker_pool_name,
                                           const std::function<void()>&          run_tasks_job,
                                           os_thread_realtime_priority           prio,
                                           span<const os_sched_affinity_bitmask> cpu_masks) :
  pool_name(worker_pool_name)
{
  worker_threads.reserve(nof_workers_);

  if (cpu_masks.size() > 1) {
    // An array with a single mask is allowed, otherwise the number of masks must be equal to the number of workers.
    report_error_if_not(cpu_masks.size() == nof_workers_, "Wrong array of CPU masks provided");
  }

  // Task dispatched to workers of the pool.
  for (unsigned i = 0; i != nof_workers_; ++i) {
    if (cpu_masks.empty()) {
      worker_threads.emplace_back(fmt::format("{}#{}", worker_pool_name, i), prio, run_tasks_job);
    } else {
      // Check whether a single mask for all workers should be used.
      os_sched_affinity_bitmask cpu_mask = (cpu_masks.size() == 1) ? cpu_masks[0] : cpu_masks[i];
      worker_threads.emplace_back(fmt::format("{}#{}", worker_pool_name, i), prio, cpu_mask, run_tasks_job);
    }
  }
}

bool detail::base_worker_pool::is_in_thread_pool() const
{
  return std::any_of(worker_threads.begin(),
                     worker_threads.end(),
                     [id = std::this_thread::get_id()](const unique_thread& t) { return t.get_id() == id; });
}

// //////////////////////////

template <concurrent_queue_policy... QueuePolicies>
task_worker_pool<QueuePolicies...>::~task_worker_pool()
{
  stop();
}

template <concurrent_queue_policy... QueuePolicies>
void task_worker_pool<QueuePolicies...>::stop()
{
  unsigned count = 0;
  for (unique_thread& w : worker_threads) {
    if (w.running()) {
      this->pending_tasks.request_stop();
      w.join();
      this->logger.info("Task worker \"{}\" finished.", worker_threads[count].get_name());
    }
    count++;
  }
}

// Helper function to push a blocking task to a concurrent queue.
template <concurrent_queue_policy... QueuePolicies>
bool push_blocking_prio_queue_helper(concurrent_priority_queue<unique_task, QueuePolicies...>& q, unique_task t)
{
  return q.template push_blocking<enqueue_priority::max>(std::move(t));
}
template <concurrent_queue_policy QueuePolicy, concurrent_queue_wait_policy WaitPolicy>
bool push_blocking_prio_queue_helper(concurrent_queue<unique_task, QueuePolicy, WaitPolicy>& q, unique_task t)
{
  return q.push_blocking(std::move(t));
}

template <concurrent_queue_policy... QueuePolicies>
void task_worker_pool<QueuePolicies...>::wait_pending_tasks()
{
  std::mutex              mutex;
  std::condition_variable cvar_all_sync, cvar_caller_return;

  // Check if there are workers still running.
  unsigned workers_running = 0;
  for (unsigned i = 0; i != nof_workers(); ++i) {
    if (worker_threads[i].running()) {
      workers_running++;
    }
  }
  if (workers_running == 0) {
    // If no worker is running, return immediately.
    return;
  }

  // This will block all workers until all of them are running the enqueued task.
  unsigned count_workers_not_sync = workers_running;
  unsigned counter_caller         = workers_running;
  for (unsigned i = 0; i != workers_running; ++i) {
    push_blocking_prio_queue_helper(
        this->pending_tasks, [&mutex, &cvar_all_sync, &cvar_caller_return, &count_workers_not_sync, &counter_caller]() {
          std::unique_lock<std::mutex> lock(mutex);

          // Sync all workers. Only when all workers are in sync, we can carry on.
          count_workers_not_sync--;
          if (count_workers_not_sync > 0) {
            cvar_all_sync.wait(lock, [&count_workers_not_sync]() { return count_workers_not_sync == 0; });
          } else {
            cvar_all_sync.notify_all();
          }

          // When all workers passed the condition variable, we can notify the caller.
          if (--counter_caller == 0) {
            cvar_caller_return.notify_one();
          }
        });
  }

  // Caller blocks waiting for all workers to sync.
  std::unique_lock<std::mutex> lock(mutex);
  cvar_caller_return.wait(lock, [&counter_caller]() { return counter_caller == 0; });
}

template <concurrent_queue_policy... QueuePolicies>
std::function<void()> task_worker_pool<QueuePolicies...>::create_pop_loop_task()
{
  return [this]() {
    unique_task job;
    while (true) {
      if (not this->pending_tasks.pop_blocking(job)) {
        break;
      }
      job();
    }
  };
}

// Explicit specializations of the task_worker_pool.
template class srsran::task_worker_pool<concurrent_queue_policy::lockfree_mpmc>;
template class srsran::task_worker_pool<concurrent_queue_policy::locking_mpmc>;
template class srsran::task_worker_pool<concurrent_queue_policy::lockfree_mpmc, concurrent_queue_policy::lockfree_mpmc>;
template class srsran::task_worker_pool<concurrent_queue_policy::lockfree_mpmc, concurrent_queue_policy::locking_mpmc>;
template class srsran::task_worker_pool<concurrent_queue_policy::locking_mpmc, concurrent_queue_policy::lockfree_mpmc>;
template class srsran::task_worker_pool<concurrent_queue_policy::locking_mpmc, concurrent_queue_policy::locking_mpmc>;
