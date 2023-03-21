/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/task_worker.h"
#include "srsran/srslog/srslog.h"
#include <assert.h>
#include <chrono>
#include <stdio.h>

namespace srsran {

task_worker::task_worker(std::string                      thread_name,
                         uint32_t                         queue_size,
                         os_thread_realtime_priority      prio,
                         const os_sched_affinity_bitmask& mask) :
  logger(srslog::fetch_basic_logger("ALL")), pending_tasks(queue_size), t_handle(thread_name, prio, mask, [this]() {
    while (true) {
      bool   success;
      task_t task = pending_tasks.pop_blocking(&success);
      if (not success) {
        break;
      }
      task();
    }
    logger.info("Task worker {} finished.", t_handle.get_name());
  })
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

void task_worker::wait_pending_tasks()
{
  std::packaged_task<void()> pkg_task([]() { /* do nothing */ });
  std::future<void>          fut = pkg_task.get_future();
  push_task(std::move(pkg_task));
  // blocks for enqueued task to complete.
  fut.get();
}

} // namespace srsran
