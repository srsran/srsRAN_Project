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
#include "srsran/adt/expected.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/support/executors/priority_multiqueue_task_worker.h"
#include "srsran/support/executors/task_execution_manager.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"
#include <unordered_map>

namespace srsran {

/// \brief Affinity mask manager.
///
/// It manages the CPUs that have been used to set an affinity mask accounting for a task priority.
class affinity_mask_manager
{
public:
  /// Creates the tuned affinity mask manager with the given number of threads per core, reserves given amount of CPU
  /// cores for non priority tasks.
  explicit affinity_mask_manager(unsigned nof_threads_per_core_, unsigned nof_cores_for_non_prio_threads_) :
    nof_threads_per_core(nof_threads_per_core_),
    nof_cores_for_non_prio_threads(nof_cores_for_non_prio_threads_),
    cpu_bitset(compute_host_nof_hardware_threads())
  {
    srsran_assert(
        nof_cores_for_non_prio_threads < compute_host_nof_hardware_threads(),
        "Number of CPU cores reserved for non-priority tasks cannot exceed number of CPU cores in the machine");

    for (unsigned bit = 0; bit != nof_cores_for_non_prio_threads; ++bit) {
      non_prio_thread_mask.set(bit);
    }
    cpu_bitset.fill(0, nof_cores_for_non_prio_threads, true);
  }

  /// Default constructor.
  affinity_mask_manager() :
    nof_threads_per_core(2U),
    nof_cores_for_non_prio_threads(compute_host_nof_hardware_threads() / 2),
    cpu_bitset(compute_host_nof_hardware_threads())
  {
    cpu_bitset.fill(0, nof_cores_for_non_prio_threads, true);
  }

  /// \brief Returns an affinity mask with assigned CPU indexes.
  ///
  /// \param name Name of the task trying to reserve a CPU core.
  /// \param prio Priority of a task trying to reserve a CPU core.
  /// \return CPU affinity mask.
  os_sched_affinity_bitmask reserve_cpu(const std::string&          name,
                                        os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime())
  {
    if (prio == os_thread_realtime_priority::no_realtime()) {
      return non_prio_thread_mask;
    }
    int start_pos = std::min(size_t(cpu_bitset.find_highest()) + nof_threads_per_core, cpu_bitset.size() - 1);
    int pos       = cpu_bitset.find_lowest(start_pos, cpu_bitset.size(), false);

    if (pos == -1) {
      fmt::print("Could not set the affinity for the {} worker\n", name);
      return {};
    }

    cpu_bitset.set(pos);
    return os_sched_affinity_bitmask(pos);
  }

private:
  /// Number of threads per physical CPU.
  const unsigned nof_threads_per_core;
  /// Number of CPU cores assigned for non-priority tasks.
  const unsigned nof_cores_for_non_prio_threads;
  /// Bitmask used to keep track of reserved CPUs.
  bounded_bitset<1024> cpu_bitset;
  /// Affinity mask assigned for non-priority tasks.
  os_sched_affinity_bitmask non_prio_thread_mask;
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

  task_executor*                                           cu_cp_exec    = nullptr;
  task_executor*                                           cu_up_exec    = nullptr;
  task_executor*                                           gtpu_pdu_exec = nullptr;
  std::vector<task_executor*>                              lower_phy_tx_exec;
  std::vector<task_executor*>                              lower_phy_rx_exec;
  std::vector<task_executor*>                              lower_phy_dl_exec;
  std::vector<task_executor*>                              lower_phy_ul_exec;
  std::vector<task_executor*>                              lower_prach_exec;
  std::vector<task_executor*>                              upper_pusch_exec;
  std::vector<task_executor*>                              upper_pucch_exec;
  std::vector<task_executor*>                              upper_prach_exec;
  std::vector<task_executor*>                              upper_pdsch_exec;
  task_executor*                                           radio_exec      = nullptr;
  task_executor*                                           ru_printer_exec = nullptr;
  std::unique_ptr<task_executor>                           ru_timing_exec;
  std::vector<std::vector<std::unique_ptr<task_executor>>> ru_dl_exec;
  std::vector<std::unique_ptr<task_executor>>              ru_tx_exec;
  std::vector<std::unique_ptr<task_executor>>              ru_rx_exec;
  task_executor*                                           cu_cp_e2_exec    = nullptr;
  task_executor*                                           cu_up_e2_exec    = nullptr;
  task_executor*                                           metrics_hub_exec = nullptr;

  du_high_executor_mapper& get_du_high_executor_mapper(unsigned du_index);

  // Gets the DU-low downlink executors.
  void get_du_low_dl_executors(std::vector<task_executor*>& executors, unsigned sector_id) const;

private:
  using ru_mpsc_worker_type =
      general_task_worker<concurrent_queue_policy::locking_mpsc, concurrent_queue_wait_policy::condition_variable>;
  using ru_spsc_worker_type =
      general_task_worker<concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>;

  struct du_high_executor_storage {
    std::unique_ptr<du_high_executor_mapper> du_high_exec_mapper;
  };

  std::vector<std::unique_ptr<ru_mpsc_worker_type>> ru_mpsc_workers;
  std::vector<std::unique_ptr<ru_spsc_worker_type>> ru_spsc_workers;
  std::unique_ptr<affinity_mask_manager>            affinity_manager;
  bool                                              use_tuned_profile = false;

  std::vector<du_high_executor_storage> du_high_executors;

  std::vector<std::vector<task_executor*>> du_low_dl_executors;

  /// Manager of execution contexts and respective executors instantiated by the gNB application.
  task_execution_manager exec_mng;

  /// Helper method to create workers with non zero priority.
  void create_prio_worker(const std::string&                                                   name,
                          unsigned                                                             queue_size,
                          const std::vector<execution_config_helper::single_worker::executor>& execs,
                          os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime());

  /// Helper method to create worker pool.
  void create_worker_pool(const std::string&                                                 name,
                          unsigned                                                           nof_workers,
                          unsigned                                                           queue_size,
                          const std::vector<execution_config_helper::worker_pool::executor>& execs,
                          os_thread_realtime_priority           prio      = os_thread_realtime_priority::no_realtime(),
                          span<const os_sched_affinity_bitmask> cpu_masks = {});

  /// Helper method that creates the Control and Distributed Units executors.
  void create_du_cu_executors(bool                       is_blocking_mode_active,
                              unsigned                   nof_ul_workers,
                              unsigned                   nof_dl_workers,
                              unsigned                   nof_pdsch_workers,
                              span<const cell_appconfig> cells_cfg,
                              unsigned                   pipeline_depth);

  void create_du_low_executors(bool                       is_blocking_mode_active,
                               unsigned                   nof_ul_workers,
                               unsigned                   nof_dl_workers,
                               unsigned                   nof_pdsch_workers,
                               span<const cell_appconfig> cells_cfg,
                               unsigned                   pipeline_depth);

  /// Helper method that creates the Radio Unit executors.
  void create_ru_executors(const gnb_appconfig& appcfg);

  /// Helper method that creates the lower PHY executors.
  void create_lower_phy_executors(lower_phy_thread_profile lower_phy_profile, unsigned nof_cells);

  /// Helper method that creates the Open Fronthaul executors.
  void create_ofh_executors(span<const cell_appconfig> cells, bool is_downlink_parallelized);

  /// Assign a CPU affinity bitmask to a given worker, based on its priority.
  os_sched_affinity_bitmask calculate_affinity_mask(const std::string& worker_name, os_thread_realtime_priority prio);
};

} // namespace srsran
