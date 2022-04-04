/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
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

#define DEBUG 0
#define debug_thread(fmt, ...)                                                                                         \
  do {                                                                                                                 \
    if (DEBUG)                                                                                                         \
      printf(fmt, __VA_ARGS__);                                                                                        \
  } while (0)

namespace srsgnb {

task_worker::task_worker(std::string thread_name_,
                         uint32_t    queue_size,
                         bool        start_deferred,
                         int32_t     prio_,
                         uint32_t    mask_) :
  thread(std::move(thread_name_)),
  prio(prio_),
  mask(mask_),
  logger(srslog::fetch_basic_logger("POOL")),
  pending_tasks(queue_size)
{
  if (not start_deferred) {
    start(prio_, mask_);
  }
}

task_worker::~task_worker()
{
  stop();
}

void task_worker::stop()
{
  if (not pending_tasks.is_stopped()) {
    pending_tasks.stop();
    wait_thread_finish();
  }
}

void task_worker::start(int32_t prio_, uint32_t mask_)
{
  prio = prio_;
  mask = mask_;

  if (mask == 255) {
    thread::start(prio);
  } else {
    thread::start_cpu_mask(prio, mask);
  }
}

void task_worker::push_task(task_t&& task)
{
  auto ret = pending_tasks.try_push(std::move(task));
  if (ret.is_error()) {
    logger.error("Cannot push anymore tasks into the worker queue. maximum size is {}",
                 uint32_t(pending_tasks.max_size()));
    return;
  }
}

void task_worker::run_thread()
{
  t_id = std::this_thread::get_id();
  while (true) {
    bool   success;
    task_t task = pending_tasks.pop_blocking(&success);
    if (not success) {
      break;
    }
    task();
  }
  logger.info("Task worker {} finished.", thread::get_name().c_str());
}

} // namespace srsgnb
