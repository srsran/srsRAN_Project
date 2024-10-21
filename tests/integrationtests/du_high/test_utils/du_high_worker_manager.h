/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/du/du_high/du_high_executor_mapper.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"
#include <array>

namespace srsran {

struct du_high_worker_manager {
  static const uint32_t task_worker_queue_size = 10000;

  du_high_worker_manager();
  void stop() { worker_pool.stop(); }
  void flush_pending_dl_pdus();

  manual_task_worker        test_worker{task_worker_queue_size};
  priority_task_worker_pool worker_pool{
      "POOL",
      3,
      std::array<concurrent_queue_params, 2>{
          concurrent_queue_params{concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
          concurrent_queue_params{concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}}};
  priority_task_worker_pool_executor               high_prio_exec{enqueue_priority::max, worker_pool};
  priority_task_worker_pool_executor               low_prio_exec{enqueue_priority::max - 1, worker_pool};
  std::unique_ptr<srs_du::du_high_executor_mapper> exec_mapper;
};

} // namespace srsran
