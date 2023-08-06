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
    while (true) {
      if (not pending_tasks.pop_blocking([](const unique_task& task) { task(); })) {
        break;
      }
    }
    srslog::fetch_basic_logger("ALL").info("Task worker {} finished.", this_thread_name());
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
