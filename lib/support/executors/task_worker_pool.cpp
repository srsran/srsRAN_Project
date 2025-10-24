/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "execution_context_description_setup.h"
#include "srsran/support/synchronization/sync_event.h"

using namespace srsran;

namespace {

// Creates the task that will be run by each instantiated worker
template <typename Queue>
std::function<void()> worker_task_factory(Queue& queue, unsigned nof_workers, unsigned worker_idx)
{
  return [&queue, nof_workers, worker_idx]() {
    // setup worker execution context.
    execution_context::set_execution_context_description(nof_workers, worker_idx);

    // start worker pop loop.
    auto consumer = queue.create_consumer();
    while (true) {
      unique_task job;
      if (not consumer.pop_blocking(job)) {
        break;
      }
      job();
    }
  };
}

} // namespace

detail::base_worker_pool::base_worker_pool(unsigned                                              nof_workers_,
                                           const std::string&                                    worker_pool_name,
                                           const std::function<std::function<void()>(unsigned)>& worker_task_factory,
                                           os_thread_realtime_priority                           prio,
                                           span<const os_sched_affinity_bitmask>                 cpu_masks) :
  pool_name(worker_pool_name)
{
  worker_threads.reserve(nof_workers_);

  if (cpu_masks.size() > 1) {
    // An array with a single mask is allowed, otherwise the number of masks must be equal to the number of workers.
    report_error_if_not(cpu_masks.size() == nof_workers_, "Wrong array of CPU masks provided for {}", worker_pool_name);
  }

  // Task dispatched to workers of the pool.
  for (unsigned i = 0; i != nof_workers_; ++i) {
    if (cpu_masks.empty()) {
      worker_threads.emplace_back(fmt::format("{}#{}", worker_pool_name, i), prio, worker_task_factory(i));
    } else {
      // Check whether a single mask for all workers should be used.
      os_sched_affinity_bitmask cpu_mask = (cpu_masks.size() == 1) ? cpu_masks[0] : cpu_masks[i];
      worker_threads.emplace_back(fmt::format("{}#{}", worker_pool_name, i), prio, cpu_mask, worker_task_factory(i));
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

priority_task_worker_pool::priority_task_worker_pool(std::string                           worker_pool_name,
                                                     unsigned                              nof_workers_,
                                                     span<const concurrent_queue_params>   queue_params,
                                                     std::chrono::microseconds             wait_sleep_time,
                                                     os_thread_realtime_priority           prio,
                                                     span<const os_sched_affinity_bitmask> cpu_masks) :
  detail::base_priority_task_queue(queue_params, wait_sleep_time),
  detail::base_worker_pool(
      nof_workers_,
      std::move(worker_pool_name),
      [this, nof_workers_](unsigned worker_idx) { return worker_task_factory(this->queue, nof_workers_, worker_idx); },
      prio,
      cpu_masks)
{
  report_fatal_error_if_not(nof_workers_ > 0, "Number of workers of a thread pool must be greater than 0");
  srsran_assert(queue_params.size() >= 2, "Number of queues in a prioritized thread pool must be greater than 1");
}

priority_task_worker_pool::~priority_task_worker_pool()
{
  stop();
}

void priority_task_worker_pool::stop()
{
  unsigned count = 0;
  for (unique_thread& w : worker_threads) {
    if (w.running()) {
      this->queue.request_stop();
      w.join();
      this->logger.info("Task worker \"{}\" finished.", worker_threads[count].get_name());
    }
    count++;
  }
}

void priority_task_worker_pool::wait_pending_tasks()
{
  std::mutex              mutex;
  std::condition_variable cvar_all_sync;

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
  unsigned   count_workers_not_sync = workers_running;
  sync_event wait_event;
  for (unsigned i = 0; i != workers_running; ++i) {
    this->queue.push_blocking(
        enqueue_priority::min, [&mutex, &cvar_all_sync, &count_workers_not_sync, tk = wait_event.get_token()]() {
          std::unique_lock<std::mutex> lock(mutex);

          // Sync all workers. Only when all workers are in sync, we can carry on.
          count_workers_not_sync--;
          if (count_workers_not_sync > 0) {
            cvar_all_sync.wait(lock, [&count_workers_not_sync]() { return count_workers_not_sync == 0; });
          } else {
            cvar_all_sync.notify_all();
          }
        });
  }

  // Caller blocks waiting for all workers to sync.
  wait_event.wait();
}

// ---- non-prioritized task worker pool

template <concurrent_queue_policy QueuePolicy>
task_worker_pool<QueuePolicy>::task_worker_pool(std::string                           worker_pool_name,
                                                unsigned                              nof_workers_,
                                                unsigned                              qsize_,
                                                std::chrono::microseconds             wait_sleep_time,
                                                unsigned                              nof_prereserved_producers,
                                                os_thread_realtime_priority           prio,
                                                span<const os_sched_affinity_bitmask> cpu_masks) :
  detail::base_task_queue<QueuePolicy>(qsize_, wait_sleep_time, nof_prereserved_producers),
  detail::base_worker_pool(
      nof_workers_,
      std::move(worker_pool_name),
      [this, nof_workers_](unsigned worker_idx) { return worker_task_factory(this->queue, nof_workers_, worker_idx); },
      prio,
      cpu_masks)
{
}

template <concurrent_queue_policy QueuePolicy>
task_worker_pool<QueuePolicy>::~task_worker_pool()
{
  stop();
}

template <concurrent_queue_policy QueuePolicy>
void task_worker_pool<QueuePolicy>::stop()
{
  unsigned count = 0;
  for (unique_thread& w : worker_threads) {
    if (w.running()) {
      this->queue.request_stop();
      w.join();
      this->logger.info("Task worker \"{}\" finished.", worker_threads[count].get_name());
    }
    count++;
  }
}

template <concurrent_queue_policy QueuePolicy>
void task_worker_pool<QueuePolicy>::wait_pending_tasks()
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
    if (not this->queue.push_blocking(
            [&mutex, &cvar_all_sync, &cvar_caller_return, &count_workers_not_sync, &counter_caller]() {
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
            })) {
      // Queue was deactivated.
      std::unique_lock<std::mutex> lock(mutex);
      count_workers_not_sync--;
    }
  }

  // Caller blocks waiting for all workers to sync.
  std::unique_lock<std::mutex> lock(mutex);
  cvar_caller_return.wait(lock, [&counter_caller]() { return counter_caller == 0; });
}

// Explicit specializations of the task_worker_pool.
template class srsran::task_worker_pool<concurrent_queue_policy::lockfree_mpmc>;
template class srsran::task_worker_pool<concurrent_queue_policy::locking_mpmc>;
template class srsran::task_worker_pool<concurrent_queue_policy::moodycamel_lockfree_mpmc>;
