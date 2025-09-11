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

#include "du_high_worker_manager.h"
#include "srsran/support/executors/strand_executor.h"
#include <future>

using namespace srsran;

static constexpr unsigned nof_ue_strands   = 2;
static constexpr unsigned nof_cell_strands = 2;

du_high_worker_manager::du_high_worker_manager()
{
  using exec_config = srs_du::du_high_executor_config;

  exec_config cfg;
  cfg.cell_executors = exec_config::strand_based_worker_pool{
      nof_cell_strands, task_worker_queue_size, std::vector<task_executor*>{&high_prio_exec}};
  cfg.ue_executors       = {exec_config::ue_executor_config::map_policy::per_cell,
                            nof_ue_strands,
                            task_worker_queue_size,
                            task_worker_queue_size,
                            &low_prio_exec};
  cfg.ctrl_executors     = {task_worker_queue_size, &high_prio_exec};
  cfg.is_rt_mode_enabled = true;
  cfg.trace_exec_tasks   = false;

  exec_mapper = srs_du::create_du_high_executor_mapper(cfg);

  time_exec = std::make_unique<task_strand<priority_task_worker_pool_executor, concurrent_queue_policy::lockfree_mpmc>>(
      high_prio_exec, 128);
}

du_high_worker_manager::~du_high_worker_manager()
{
  stop();
}

void du_high_worker_manager::stop()
{
  worker_pool.stop();
}

void du_high_worker_manager::flush_pending_dl_pdus()
{
  std::vector<std::promise<void>> promises(nof_ue_strands);
  std::vector<std::future<void>>  futures;
  for (unsigned i = 0; i != nof_ue_strands; ++i) {
    futures.emplace_back(promises[i].get_future());
    bool ret =
        exec_mapper->ue_mapper().f1u_dl_pdu_executor(to_du_ue_index(i)).defer([p = &promises[i]]() { p->set_value(); });
    report_fatal_error_if_not(ret, "unable to dispatch task");
  }

  for (unsigned i = 0; i != nof_ue_strands; ++i) {
    futures[i].get();
  }
}

void du_high_worker_manager::flush_pending_control_tasks()
{
  std::promise<void> prom;
  std::future<void>  fut = prom.get_future();
  bool               ret = exec_mapper->du_control_executor().defer([&prom]() { prom.set_value(); });
  report_fatal_error_if_not(ret, "unable to dispatch task");
  fut.wait();
}
