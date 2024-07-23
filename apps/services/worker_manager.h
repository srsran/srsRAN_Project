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

#include "../gnb/gnb_appconfig.h"
#include "../units/flexible_du/split_dynamic/dynamic_du_unit_config.h"
#include "apps/services/worker_manager_worker_getter.h"
#include "apps/units/cu_cp/cu_cp_unit_pcap_config.h"
#include "apps/units/cu_up/cu_up_unit_pcap_config.h"
#include "os_sched_affinity_manager.h"
#include "srsran/cu_up/cu_up_executor_pool.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/support/executors/task_execution_manager.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// Manages the workers of the app.
struct worker_manager : public worker_manager_executor_getter {
  worker_manager(const dynamic_du_unit_config&     du_cfg,
                 const expert_execution_appconfig& expert_appcfg,
                 cu_cp_unit_pcap_config&           cu_cp_pcap_cfg,
                 cu_up_unit_pcap_config&           cu_up_pcap_cfg,
                 unsigned                          gtpu_queue_size);

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

  task_executor*              cu_cp_exec       = nullptr;
  task_executor*              cu_up_ctrl_exec  = nullptr; ///< CU-UP executor for control
  task_executor*              cu_up_io_ul_exec = nullptr; ///< CU-UP executor for UL data flow
  std::vector<task_executor*> lower_phy_tx_exec;
  std::vector<task_executor*> lower_phy_rx_exec;
  std::vector<task_executor*> lower_phy_dl_exec;
  std::vector<task_executor*> lower_phy_ul_exec;
  std::vector<task_executor*> lower_prach_exec;
  std::vector<task_executor*> upper_pusch_exec;
  std::vector<task_executor*> upper_pusch_decoder_exec;
  std::vector<task_executor*> upper_pucch_exec;
  std::vector<task_executor*> upper_prach_exec;
  std::vector<task_executor*> upper_srs_exec;
  std::vector<task_executor*> upper_pdsch_exec;
  task_executor*              radio_exec      = nullptr;
  task_executor*              ru_printer_exec = nullptr;
  task_executor*              ru_timing_exec  = nullptr;
  std::vector<task_executor*> ru_txrx_exec;
  std::vector<task_executor*> fapi_exec;
  std::vector<task_executor*> ru_dl_exec;
  std::vector<task_executor*> ru_rx_exec;
  task_executor*              cu_cp_e2_exec    = nullptr;
  task_executor*              cu_up_e2_exec    = nullptr;
  task_executor*              metrics_hub_exec = nullptr;

  std::unique_ptr<srs_cu_up::cu_up_executor_pool> cu_up_exec_mapper;

  du_high_executor_mapper& get_du_high_executor_mapper(unsigned du_index);

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

  struct du_high_executor_storage {
    std::unique_ptr<du_high_executor_mapper> du_high_exec_mapper;
  };
  std::vector<du_high_executor_storage>    du_high_executors;
  std::vector<std::vector<task_executor*>> du_low_dl_executors;

  /// Manager of execution contexts and respective executors instantiated by the application.
  task_execution_manager exec_mng;

  os_sched_affinity_manager low_prio_affinity_mng;
  os_sched_affinity_bitmask ru_timing_mask;

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

  execution_config_helper::worker_pool create_low_prio_workers(const expert_execution_appconfig& expert_appcfg);
  void                                 create_low_prio_executors(const expert_execution_appconfig& expert_appcfg,
                                                                 const cu_cp_unit_pcap_config&     cu_cp_pcaps,
                                                                 const cu_up_unit_pcap_config&     cu_up_pcaps,
                                                                 const du_high_unit_pcap_config&   du_pcaps,
                                                                 unsigned                          nof_cells,
                                                                 unsigned                          gtpu_queue_size,
                                                                 bool                              rt_mode);
  void                                 associate_low_prio_executors();

  std::vector<execution_config_helper::single_worker> create_fapi_workers(unsigned nof_cells);

  std::vector<execution_config_helper::priority_multiqueue_worker> create_du_hi_slot_workers(unsigned nof_cells,
                                                                                             bool     rt_mode);

  /// Helper method that creates the Distributed Unit executors.
  void create_du_executors(bool                      is_blocking_mode_active,
                           unsigned                  nof_cells,
                           const du_low_unit_config& du_low,
                           const fapi_unit_config&   fapi_cfg);

  /// Helper method that creates the low Distributed Unit executors.
  void create_du_low_executors(bool     is_blocking_mode_active,
                               unsigned nof_ul_workers,
                               unsigned nof_dl_workers,
                               unsigned nof_pusch_decoder_workers,
                               unsigned nof_cells);

  /// Helper method that creates the Radio Unit executors.
  void
  create_ru_executors(const std::variant<ru_sdr_unit_config, ru_ofh_unit_parsed_config, ru_dummy_unit_config>& ru_cfg,
                      const du_high_unit_config&                                                               du_high);

  /// Helper method that creates the lower PHY executors.
  void create_lower_phy_executors(lower_phy_thread_profile lower_phy_profile, unsigned nof_cells);

  /// Helper method that creates the Open Fronthaul executors.
  void create_ofh_executors(const ru_ofh_unit_expert_execution_config& ru_cfg, span<const unsigned> cell_dl_antennas);
};

} // namespace srsran
