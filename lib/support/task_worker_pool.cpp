/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/task_worker_pool.h"
#include <future>

using namespace srsran;

task_worker_pool::task_worker_pool(unsigned                    nof_workers_,
                                   unsigned                    queue_size,
                                   const std::string&          worker_name_prefix,
                                   os_thread_realtime_priority prio_) :
  logger(srslog::fetch_basic_logger("ALL")), workers(nof_workers_), pending_tasks(queue_size)
{
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
    workers[i].t_handle = unique_thread{fmt::format("{}#{}", worker_name_prefix, i), prio_, task_func};
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
