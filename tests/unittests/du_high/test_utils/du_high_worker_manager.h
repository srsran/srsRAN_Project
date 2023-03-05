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

#include "lib/du_high/du_high_executor_strategies.h"
#include "srsran/adt/static_vector.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/executors/task_worker.h"

namespace srsran {

struct du_high_worker_manager {
  static const uint32_t task_worker_queue_size = 10000;

  void stop()
  {
    for (auto& w : cell_workers) {
      w.stop();
    }
    for (auto& w : ue_workers) {
      w.stop();
    }
  }

  manual_task_worker ctrl_worker{task_worker_queue_size};
  task_worker        cell_workers[2] = {{"DU-CELL#0", task_worker_queue_size}, {"CELL#1", task_worker_queue_size}};
  task_worker        ue_workers[2]   = {{"UE#0", task_worker_queue_size}, {"UE#1", task_worker_queue_size}};
  static_vector<task_worker_executor, 2> cell_execs{{cell_workers[0]}, {cell_workers[1]}};
  static_vector<task_worker_executor, 2> ue_execs{{ue_workers[0]}, {ue_workers[1]}};
  pcell_ue_executor_mapper               ue_exec_mapper{{&ue_execs[0], &ue_execs[1]}};
  cell_executor_mapper                   cell_exec_mapper{{&cell_execs[0], &cell_execs[1]}, false};
};

} // namespace srsran
