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

#include "srsran/support/executors/task_worker_pool.h"
#include <future>

using namespace srsran;

task_worker_pool::task_worker_pool(unsigned                              nof_workers_,
                                   unsigned                              queue_size,
                                   const std::string&                    worker_name_prefix,
                                   os_thread_realtime_priority           prio_,
                                   span<const os_sched_affinity_bitmask> cpu_masks) :
  pool_name(worker_name_prefix),
  logger(srslog::fetch_basic_logger("ALL")),
  workers(nof_workers_),
  pending_tasks(queue_size)
{
  if (cpu_masks.size() > 1) {
    // An array with a single mask is allowed, otherwise the number of masks must be equal to the number of workers.
    srsran_assert(cpu_masks.size() == nof_workers_, "Wrong array of CPU masks provided");
  }
  for (unsigned i = 0; i != nof_workers_; ++i) {
    auto task_func = [this, i]() {
      while (true) {
        bool        success;
        unique_task task = pending_tasks.pop_blocking(&success);
        if (not success) {
          break;
        }
        task();
      }
      logger.info("Task worker \"{}\" finished.", workers[i].t_handle.get_name());
    };
    if (cpu_masks.empty()) {
      workers[i].t_handle = unique_thread{fmt::format("{}#{}", worker_name_prefix, i), prio_, task_func};
    } else {
      // Check whether a single mask for all workers should be used.
      os_sched_affinity_bitmask cpu_mask = (cpu_masks.size() == 1) ? cpu_masks[0] : cpu_masks[i];
      workers[i].t_handle = unique_thread{fmt::format("{}#{}", worker_name_prefix, i), prio_, cpu_mask, task_func};
    }
  }
}

task_worker_pool::~task_worker_pool()
{
  stop();
}

void task_worker_pool::stop()
{
  if (not pending_tasks.is_stopped()) {
    pending_tasks.stop();
    for (auto& w : workers) {
      w.t_handle.join();
    }
  }
}

/// \brief Wait for all the currently enqueued tasks to complete.
void task_worker_pool::wait_pending_tasks()
{
  std::packaged_task<void()> pkg_task([]() { /* do nothing */ });
  std::future<void>          fut = pkg_task.get_future();
  push_task(std::move(pkg_task));
  // blocks for enqueued task to complete.
  fut.get();
}
