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

#include "cu_worker_manager.h"
#include "srsran/support/event_tracing.h"

using namespace srsran;

static const uint32_t task_worker_queue_size = 2048;

cu_worker_manager::cu_worker_manager(const cu_appconfig&    appcfg,
                                     cu_cp_unit_pcap_config cu_cp_pcap_cfg,
                                     cu_up_unit_pcap_config cu_up_pcap_cfg,
                                     unsigned               gtpu_queue_size) :
  low_prio_affinity_mng({appcfg.expert_execution_cfg.affinities.low_priority_cpu_cfg})
{
  create_low_prio_executors(appcfg, cu_cp_pcap_cfg, cu_up_pcap_cfg, gtpu_queue_size);
  associate_low_prio_executors();
}

void cu_worker_manager::stop()
{
  exec_mng.stop();
}

void cu_worker_manager::create_worker_pool(const std::string&                                    name,
                                           unsigned                                              nof_workers,
                                           unsigned                                              queue_size,
                                           const std::vector<execution_config_helper::executor>& execs,
                                           os_thread_realtime_priority                           prio,
                                           span<const os_sched_affinity_bitmask>                 cpu_masks)
{
  using namespace execution_config_helper;

  concurrent_queue_policy queue_policy = concurrent_queue_policy::locking_mpmc;

  const worker_pool pool{name,
                         nof_workers,
                         {{queue_policy, queue_size}},
                         execs,
                         std::chrono::microseconds{queue_policy == concurrent_queue_policy::locking_mpmc ? 0 : 10},
                         prio,
                         std::vector<os_sched_affinity_bitmask>{cpu_masks.begin(), cpu_masks.end()}};
  if (not exec_mng.add_execution_context(create_execution_context(pool))) {
    report_fatal_error("Failed to instantiate {} execution context", pool.name);
  }
}

void cu_worker_manager::create_prio_worker(const std::string&                                    name,
                                           unsigned                                              queue_size,
                                           const std::vector<execution_config_helper::executor>& execs,
                                           const os_sched_affinity_bitmask&                      mask,
                                           os_thread_realtime_priority                           prio)
{
  using namespace execution_config_helper;

  const single_worker worker_desc{
      name, {concurrent_queue_policy::locking_mpsc, queue_size}, execs, std::nullopt, prio, mask};
  if (not exec_mng.add_execution_context(create_execution_context(worker_desc))) {
    report_fatal_error("Failed to instantiate {} execution context", worker_desc.name);
  }
}

void append_pcap_strands(std::vector<execution_config_helper::strand>& strand_list,
                         const cu_cp_unit_pcap_config&                 cp_pcaps,
                         const cu_up_unit_pcap_config&                 up_pcaps)
{
  using namespace execution_config_helper;

  // Default configuration for each pcap writer strand.
  strand base_strand_cfg{{{"pcap_exec", concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}}};

  // These layers have very low throughput, so no point in instantiating more than one strand.
  // This means that there is no parallelization in pcap writing across these layers.
  if (cp_pcaps.f1ap.enabled or cp_pcaps.ngap.enabled or cp_pcaps.e1ap.enabled) {
    strand_list.emplace_back(base_strand_cfg);
  }

  if (up_pcaps.n3.enabled) {
    base_strand_cfg.queues[0].name = "n3_pcap_exec";
    strand_list.emplace_back(base_strand_cfg);
  }

  if (up_pcaps.f1u.enabled) {
    base_strand_cfg.queues[0].name = "f1u_pcap_exec";
    strand_list.emplace_back(base_strand_cfg);
  }
}

execution_config_helper::worker_pool cu_worker_manager::create_low_prio_workers(const cu_appconfig& appcfg)
{
  using namespace execution_config_helper;

  // Configure non-RT worker pool.
  worker_pool non_rt_pool{
      "non_rt_pool",
      appcfg.expert_execution_cfg.threads.non_rt_threads.nof_non_rt_threads,
      {{concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}, // two task priority levels.
       {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}},
      // Left empty, is filled later.
      {},
      std::chrono::microseconds{100},
      os_thread_realtime_priority::no_realtime(),
      std::vector<os_sched_affinity_bitmask>{appcfg.expert_execution_cfg.affinities.low_priority_cpu_cfg.mask}};

  return non_rt_pool;
}

