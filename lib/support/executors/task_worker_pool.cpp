/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/task_worker_pool.h"
#include <future>

using namespace srsran;

template <bool UseLockfreeMPMC>
void task_worker_pool<UseLockfreeMPMC>::start_impl(unsigned                              nof_workers,
                                                   os_thread_realtime_priority           prio_,
                                                   span<const os_sched_affinity_bitmask> cpu_masks)
{
  workers.clear();
  workers.reserve(nof_workers);

  if (cpu_masks.size() > 1) {
    // An array with a single mask is allowed, otherwise the number of masks must be equal to the number of workers.
    srsran_assert(cpu_masks.size() == nof_workers, "Wrong array of CPU masks provided");
  }
  for (unsigned i = 0; i != nof_workers; ++i) {
    auto task_func = [this, i]() {
      while (true) {
        optional<unique_task> task = pending_tasks.pop_blocking();
        if (not task.has_value()) {
          break;
        }
        (*task)();
      }
      logger.info("Task worker \"{}\" finished.", workers[i].get_name());
    };
    if (cpu_masks.empty()) {
      workers.emplace_back(fmt::format("{}#{}", pool_name, i), prio_, task_func);
    } else {
      // Check whether a single mask for all workers should be used.
      os_sched_affinity_bitmask cpu_mask = (cpu_masks.size() == 1) ? cpu_masks[0] : cpu_masks[i];
      workers.emplace_back(fmt::format("{}#{}", pool_name, i), prio_, cpu_mask, task_func);
    }
  }
}

template <bool UseLockfreeMPMC>
task_worker_pool<UseLockfreeMPMC>::~task_worker_pool()
{
  stop();
}

template <bool UseLockfreeMPMC>
void task_worker_pool<UseLockfreeMPMC>::stop()
{
  for (unique_thread& w : workers) {
    if (w.running()) {
      pending_tasks.request_stop();
      w.join();
    }
  }
}

template <bool UseLockfreeMPMC>
void task_worker_pool<UseLockfreeMPMC>::wait_pending_tasks()
{
  std::mutex              mutex;
  std::condition_variable cvar_all_sync, cvar_caller_return;

  // Check if there are workers still running.
  unsigned workers_running = 0;
  for (unsigned i = 0; i != workers.size(); ++i) {
    if (workers[i].running()) {
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
    push_task_blocking([&mutex, &cvar_all_sync, &cvar_caller_return, &count_workers_not_sync, &counter_caller]() {
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

template <bool UseLockfreeMPMC>
bool srsran::task_worker_pool<UseLockfreeMPMC>::is_in_thread_pool() const
{
  std::any_of(workers.begin(), workers.end(), [id = std::this_thread::get_id()](const unique_thread& t) {
    return t.get_id() == id;
  });
  return false;
}

template class srsran::task_worker_pool<false>;
template class srsran::task_worker_pool<true>;
