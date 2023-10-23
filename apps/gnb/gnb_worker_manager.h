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

#include "gnb_appconfig.h"
#include "gnb_os_sched_affinity_manager.h"
#include "srsran/adt/expected.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/support/executors/task_execution_manager.h"
#include "srsran/support/executors/task_executor.h"
#include <unordered_map>

namespace srsran {

/// Manages the workers of the app.
struct worker_manager {
  explicit worker_manager(const gnb_appconfig& appcfg);

  void stop();

  /// du ctrl exec points to general ctrl_worker
  /// du ue exec points to the general ue_worker
  /// cu-cp ctrl exec points to general ctrl_worker (just like the du ctrl exec)
  /// cu-up ue exec points to the general ue_worker (just like the du ue exec)
  ///
  /// The handler side is responsible for executor dispatching:
  /// - ngap::handle_message calls cu-cp ctrl exec
  /// - f1ap_cu::handle_message calls cu-cp ctrl exec
  /// - e1ap_cu_cp::handle_message calls cu-cp ctrl exec
  /// - e1ap_cu_up::handle_message calls cu-up ue exec

  task_executor*                           cu_cp_exec    = nullptr;
  task_executor*                           cu_up_exec    = nullptr;
  task_executor*                           gtpu_pdu_exec = nullptr;
  std::vector<task_executor*>              lower_phy_tx_exec;
  std::vector<task_executor*>              lower_phy_rx_exec;
  std::vector<task_executor*>              lower_phy_dl_exec;
  std::vector<task_executor*>              lower_phy_ul_exec;
  std::vector<task_executor*>              lower_prach_exec;
  std::vector<task_executor*>              upper_pusch_exec;
  std::vector<task_executor*>              upper_pusch_decoder_exec;
  std::vector<task_executor*>              upper_pucch_exec;
  std::vector<task_executor*>              upper_prach_exec;
  std::vector<task_executor*>              upper_pdsch_exec;
  task_executor*                           radio_exec      = nullptr;
  task_executor*                           ru_printer_exec = nullptr;
  task_executor*                           ru_timing_exec  = nullptr;
  std::vector<std::vector<task_executor*>> ru_dl_exec;
  std::vector<task_executor*>              ru_tx_exec;
  std::vector<task_executor*>              ru_rx_exec;
  task_executor*                           cu_cp_e2_exec    = nullptr;
  task_executor*                           cu_up_e2_exec    = nullptr;
  task_executor*                           metrics_hub_exec = nullptr;

  du_high_executor_mapper& get_du_high_executor_mapper(unsigned du_index);

  // Gets the DU-low downlink executors.
  void get_du_low_dl_executors(std::vector<task_executor*>& executors, unsigned sector_id) const;

private:
  struct du_high_executor_storage {
    std::unique_ptr<du_high_executor_mapper> du_high_exec_mapper;
  };
  std::vector<du_high_executor_storage>    du_high_executors;
  std::vector<std::vector<task_executor*>> du_low_dl_executors;

  /// Manager of execution contexts and respective executors instantiated by the gNB application.
  task_execution_manager exec_mng;

  /// CPU affinity bitmask manager.
  gnb_os_sched_affinity_manager affinity_mng;

  /// Helper method to create workers with non zero priority.
  void create_prio_worker(const std::string&                                                   name,
                          unsigned                                                             queue_size,
                          const std::vector<execution_config_helper::single_worker::executor>& execs,
                          const os_sched_affinity_bitmask&                                     mask,
                          os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime());

  /// Helper method to create worker pool.
  void create_worker_pool(const std::string&                                                 name,
                          unsigned                                                           nof_workers,
                          unsigned                                                           queue_size,
                          const std::vector<execution_config_helper::worker_pool::executor>& execs,
                          os_thread_realtime_priority           prio      = os_thread_realtime_priority::no_realtime(),
                          span<const os_sched_affinity_bitmask> cpu_masks = {});

  /// Helper method that creates the Control and Distributed Units executors.
  void create_du_cu_executors(const gnb_appconfig& appcfg);

  /// Helper method that creates the low Distributed Unit executors.
  void create_du_low_executors(bool                       is_blocking_mode_active,
                               unsigned                   nof_ul_workers,
                               unsigned                   nof_dl_workers,
                               unsigned                   nof_pdsch_workers,
                               unsigned                   nof_pusch_decoder_workers,
                               span<const cell_appconfig> cells_cfg,
                               unsigned                   pipeline_depth);

  /// Helper method that creates the Radio Unit executors.
  void create_ru_executors(const gnb_appconfig& appcfg);

  /// Helper method that creates the lower PHY executors.
  void create_lower_phy_executors(lower_phy_thread_profile lower_phy_profile, unsigned nof_cells);

  /// Helper method that creates the Open Fronthaul executors.
  void create_ofh_executors(span<const cell_appconfig> cells, bool is_downlink_parallelized);
};

} // namespace srsran
