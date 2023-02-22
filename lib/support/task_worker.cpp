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

#include "srsran/support/executors/task_worker.h"
#include "srsran/srslog/srslog.h"
#include <assert.h>
#include <chrono>
#include <stdio.h>

namespace srsran {

task_worker::task_worker(std::string                      thread_name_,
                         uint32_t                         queue_size,
                         bool                             start_postponed,
                         os_thread_realtime_priority      prio_,
                         const os_sched_affinity_bitmask& mask_) :
  worker_name(std::move(thread_name_)),
  prio(prio_),
  mask(mask_),
  logger(srslog::fetch_basic_logger("ALL")),
  pending_tasks(queue_size),
  t_handle(start_postponed ? unique_thread{} : make_thread())
{
}

task_worker::~task_worker()
{
  stop();
}

void task_worker::stop()
{
  if (not pending_tasks.is_stopped()) {
    pending_tasks.stop();
    t_handle.join();
  }
}

unique_thread task_worker::make_thread()
{
  auto task_func = [this]() {
    while (true) {
      bool   success;
      task_t task = pending_tasks.pop_blocking(&success);
      if (not success) {
        break;
      }
      task();
    }
    logger.info("Task worker {} finished.", t_handle.get_name());
  };
  return unique_thread{worker_name, prio, mask, task_func};
}

void task_worker::start(os_thread_realtime_priority prio_, const os_sched_affinity_bitmask& mask_)
{
  if (t_handle.running()) {
    logger.error("ERROR: Task Worker can only be started once.");
    return;
  }

  prio     = prio_;
  mask     = mask_;
  t_handle = make_thread();
}

} // namespace srsran
