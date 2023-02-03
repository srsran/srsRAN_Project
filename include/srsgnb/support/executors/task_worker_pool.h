/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/blocking_queue.h"
#include "srsgnb/support/unique_thread.h"

namespace srsgnb {

class task_worker_pool
{
public:
  task_worker_pool(unsigned                    nof_workers,
                   unsigned                    queue_size,
                   const std::string&          pool_name,
                   os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime());
  task_worker_pool(const task_worker_pool&)            = delete;
  task_worker_pool(task_worker_pool&&)                 = delete;
  task_worker_pool& operator=(const task_worker_pool&) = delete;
  task_worker_pool& operator=(task_worker_pool&&)      = delete;
  ~task_worker_pool();

  /// Stop task worker pool, if running.
  void stop();

  /// \brief Number of workers that compose the worker pool.
  unsigned nof_workers() const { return workers.size(); }

  /// \brief Number of tasks currently enqueued.
  uint32_t nof_pending_tasks() const { return pending_tasks.size(); }

  bool push_task(unique_task&& task)
  {
    auto ret = pending_tasks.try_push(std::move(task));
    if (ret.is_error()) {
      logger.error("Cannot push anymore tasks into the {} worker pool queue. maximum size is {}",
                   pool_name,
                   uint32_t(pending_tasks.max_size()));
      return false;
    }
    return true;
  }

  void push_task_blocking(unique_task&& task)
  {
    auto ret = pending_tasks.push_blocking(std::move(task));
    if (ret.is_error()) {
      logger.debug("Cannot push anymore tasks into the {} worker queue because it was closed", pool_name);
      return;
    }
  }

  /// \brief Wait for all the currently enqueued tasks to complete.
  void wait_pending_tasks();

private:
  struct worker {
    unique_thread t_handle;
  };

  std::string           pool_name;
  srslog::basic_logger& logger;

  // List of workers belonging to the worker pool.
  std::vector<worker> workers;

  // Queue of tasks.
  srsgnb::blocking_queue<unique_task> pending_tasks;
};

} // namespace srsgnb