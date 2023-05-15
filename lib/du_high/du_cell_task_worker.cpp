/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/du_high/du_cell_task_worker.h"

using namespace srsran;

du_cell_task_worker::du_cell_task_worker(std::string                      thread_name,
                                         unsigned                         low_prio_task_queue_size,
                                         os_thread_realtime_priority      prio,
                                         const os_sched_affinity_bitmask& mask) :
  pending_low_prio_tasks(low_prio_task_queue_size), t_handle(thread_name, prio, mask, [this]() { run_pop_task_loop(); })
{
}

void du_cell_task_worker::stop()
{
  if (running.exchange(false)) {
    pending_low_prio_tasks.stop();
    t_handle.join();
  }
}

void du_cell_task_worker::run_pop_task_loop()
{
  while (running.load(std::memory_order_relaxed)) {
    unique_task* task = pending_slots.front();
    if (task != nullptr) {
      (*task)();
      pending_slots.pop();
      continue;
    }

    unique_task low_prio_task;
    if (not pending_low_prio_tasks.try_pop(low_prio_task)) {
      std::this_thread::sleep_for(std::chrono::microseconds{10});
      continue;
    }
    low_prio_task();
  }
  srslog::fetch_basic_logger("ALL").info("Cell task worker {} finished.", this_thread_name());
}
