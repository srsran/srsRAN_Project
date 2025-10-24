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

#pragma once

#include "pcap_executor_mapper.h"
#include "worker_manager_config.h"
#include "srsran/cu_cp/cu_cp_executor_mapper.h"
#include "srsran/cu_up/cu_up_executor_mapper.h"
#include "srsran/du/du_high/du_high_executor_mapper.h"
#include "srsran/du/du_low/du_low_executor_mapper.h"
#include "srsran/ru/dummy/ru_dummy_executor_mapper.h"
#include "srsran/ru/generic/ru_generic_executor_mapper.h"
#include "srsran/ru/ofh/ru_ofh_executor_mapper.h"
#include "srsran/support/executors/task_execution_manager.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

namespace app_services {
class metrics_notifier;
}

/// Mapper of PCAP executors for the APP.
class gnb_pcap_executor_mapper : public du_pcap_executor_mapper,
                                 public cu_cp_pcap_executor_mapper,
                                 public cu_up_pcap_executor_mapper
{
public:
  virtual ~gnb_pcap_executor_mapper() = default;
};

/// Manages the workers of the app.
struct worker_manager {
  worker_manager(const worker_manager_config& config);
  ~worker_manager();

  void stop();

  std::vector<task_executor*> fapi_exec;
  task_executor*              split6_exec      = nullptr;
  task_executor*              split6_crtl_exec = nullptr;

  srs_du::du_high_executor_mapper& get_du_high_executor_mapper()
  {
    srsran_assert(du_high_exec_mapper, "DU High execution mapper is not available.");
    return *du_high_exec_mapper;
  }

  srs_cu_cp::cu_cp_executor_mapper& get_cu_cp_executor_mapper()
  {
    srsran_assert(cu_cp_exec_mapper, "CU-CP execution mapper is not available.");
    return *cu_cp_exec_mapper;
  }

  srs_cu_up::cu_up_executor_mapper& get_cu_up_executor_mapper()
  {
    srsran_assert(cu_up_exec_mapper, "CU-UP execution mapper is not available.");
    return *cu_up_exec_mapper;
  }

  srs_du::du_low_executor_mapper& get_du_low_executor_mapper()
  {
    srsran_assert(du_low_exec_mapper, "DU Low execution mapper is not available.");
    return *du_low_exec_mapper;
  }

  ru_dummy_executor_mapper& get_dummy_ru_executor_mapper()
  {
    srsran_assert(dummy_exec_mapper, "Dummy RU execution mapper is not available.");
    return *dummy_exec_mapper;
  }

  ru_generic_executor_mapper& get_sdr_ru_executor_mapper()
  {
    srsran_assert(sdr_exec_mapper, "SDR RU execution mapper is not available.");
    return *sdr_exec_mapper;
  }

  ru_ofh_executor_mapper& get_ofh_ru_executor_mapper()
  {
    srsran_assert(ofh_exec_mapper, "OFH RU execution mapper is not available.");
    return *ofh_exec_mapper;
  }

  du_pcap_executor_mapper&    get_du_pcap_executors() { return *pcap_exec_mapper; }
  cu_cp_pcap_executor_mapper& get_cu_cp_pcap_executors() { return *pcap_exec_mapper; }
  cu_up_pcap_executor_mapper& get_cu_up_pcap_executors() { return *pcap_exec_mapper; }

  task_executor& get_cmd_line_executor() const { return *non_rt_medium_prio_exec; }
  task_executor& get_timer_source_executor() const { return *timer_source_exec; }
  task_executor& get_metrics_executor() const { return *metrics_exec; }

private:
  srslog::basic_logger& app_logger;

  /// Total number of workers for the general task worker pool. Necessary for providing maximum concurrency level to
  /// the physical layer.
  unsigned nof_workers_general_pool = 0;

  /// Instantiated raw executors.
  task_executor* non_rt_low_prio_exec    = nullptr;
  task_executor* non_rt_medium_prio_exec = nullptr;
  task_executor* non_rt_hi_prio_exec     = nullptr;
  task_executor* rt_hi_prio_exec         = nullptr;

