/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/support/executors/manual_task_worker.h"

namespace srsgnb {

struct du_high_worker_manager {
  static const uint32_t task_worker_queue_size = 10000;

  void stop()
  {
    for (auto& w : dl_workers) {
      w.stop();
    }
    for (auto& w : ul_workers) {
      w.stop();
    }
  }

  manual_task_worker ctrl_worker{task_worker_queue_size};
  task_worker        dl_workers[2] = {{"DU-DL#0", task_worker_queue_size}, {"DU-DL#1", task_worker_queue_size}};
  task_worker        ul_workers[2] = {{"DU-UL#0", task_worker_queue_size}, {"DU-UL#1", task_worker_queue_size}};
  static_vector<task_worker_executor, 2> dl_execs{{dl_workers[0]}, {dl_workers[1]}};
  static_vector<task_worker_executor, 2> ul_execs{{ul_workers[0]}, {ul_workers[1]}};
  pcell_ul_executor_mapper               ul_exec_mapper{{&ul_execs[0], &ul_execs[1]}};
  cell_dl_executor_mapper                dl_exec_mapper{{&dl_execs[0], &dl_execs[1]}};
};

} // namespace srsgnb