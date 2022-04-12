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

namespace srsgnb {

task_worker::task_worker(std::string thread_name_, uint32_t queue_size, bool start_postponed, int prio_, int mask_) :
  prio(prio_),
  mask(mask_),
  logger(srslog::fetch_basic_logger("POOL")),
  t_handle(std::move(thread_name_)),
  pending_tasks(queue_size)
{
  if (not start_postponed) {
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
    t_handle.join();
  }
}

void task_worker::start(int prio_, int mask_)
{
  prio = prio_;
  mask = mask_;

  t_handle.start_cpu_mask(prio, mask, [this]() {
    while (true) {
      bool   success;
      task_t task = pending_tasks.pop_blocking(&success);
      if (not success) {
        break;
      }
      task();
    }
    logger.info("Task worker {} finished.", t_handle.get_name());
  });
}

} // namespace srsgnb
