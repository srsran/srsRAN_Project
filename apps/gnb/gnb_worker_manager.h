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

#include "gnb_appconfig.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/support/executors/priority_multiqueue_task_worker.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"
#include <unordered_map>

namespace srsran {

/// Manages the workers of the app.
struct worker_manager {
  worker_manager(const gnb_appconfig& appcfg);

  void stop();

  /*
  du ctrl exec points to general ctrl_worker
  du ue exec points to the general ue_worker
  cu-cp ctrl exec points to general ctrl_worker (just like the du ctrl exec)
  cu-up ue exec points to the general ue_worker (just like the du ue exec)

  The handler side is responsible for executor dispatching:
  - ngap::handle_message calls cu-cp ctrl exec
  - f1ap_cu::handle_message calls cu-cp ctrl exec
  - e1ap_cu_cp::handle_message calls cu-cp ctrl exec
  - e1ap_cu_up::handle_message calls cu-up ue exec
  */

  std::unique_ptr<task_executor> cu_cp_exec;
  std::unique_ptr<task_executor> cu_up_exec;
  std::unique_ptr<task_executor> gtpu_pdu_exec;
  std::unique_ptr<task_executor> du_ctrl_exec;
  std::unique_ptr<task_executor> du_timer_exec;
  std::unique_ptr<task_executor> du_ue_exec;
  std::unique_ptr<task_executor> du_cell_exec;
  std::unique_ptr<task_executor> du_slot_exec;
  std::unique_ptr<task_executor> lower_phy_tx_exec;
  std::unique_ptr<task_executor> lower_phy_rx_exec;
  std::unique_ptr<task_executor> lower_phy_dl_exec;
  std::unique_ptr<task_executor> lower_phy_ul_exec;
  std::unique_ptr<task_executor> lower_prach_exec;
  std::unique_ptr<task_executor> upper_dl_exec;
  std::unique_ptr<task_executor> upper_pusch_exec;
  std::unique_ptr<task_executor> upper_pucch_exec;
  std::unique_ptr<task_executor> upper_prach_exec;
  std::unique_ptr<task_executor> upper_pdsch_exec;
  std::unique_ptr<task_executor> radio_exec;
  std::unique_ptr<task_executor> ru_printer_exec;
  std::unique_ptr<task_executor> ru_timing_exec;
  std::unique_ptr<task_executor> ru_tx_exec;
  std::unique_ptr<task_executor> ru_rx_exec;

  std::unordered_map<std::string, std::unique_ptr<task_executor>> task_execs;
  std::unique_ptr<du_high_executor_mapper>                        du_high_exec_mapper;

private:
  using du_cell_worker_type  = priority_multiqueue_task_worker<task_queue_policy::spsc, task_queue_policy::blocking>;
  using gnb_ctrl_worker_type = priority_multiqueue_task_worker<task_queue_policy::spsc, task_queue_policy::blocking>;

  std::unique_ptr<du_cell_worker_type>                               du_cell_worker;
  std::unique_ptr<gnb_ctrl_worker_type>                              gnb_ctrl_worker;
  std::unordered_map<std::string, std::unique_ptr<task_worker>>      workers;
  std::unordered_map<std::string, std::unique_ptr<task_worker_pool>> worker_pools;

  // helper method to create workers
  template <typename... Args>
  void create_worker(const std::string& name, Args&&... args);

  // helper method to create worker pool
  void create_worker_pool(const std::string&          name,
                          size_t                      nof_workers,
                          size_t                      queue_size,
                          os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime());
  void create_executors(bool                               blocking_mode_active,
                        optional<lower_phy_thread_profile> lower_phy_profile,
                        unsigned                           nof_ul_workers);
};

} // namespace srsran