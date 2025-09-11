/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "srsran/du/du_high/du_high_executor_mapper.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"
#include <array>

namespace srsran {

struct du_high_worker_manager {
  static const uint32_t task_worker_queue_size = 10000;

  du_high_worker_manager();
  ~du_high_worker_manager();
  void stop();
  void flush_pending_dl_pdus();
  void flush_pending_control_tasks();

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
  std::unique_ptr<task_executor>                   time_exec;
};

} // namespace srsran
