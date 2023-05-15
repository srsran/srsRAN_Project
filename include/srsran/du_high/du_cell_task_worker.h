/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rigtorp/SPSCQueue.h"
#include "srsran/adt/blocking_queue.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/unique_thread.h"

namespace srsran {

class du_cell_task_worker
{
public:
  du_cell_task_worker(std::string                      thread_name,
                      unsigned                         low_prio_task_queue_size,
                      os_thread_realtime_priority      prio = os_thread_realtime_priority::no_realtime(),
                      const os_sched_affinity_bitmask& mask = {});
  ~du_cell_task_worker() { stop(); }

  /// Stop task worker, if running.
  void stop();

  SRSRAN_NODISCARD bool push_slot_indication_task(unique_task task) { return pending_slots.try_push(std::move(task)); }

  SRSRAN_NODISCARD bool push_low_prio_task(unique_task task)
  {
    return pending_low_prio_tasks.try_push(std::move(task)).has_value();
  }

  /// Maximum number of pending tasks the task FIFO can hold.
  unsigned max_pending_tasks() const { return pending_slots.capacity() + pending_low_prio_tasks.max_size(); }

  /// Get worker thread id.
  std::thread::id get_id() const { return t_handle.get_id(); }

  /// Get worker thread name.
  const char* worker_name() const { return t_handle.get_name(); }

private:
  void run_pop_task_loop();

  // Queue of slot indication tasks.
  rigtorp::SPSCQueue<unique_task> pending_slots{8U};

  // Queue of tasks that are not slot indication tasks. These tasks are treated with lower priority.
  blocking_queue<unique_task> pending_low_prio_tasks;

  std::atomic<bool> running{true};

  // Worker thread
  unique_thread t_handle;
};

/// Executor for low priority tasks of the du_cell_task_worker.
class cell_low_prio_worker_executor : public task_executor
{
public:
  cell_low_prio_worker_executor() = default;

  cell_low_prio_worker_executor(du_cell_task_worker& worker_, bool report_on_push_failure = true) :
    worker(&worker_), report_on_failure(report_on_push_failure)
  {
  }

  bool execute(unique_task task) override
  {
    if (worker->get_id() == std::this_thread::get_id()) {
      // Same thread. Run task right away.
      task();
      return true;
    }
    return defer(std::move(task));
  }

  bool defer(unique_task task) override
  {
    if (not worker->push_low_prio_task(std::move(task))) {
      if (report_on_failure) {
        srslog::fetch_basic_logger("ALL").error("Cannot push more tasks into the {} worker queue. Maximum size is {}",
                                                worker->worker_name(),
                                                worker->max_pending_tasks());
      }
      return false;
    }
    return true;
  }

private:
  du_cell_task_worker* worker            = nullptr;
  bool                 report_on_failure = true;
};

/// Executor for low priority tasks of the du_cell_task_worker.
class cell_slot_ind_worker_executor : public task_executor
{
public:
  cell_slot_ind_worker_executor() = default;

  cell_slot_ind_worker_executor(du_cell_task_worker& worker_, bool report_on_push_failure = true) :
    worker(&worker_), report_on_failure(report_on_push_failure)
  {
  }

  bool execute(unique_task task) override
  {
    if (worker->get_id() == std::this_thread::get_id()) {
      // Same thread. Run task right away.
      task();
      return true;
    }
    return defer(std::move(task));
  }

  bool defer(unique_task task) override
  {
    if (not worker->push_slot_indication_task(std::move(task))) {
      if (report_on_failure) {
        srslog::fetch_basic_logger("ALL").error("Cannot push more slot indication tasks into the {} worker queue.",
                                                worker->worker_name());
      }
      return false;
    }
    return true;
  }

private:
  du_cell_task_worker* worker            = nullptr;
  bool                 report_on_failure = true;
};

} // namespace srsran