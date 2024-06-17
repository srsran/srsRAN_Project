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

#include "apps/services/os_sched_affinity_manager.h"
#include "apps/services/worker_manager_worker_getter.h"
#include "apps/units/cu_cp/cu_cp_unit_pcap_config.h"
#include "apps/units/cu_up/cu_up_unit_pcap_config.h"
#include "cu_appconfig.h"
#include "srsran/cu_up/cu_up_executor_pool.h"
#include "srsran/support/executors/task_execution_manager.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// Manages the workers of the app.
struct cu_worker_manager : public worker_manager_executor_getter {
  cu_worker_manager(const cu_appconfig&    appcfg,
                    cu_cp_unit_pcap_config cu_cp_pcap_cfg,
                    cu_up_unit_pcap_config cu_up_pcap_cfg,
                    unsigned               gtpu_queue_size);

  void stop();

  /// cu-cp ctrl exec points to general ctrl_worker
  /// cu-up ue exec points to the general ue_worker
  ///
  /// The handler side is responsible for executor dispatching:
  /// - ngap::handle_message calls cu-cp ctrl exec
  /// - f1ap_cu::handle_message calls cu-cp ctrl exec
  /// - e1ap_cu_cp::handle_message calls cu-cp ctrl exec
  /// - e1ap_cu_up::handle_message calls cu-up ue exec

  task_executor* cu_cp_exec       = nullptr;
  task_executor* cu_up_ctrl_exec  = nullptr; ///< CU-UP executor for control
  task_executor* cu_up_io_ul_exec = nullptr; ///< CU-UP executor for UL socket transmission
  task_executor* cu_cp_e2_exec    = nullptr;
  task_executor* cu_up_e2_exec    = nullptr;
  task_executor* metrics_hub_exec = nullptr;

  std::unique_ptr<srs_cu_up::cu_up_executor_pool> cu_up_exec_mapper;

  // Gets the DU-low downlink executors.
  void get_du_low_dl_executors(std::vector<task_executor*>& executors, unsigned sector_id) const;

  /// Get executor based on the name.
  task_executor* find_executor(const std::string& name) const
  {
    auto it = exec_mng.executors().find(name);
    return it != exec_mng.executors().end() ? it->second : nullptr;
  }

  task_executor& get_executor(const std::string& name) const override { return *exec_mng.executors().at(name); }

  worker_manager_executor_getter* get_executor_getter() { return this; }

private:
  static const unsigned nof_cu_up_ue_strands = 16;

  /// Manager of execution contexts and respective executors instantiated by the application.
  task_execution_manager exec_mng;

  os_sched_affinity_manager low_prio_affinity_mng;

  /// CPU affinity bitmask manager per cell.
  std::vector<os_sched_affinity_manager> affinity_mng;

  /// Helper method to create workers with non zero priority.
  void create_prio_worker(const std::string&                                    name,
                          unsigned                                              queue_size,
                          const std::vector<execution_config_helper::executor>& execs,
                          const os_sched_affinity_bitmask&                      mask,
                          os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime());

  /// Helper method to create worker pool.
  void create_worker_pool(const std::string&                                    name,
                          unsigned                                              nof_workers,
                          unsigned                                              queue_size,
                          const std::vector<execution_config_helper::executor>& execs,
                          os_thread_realtime_priority           prio      = os_thread_realtime_priority::no_realtime(),
                          span<const os_sched_affinity_bitmask> cpu_masks = {});

  execution_config_helper::worker_pool create_low_prio_workers(const cu_appconfig& appcfg);
  void                                 create_low_prio_executors(const cu_appconfig&           appcfg,
                                                                 const cu_cp_unit_pcap_config& cu_cp_pcap_cfg,
                                                                 const cu_up_unit_pcap_config& cu_up_pcap_cfg,
                                                                 unsigned                      gtpu_queue_size);
  void                                 associate_low_prio_executors();
};

} // namespace srsran