void cu_worker_manager::create_low_prio_executors(const cu_appconfig&           appcfg,
                                                  const cu_cp_unit_pcap_config& cu_cp_pcap_cfg,
                                                  const cu_up_unit_pcap_config& cu_up_pcap_cfg,
                                                  unsigned                      gtpu_queue_size)
{
  using namespace execution_config_helper;
  // TODO: split executor creation and association to workers
  worker_pool non_rt_pool = create_low_prio_workers(appcfg);

  // Associate executors to the worker pool.
  // Used for PCAP writing.
  non_rt_pool.executors.emplace_back("low_prio_exec", task_priority::max - 1);
  // Used for control plane and timer management.
  non_rt_pool.executors.emplace_back("high_prio_exec", task_priority::max);
  // Used to serialize all CU-UP tasks, while CU-UP does not support multithreading.
  non_rt_pool.executors.push_back({"cu_up_strand",
                                   task_priority::max - 1,
                                   {}, // define CU-UP strands below.
                                   task_worker_queue_size});

  std::vector<strand>& low_prio_strands  = non_rt_pool.executors[0].strands;
  std::vector<strand>& high_prio_strands = non_rt_pool.executors[1].strands;
  std::vector<strand>& cu_up_strands     = non_rt_pool.executors[2].strands;

  // Configuration of strands for PCAP writing. These strands will use the low priority executor.
  append_pcap_strands(low_prio_strands, cu_cp_pcap_cfg, cu_up_pcap_cfg);

  // Configuration of strand for the control plane handling (CU-CP and DU-high control plane). This strand will
  // support two priority levels, the highest being for timer management.
  strand cp_strand{{{"timer_exec", concurrent_queue_policy::lockfree_spsc, task_worker_queue_size},
                    {"ctrl_exec", concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}}};
  high_prio_strands.push_back(cp_strand);

  // Configuration of strands for user plane handling (CU-UP and DU-low user plane). Given that the CU-UP doesn't
  // currently support multithreading, these strands will point to a strand that interfaces with the non-RT thread pool.
  // Each UE strand will have three queues, one for timer management and configuration, one for DL data plane and one
  // for UL data plane.
  cu_up_strands.push_back(
      strand{{{"cu_up_ctrl_exec", concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
              {"cu_up_io_ul_exec", concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}}});
  for (unsigned i = 0; i != nof_cu_up_ue_strands; ++i) {
    cu_up_strands.push_back(
        strand{{{fmt::format("ue_up_ctrl_exec#{}", i), concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                {fmt::format("ue_up_ul_exec#{}", i),
                 concurrent_queue_policy::lockfree_mpmc,
                 gtpu_queue_size}, // TODO: Consider separate param for size of UL queue if needed.
                {fmt::format("ue_up_dl_exec#{}", i), concurrent_queue_policy::lockfree_mpmc, gtpu_queue_size}}});
  }

  // Create non-RT worker pool.
  if (not exec_mng.add_execution_context(create_execution_context(non_rt_pool))) {
    report_fatal_error("Failed to instantiate {} execution context", non_rt_pool.name);
  }
}

void cu_worker_manager::associate_low_prio_executors()
{
  using namespace execution_config_helper;
  const auto& exec_map = exec_mng.executors();

  // Update executor pointer mapping
  cu_cp_exec       = exec_map.at("ctrl_exec");
  cu_cp_e2_exec    = exec_map.at("ctrl_exec");
  metrics_hub_exec = exec_map.at("ctrl_exec");
  cu_up_ctrl_exec  = exec_map.at("cu_up_ctrl_exec");
  cu_up_io_ul_exec = exec_map.at("cu_up_io_ul_exec");
  cu_up_e2_exec    = exec_map.at("cu_up_ctrl_exec");

  // Create CU-UP execution mapper object.
  std::vector<task_executor*> ue_up_dl_execs(nof_cu_up_ue_strands, nullptr);
  std::vector<task_executor*> ue_up_ul_execs(nof_cu_up_ue_strands, nullptr);
  std::vector<task_executor*> ue_up_ctrl_execs(nof_cu_up_ue_strands, nullptr);
  for (unsigned i = 0; i != nof_cu_up_ue_strands; ++i) {
    ue_up_dl_execs[i]   = exec_map.at(fmt::format("ue_up_dl_exec#{}", i));
    ue_up_ul_execs[i]   = exec_map.at(fmt::format("ue_up_ul_exec#{}", i));
    ue_up_ctrl_execs[i] = exec_map.at(fmt::format("ue_up_ctrl_exec#{}", i));
  }
  cu_up_exec_mapper = srs_cu_up::make_cu_up_executor_pool(
      *exec_map.at("cu_up_ctrl_exec"), ue_up_dl_execs, ue_up_ul_execs, ue_up_ctrl_execs, *exec_map.at("low_prio_exec"));
}
