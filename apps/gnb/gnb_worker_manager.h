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
#include "srsran/adt/expected.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/support/executors/priority_multiqueue_task_worker.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"
#include <unordered_map>

namespace srsran {

/// Affinity mask manager.
///
/// It manages the CPUs that have been used to set an affinity mask.
class affinity_mask_manager
{
public:
  /// Creates the affinity mask manager with the given number of threads per core.
  affinity_mask_manager(unsigned nof_threads_per_core_ = 2U) :
    nof_threads_per_core(nof_threads_per_core_), cpu_bitset(compute_host_nof_hardware_threads())
  {
  }

  /// \brief Returns a free CPU index or an error in case that all the CPUs are being used.
  ///
  /// This manager returns the index of one physical CPU.
  expected<unsigned> reserve_cpu_index()
  {
    if (cpu_bitset.none()) {
      cpu_bitset.set(0);
      return {0U};
    }

    int pos = cpu_bitset.find_lowest(
        cpu_bitset.find_highest() + nof_threads_per_core, cpu_bitset.size() - nof_threads_per_core, false);

    if (pos == -1) {
      return default_error_t{};
    }

    cpu_bitset.set(pos);
    return pos;
  }

private:
  const unsigned       nof_threads_per_core;
  bounded_bitset<1024> cpu_bitset;
};

/// Manages the workers of the app.
struct worker_manager {
  worker_manager(const gnb_appconfig& appcfg);

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

  std::unique_ptr<task_executor>                           cu_cp_exec;
  std::unique_ptr<task_executor>                           cu_up_exec;
  std::unique_ptr<task_executor>                           gtpu_pdu_exec;
  std::vector<std::unique_ptr<task_executor>>              lower_phy_tx_exec;
  std::vector<std::unique_ptr<task_executor>>              lower_phy_rx_exec;
  std::vector<std::unique_ptr<task_executor>>              lower_phy_dl_exec;
  std::vector<std::unique_ptr<task_executor>>              lower_phy_ul_exec;
  std::vector<std::unique_ptr<task_executor>>              lower_prach_exec;
  std::vector<std::unique_ptr<task_executor>>              upper_pusch_exec;
  std::vector<std::unique_ptr<task_executor>>              upper_pucch_exec;
  std::vector<std::unique_ptr<task_executor>>              upper_prach_exec;
  std::vector<std::unique_ptr<task_executor>>              upper_pdsch_exec;
  std::unique_ptr<task_executor>                           radio_exec;
  std::unique_ptr<task_executor>                           ru_printer_exec;
  std::unique_ptr<task_executor>                           ru_timing_exec;
  std::vector<std::vector<std::unique_ptr<task_executor>>> ru_dl_exec;
  std::vector<std::unique_ptr<task_executor>>              ru_tx_exec;
  std::vector<std::unique_ptr<task_executor>>              ru_rx_exec;

  std::unordered_map<std::string, std::unique_ptr<task_executor>> task_execs;

  du_high_executor_mapper& get_du_high_executor_mapper(unsigned du_index);

  // Gets the DU-low downlink executors.
  void get_du_low_dl_executors(std::vector<task_executor*>& executors, unsigned sector_id) const;

private:
  using du_cell_worker_type  = priority_multiqueue_task_worker<task_queue_policy::spsc, task_queue_policy::blocking>;
  using gnb_ctrl_worker_type = priority_multiqueue_task_worker<task_queue_policy::spsc, task_queue_policy::blocking>;

  struct du_high_executor_storage {
    std::unique_ptr<task_executor>           du_ctrl_exec;
    std::unique_ptr<task_executor>           du_timer_exec;
    std::unique_ptr<task_executor>           du_ue_exec;
    std::unique_ptr<task_executor>           du_cell_exec;
    std::unique_ptr<task_executor>           du_slot_exec;
    std::unique_ptr<du_high_executor_mapper> du_high_exec_mapper;
  };

  std::unique_ptr<du_cell_worker_type>                               du_cell_worker;
  std::unique_ptr<gnb_ctrl_worker_type>                              gnb_ctrl_worker;
  std::unordered_map<std::string, std::unique_ptr<task_worker>>      workers;
  std::unordered_map<std::string, std::unique_ptr<task_worker_pool>> worker_pools;
  affinity_mask_manager                                              affinity_manager;

  std::vector<du_high_executor_storage> du_high_executors;

  std::vector<std::vector<std::unique_ptr<task_executor>>> du_low_dl_executors;

  /// helper method to create workers
  template <typename... Args>
  void create_worker(const std::string& name, Args&&... args);

  /// helper method to create worker pool
  void create_worker_pool(const std::string&          name,
                          size_t                      nof_workers,
                          size_t                      queue_size,
                          os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime());

  /// Helper method that creates the Control and Distributed Units executors.
  void create_du_cu_executors(bool     is_blocking_mode_active,
                              unsigned nof_ul_workers,
                              unsigned nof_dl_workers,
                              unsigned nof_pdsch_workers,
                              unsigned nof_cells);

  void create_du_low_executors(bool     is_blocking_mode_active,
                               unsigned nof_ul_workers,
                               unsigned nof_dl_workers,
                               unsigned nof_pdsch_workers,
                               unsigned nof_cells);

  /// Helper method that creates the Radio Unit executors.
  void create_ru_executors(const gnb_appconfig& appcfg);

  /// Helper method that creates the lower PHY executors.
  void create_lower_phy_executors(lower_phy_thread_profile lower_phy_profile, unsigned nof_cells);

  /// Helper method that creates the Open Fronthaul executors.
  void create_ofh_executors(span<const cell_appconfig> cells, bool is_downlink_parallelized);

  /// Helper method that creates and returns an executor.
  std::unique_ptr<task_executor>
  create_ofh_executor(const std::string& name, unsigned priority_from_max, unsigned queue_size);
};

} // namespace srsran