  std::unique_ptr<srs_du::du_high_executor_mapper>  du_high_exec_mapper;
  std::unique_ptr<srs_du::du_low_executor_mapper>   du_low_exec_mapper;
  std::unique_ptr<srs_cu_cp::cu_cp_executor_mapper> cu_cp_exec_mapper;
  std::unique_ptr<srs_cu_up::cu_up_executor_mapper> cu_up_exec_mapper;

  /// Serialized Executor used for metrics.
  task_executor* metrics_exec = nullptr;

  /// Serialized Executor used for timer ticking.
  task_executor* timer_source_exec = nullptr;

  std::unique_ptr<gnb_pcap_executor_mapper> pcap_exec_mapper;

  /// Dummy RU executor mapper.
  std::unique_ptr<ru_dummy_executor_mapper> dummy_exec_mapper;

  /// SDR RU executor mapper.
  std::unique_ptr<ru_generic_executor_mapper> sdr_exec_mapper;

  /// Open Fronthaul RU executor mapper.
  std::unique_ptr<ru_ofh_executor_mapper> ofh_exec_mapper;

  /// Manager of execution contexts and respective executors instantiated by the application.
  task_execution_manager exec_mng;

  /// Executor metrics channel registry.
  executor_metrics_channel_registry* exec_metrics_channel_registry = nullptr;

  /// Collection of task executor decorators.
  std::vector<std::unique_ptr<task_executor>> executor_decorators_exec;

  os_sched_affinity_manager              main_pool_affinity_mng;
  os_sched_affinity_bitmask              ru_timing_mask;
  std::vector<os_sched_affinity_bitmask> ru_txrx_affinity_masks;

  /// CPU affinity bitmask manager per cell.
  std::vector<os_sched_affinity_manager> affinity_mng;

  /// State of worker_manager.
  bool running = true;

  void create_main_worker_pool(const worker_manager_config& worker_cfg);

  /// Helper method to create workers with non zero priority.
  void create_prio_worker(const std::string&               name,
                          const std::string&               exec_name,
                          unsigned                         queue_size,
                          const os_sched_affinity_bitmask& mask,
                          os_thread_realtime_priority      prio = os_thread_realtime_priority::no_realtime());

  /// Helper method to create worker pool.
  void create_worker_pool(const std::string&                    name,
                          unsigned                              nof_workers,
                          const std::string&                    exec_name,
                          unsigned                              queue_size,
                          os_thread_realtime_priority           prio      = os_thread_realtime_priority::no_realtime(),
                          span<const os_sched_affinity_bitmask> cpu_masks = {},
                          concurrent_queue_policy               queue_policy = concurrent_queue_policy::locking_mpmc);

  void create_support_strands(const worker_manager_config& config);

  void add_pcap_strands(const worker_manager_config::pcap_config& config);

  std::vector<execution_config_helper::single_worker> create_fapi_workers(unsigned nof_cells);

  /// Helper method that creates the CU-CP executors.
  void create_cu_cp_executors(const worker_manager_config::cu_cp_config& config, timer_manager& timers);

  /// Helper method that creates the CU-UP executors.
  void create_cu_up_executors(const worker_manager_config::cu_up_config& config, timer_manager& timers);

  /// Helper method that creates the DU-high executors.
  void create_du_high_executors(const worker_manager_config::du_high_config& config);

  /// Helper method that creates the FAPI executors.
  void create_fapi_executors(const worker_manager_config::fapi_config& config);

  /// Helper method that creates the low Distributed Unit executors.
  void create_du_low_executors(const worker_manager_config::du_low_config& du_low);

  /// Helper method that creates the Radio Unit dummy executors.
  void create_ru_dummy_executors();

  /// Helper method that creates the lower PHY executors.
  void create_lower_phy_executors(const worker_manager_config::ru_sdr_config& config);

  /// Helper method that creates the Open Fronthaul executors.
  void create_ofh_executors(const worker_manager_config::ru_ofh_config& config);

  /// Helper method that creates the split 6 executors.
  void create_split6_executors();
};

} // namespace srsran
