/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "lib/du_high/du_high_executor_strategies.h"
#include "srsran/adt/static_vector.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/executors/task_worker.h"
#include <array>

namespace srsran {

struct du_high_worker_manager {
  static const uint32_t task_worker_queue_size = 10000;

  void stop()
  {
    ctrl_worker.stop();
    for (auto& w : cell_workers) {
      w.stop();
    }
    for (auto& w : ue_workers) {
      w.stop();
    }
  }

  manual_task_worker         test_worker{task_worker_queue_size};
  task_worker                ctrl_worker{"CTRL", task_worker_queue_size};
  std::array<task_worker, 2> cell_workers{{{"DU-CELL#0", task_worker_queue_size}, {"CELL#1", task_worker_queue_size}}};
  std::array<task_worker, 2> ue_workers{{{"UE#0", task_worker_queue_size}, {"UE#1", task_worker_queue_size}}};
  task_worker_executor       ctrl_exec{ctrl_worker};
  static_vector<task_worker_executor, 2> cell_execs{{cell_workers[0]}, {cell_workers[1]}};
  static_vector<task_worker_executor, 2> ue_execs{{ue_workers[0]}, {ue_workers[1]}};
  du_high_executor_mapper_impl           exec_mapper{
      std::make_unique<cell_executor_mapper>(std::initializer_list<task_executor*>{&cell_execs[0], &cell_execs[1]}),
      std::make_unique<pcell_ue_executor_mapper>(std::initializer_list<task_executor*>{&ue_execs[0], &ue_execs[1]}),
      ctrl_exec,
      ctrl_exec,
      ctrl_exec};
};

} // namespace srsran
