/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/task_worker.h"
#include <future>

using namespace srsran;

template <concurrent_queue_policy QueuePolicy, concurrent_queue_wait_policy WaitPolicy>
general_task_worker<QueuePolicy, WaitPolicy>::~general_task_worker()
{
  stop();
}

template <concurrent_queue_policy QueuePolicy, concurrent_queue_wait_policy WaitPolicy>
void general_task_worker<QueuePolicy, WaitPolicy>::stop()
{
  if (t_handle.running()) {
    pending_tasks.request_stop();
    t_handle.join();
  }
}

template <concurrent_queue_policy QueuePolicy, concurrent_queue_wait_policy WaitPolicy>
unique_function<void()> general_task_worker<QueuePolicy, WaitPolicy>::make_blocking_pop_task()
{
  return [this]() {
    auto& logger = srslog::fetch_basic_logger("ALL");
    logger.info("Task worker \"{}\" started...", this_thread_name());

    unique_task t;
    while (pending_tasks.pop_blocking(t)) {
      // Call task.
      t();

      // Note: Important to delete the task before the next pop attempt, in case the capture has RAII objects.
      t = {};
    }
    logger.info("Task worker \"{}\" finished.", this_thread_name());
  };
}

template <concurrent_queue_policy QueuePolicy, concurrent_queue_wait_policy WaitPolicy>
void general_task_worker<QueuePolicy, WaitPolicy>::wait_pending_tasks()
{
  std::packaged_task<void()> pkg_task([]() { /* do nothing */ });
  std::future<void>          fut = pkg_task.get_future();
  push_task_blocking(std::move(pkg_task));
  // blocks for enqueued task to complete.
  fut.get();
}

template class srsran::general_task_worker<concurrent_queue_policy::locking_mpsc,
                                           concurrent_queue_wait_policy::condition_variable>;
template class srsran::general_task_worker<concurrent_queue_policy::locking_mpsc, concurrent_queue_wait_policy::sleep>;
template class srsran::general_task_worker<concurrent_queue_policy::locking_mpmc,
                                           concurrent_queue_wait_policy::condition_variable>;
template class srsran::general_task_worker<concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>;
template class srsran::general_task_worker<concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep>;
