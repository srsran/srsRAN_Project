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

#pragma once

#include "srsran/support/executors/manual_task_worker.h"

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
  pcell_ul_executor_mapper               ue_exec_mapper{{&ue_execs[0], &ue_execs[1]}};
  cell_executor_mapper                   cell_exec_mapper{{&cell_execs[0], &cell_execs[1]}, false};
};

} // namespace srsran
