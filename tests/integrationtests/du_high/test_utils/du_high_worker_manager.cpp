/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_high_worker_manager.h"
#include <future>

using namespace srsran;

static constexpr unsigned nof_ue_strands   = 2;
static constexpr unsigned nof_cell_strands = 2;

du_high_worker_manager::du_high_worker_manager()
{
  using exec_config = srs_du::du_high_executor_config;

  exec_config cfg;
  cfg.cell_executors = exec_config::strand_based_worker_pool{nof_cell_strands, task_worker_queue_size, &high_prio_exec};
  cfg.ue_executors   = {exec_config::ue_executor_config::map_policy::per_cell,
                        nof_ue_strands,
                        task_worker_queue_size,
                        task_worker_queue_size,
                        &low_prio_exec};
  cfg.ctrl_executors = {task_worker_queue_size, &high_prio_exec};
  cfg.is_rt_mode_enabled = true;
  cfg.trace_exec_tasks   = false;

  exec_mapper = srs_du::create_du_high_executor_mapper(cfg);
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
