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
#include <future>

using namespace srsran;

static unique_function<void()> make_blocking_pop_task(blocking_queue<unique_task>& queue)
{
  return [&queue]() {
    while (true) {
      bool        success;
      unique_task task = queue.pop_blocking(&success);
      if (not success) {
        break;
      }
      task();
    }
    srslog::fetch_basic_logger("ALL").info("Task worker {} finished.", this_thread_name());
  };
}

static unique_function<void()> make_wait_pop_task(blocking_queue<unique_task>& queue,
                                                  std::chrono::microseconds    duration)
{
  return [&queue, duration]() {
    blocking_queue<unique_task>::result ret = blocking_queue<unique_task>::result::timeout;
    while (ret != blocking_queue<unique_task>::result::failed) {
      unique_task task;
      ret = queue.pop_wait_for(task, duration);
      if (ret == blocking_queue<unique_task>::result::success) {
        task();
      }
    }
    srslog::fetch_basic_logger("ALL").info("Task worker {} finished.", this_thread_name());
  };
}

task_worker::task_worker(std::string                      thread_name,
                         unsigned                         queue_size,
                         os_thread_realtime_priority      prio,
                         const os_sched_affinity_bitmask& mask,
                         std::chrono::microseconds        pop_wait_timeout) :
  pending_tasks(queue_size),
  t_handle(thread_name,
           prio,
           mask,
           pop_wait_timeout.count() == 0 ? make_blocking_pop_task(pending_tasks)
                                         : make_wait_pop_task(pending_tasks, pop_wait_timeout))
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
  push_task_blocking(std::move(pkg_task));
  // blocks for enqueued task to complete.
  fut.get();
}
