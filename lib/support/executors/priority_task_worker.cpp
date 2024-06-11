/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/priority_task_worker.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

priority_task_worker::priority_task_worker(std::string                         thread_name,
                                           span<const concurrent_queue_params> task_queue_params,
                                           std::chrono::microseconds           wait_interval,
                                           os_thread_realtime_priority         prio,
                                           const os_sched_affinity_bitmask&    mask) :
  task_queue(task_queue_params, wait_interval), t_handle(thread_name, prio, mask, [this]() { run_pop_task_loop(); })
{
}

void priority_task_worker::stop()
{
  if (t_handle.running()) {
    task_queue.request_stop();
    t_handle.join();
  }
}

void priority_task_worker::run_pop_task_loop()
{
  unique_task t;

  while (task_queue.pop_blocking(t)) {
    // Run popped task
    t();

    // Note: Important to clear the task in case it has any RAII object.
    t = {};
  }

  srslog::fetch_basic_logger("ALL").info("Task worker \"{}\" finished.", this_thread_name());
}
