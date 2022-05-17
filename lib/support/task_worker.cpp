/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/srslog/srslog.h"
#include <assert.h>
#include <chrono>
#include <stdio.h>

namespace srsgnb {

task_worker::task_worker(std::string                      thread_name_,
                         uint32_t                         queue_size,
                         bool                             start_postponed,
                         os_thread_realtime_priority      prio_,
                         const os_sched_affinity_bitmask& mask_) :
  worker_name(std::move(thread_name_)),
  prio(prio_),
  mask(mask_),
  logger(srslog::fetch_basic_logger("POOL")),
  pending_tasks(queue_size),
  t_handle(start_postponed ? unique_thread{} : make_thread())
{}

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

} // namespace srsgnb
