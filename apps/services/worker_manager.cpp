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

#include "worker_manager.h"
#include "../../lib/du_high/du_high_executor_strategies.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/support/event_tracing.h"

using namespace srsran;

static const uint32_t task_worker_queue_size = 2048;

static std::vector<os_sched_affinity_config>
build_affinity_manager_dependencies(const du_high_unit_cpu_affinities_cell_config&                du_high_affinities,
                                    const du_low_unit_cpu_affinities_cell_config&                 du_low_affinities,
                                    const std::variant<ru_sdr_unit_cpu_affinities_cell_config,
                                                       ru_ofh_unit_cpu_affinities_cell_config,
                                                       ru_dummy_cpu_affinities_cell_unit_config>& ru_affinities)
{
  std::vector<os_sched_affinity_config> out;
  out.push_back(du_low_affinities.l1_ul_cpu_cfg);
  out.push_back(du_low_affinities.l1_dl_cpu_cfg);
  out.push_back(du_high_affinities.l2_cell_cpu_cfg);

  if (std::holds_alternative<ru_sdr_unit_cpu_affinities_cell_config>(ru_affinities)) {
    out.push_back(std::get<ru_sdr_unit_cpu_affinities_cell_config>(ru_affinities).ru_cpu_cfg);
  }

  else if (std::holds_alternative<ru_ofh_unit_cpu_affinities_cell_config>(ru_affinities)) {
    out.push_back(std::get<ru_ofh_unit_cpu_affinities_cell_config>(ru_affinities).ru_cpu_cfg);
  } else {
    out.push_back(std::get<ru_dummy_cpu_affinities_cell_unit_config>(ru_affinities).ru_cpu_cfg);
  }

  return out;
}

worker_manager::worker_manager(const dynamic_du_unit_config&     du_cfg,
                               const expert_execution_appconfig& expert_appcfg,
                               cu_cp_unit_pcap_config&           cu_cp_pcap_cfg,
                               cu_up_unit_pcap_config&           cu_up_pcap_cfg,
                               unsigned                          gtpu_queue_size) :
  low_prio_affinity_mng({expert_appcfg.affinities.low_priority_cpu_cfg})
{
  if (std::holds_alternative<ru_ofh_unit_parsed_config>(du_cfg.ru_cfg)) {
    ru_timing_mask = std::get<ru_ofh_unit_parsed_config>(du_cfg.ru_cfg).config.expert_execution_cfg.ru_timing_cpu;
  }

  const unsigned nof_cells = du_cfg.du_high_cfg.config.expert_execution_cfg.cell_affinities.size();
  for (unsigned i = 0, e = nof_cells; i != e; ++i) {
    std::variant<ru_sdr_unit_cpu_affinities_cell_config,
                 ru_ofh_unit_cpu_affinities_cell_config,
                 ru_dummy_cpu_affinities_cell_unit_config>
        ru;
    if (std::holds_alternative<ru_sdr_unit_config>(du_cfg.ru_cfg)) {
      ru = std::get<ru_sdr_unit_config>(du_cfg.ru_cfg).expert_execution_cfg.cell_affinities[i];
    } else if (std::holds_alternative<ru_ofh_unit_parsed_config>(du_cfg.ru_cfg)) {
      ru = std::get<ru_ofh_unit_parsed_config>(du_cfg.ru_cfg).config.expert_execution_cfg.cell_affinities[i];
    } else {
      ru = std::get<ru_dummy_unit_config>(du_cfg.ru_cfg).cell_affinities[i];
    }

    affinity_mng.emplace_back(
        build_affinity_manager_dependencies(du_cfg.du_high_cfg.config.expert_execution_cfg.cell_affinities[i],
                                            du_cfg.du_low_cfg.expert_execution_cfg.cell_affinities[i],
                                            ru));
  }

  // Determine whether the gnb app is running in realtime or in simulated environment.
  bool is_blocking_mode_active = false;
  if (std::holds_alternative<ru_sdr_unit_config>(du_cfg.ru_cfg)) {
    const auto& sdr_cfg     = std::get<ru_sdr_unit_config>(du_cfg.ru_cfg);
    is_blocking_mode_active = sdr_cfg.device_driver == "zmq";
  }

  create_low_prio_executors(expert_appcfg,
                            cu_cp_pcap_cfg,
                            cu_up_pcap_cfg,
                            du_cfg.du_high_cfg.config.pcaps,
                            du_cfg.du_high_cfg.config.cells_cfg.size(),
                            gtpu_queue_size,
                            not is_blocking_mode_active);
  associate_low_prio_executors();

  create_du_executors(is_blocking_mode_active, nof_cells, du_cfg.du_low_cfg, du_cfg.fapi_cfg);
  create_ru_executors(du_cfg.ru_cfg, du_cfg.du_high_cfg.config);
}

void worker_manager::stop()
{
  exec_mng.stop();
}

void worker_manager::create_worker_pool(const std::string&                                    name,
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

void worker_manager::create_prio_worker(const std::string&                                    name,
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
                         const cu_cp_unit_pcap_config&                 cu_cp_pcap_cfg,
                         const cu_up_unit_pcap_config&                 cu_up_pcap_cfg,
                         const du_high_unit_pcap_config&               du_pcaps)
{
  using namespace execution_config_helper;

  // Default configuration for each pcap writer strand.
  strand base_strand_cfg{{{"pcap_exec", concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}}};

  // These layers have very low throughput, so no point in instantiating more than one strand.
  // This means that there is no parallelization in pcap writing across these layers.
  if (cu_cp_pcap_cfg.f1ap.enabled or cu_cp_pcap_cfg.ngap.enabled or cu_cp_pcap_cfg.e1ap.enabled or
      cu_up_pcap_cfg.e1ap.enabled or du_pcaps.f1ap.enabled or du_pcaps.e2ap.enabled) {
    strand_list.emplace_back(base_strand_cfg);
  }

  if (cu_up_pcap_cfg.n3.enabled) {
    base_strand_cfg.queues[0].name = "n3_pcap_exec";
    strand_list.emplace_back(base_strand_cfg);
  }

  if (cu_up_pcap_cfg.f1u.enabled or du_pcaps.f1u.enabled) {
    base_strand_cfg.queues[0].name = "f1u_pcap_exec";
    strand_list.emplace_back(base_strand_cfg);
  }

  if (du_pcaps.mac.enabled) {
    base_strand_cfg.queues[0].name = "mac_pcap_exec";
    strand_list.emplace_back(base_strand_cfg);
  }

  if (du_pcaps.rlc.enabled) {
    base_strand_cfg.queues[0].name = "rlc_pcap_exec";
    strand_list.emplace_back(base_strand_cfg);
  }
}

std::vector<execution_config_helper::single_worker> worker_manager::create_fapi_workers(unsigned nof_cells)
{
  using namespace execution_config_helper;
  std::vector<single_worker> workers;

  for (unsigned cell_id = 0; cell_id != nof_cells; ++cell_id) {
    const std::string name = "fapi#" + std::to_string(cell_id);

    single_worker buffered_worker{name,
                                  {concurrent_queue_policy::locking_mpsc, task_worker_queue_size},
                                  // Left empty, is filled later.
                                  {},
                                  std::chrono::microseconds{50},
                                  os_thread_realtime_priority::max() - 6,
                                  affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::l2_cell)};

    workers.push_back(buffered_worker);
  }

  return workers;
}

std::vector<execution_config_helper::priority_multiqueue_worker>
worker_manager::create_du_hi_slot_workers(unsigned nof_cells, bool rt_mode)
{
  using namespace execution_config_helper;
  std::vector<priority_multiqueue_worker> workers;

  for (unsigned cell_id = 0; cell_id != nof_cells; ++cell_id) {
    const std::string cell_id_str = std::to_string(cell_id);

    const priority_multiqueue_worker du_cell_worker{
        "du_cell#" + cell_id_str,
        {{concurrent_queue_policy::lockfree_spsc, 4}, {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}},
        std::chrono::microseconds{10},
        // Left empty, is filled later.
        {},
        rt_mode ? os_thread_realtime_priority::max() - 2 : os_thread_realtime_priority::no_realtime(),
        affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::l2_cell)};

    workers.push_back(du_cell_worker);
  }

  return workers;
}

void worker_manager::create_du_executors(bool                      is_blocking_mode_active,
                                         unsigned                  nof_cells,
                                         const du_low_unit_config& du_low,
                                         const fapi_unit_config&   fapi_cfg)
{
  using namespace execution_config_helper;
  const auto& exec_map = exec_mng.executors();

  // FAPI message buffering executors.
  fapi_exec.resize(nof_cells);
  std::fill(fapi_exec.begin(), fapi_exec.end(), nullptr);
  if (fapi_cfg.l2_nof_slots_ahead) {
    // Create workers.
    auto workers = create_fapi_workers(nof_cells);

    for (unsigned cell_id = 0; cell_id != nof_cells; ++cell_id) {
      const std::string exec_name = "fapi_exec#" + std::to_string(cell_id);
      workers[cell_id].executors.emplace_back(exec_name);
      // Create executor and associated workers.
      if (!exec_mng.add_execution_context(create_execution_context(workers[cell_id]))) {
        report_fatal_error("Failed to instantiate {} execution context", workers[cell_id].name);
      }

      // Associate executor.
      fapi_exec[cell_id] = exec_mng.executors().at(exec_name);
    }
  }

  // Workers for handling cell slot indications of different cells.
  auto slot_workers = create_du_hi_slot_workers(nof_cells, not is_blocking_mode_active);
  for (unsigned cell_id = 0; cell_id != nof_cells; ++cell_id) {
    const std::string cell_id_str = std::to_string(cell_id);

    slot_workers[cell_id].executors.push_back(
        {"cell_exec#" + cell_id_str, task_priority::max - 1, {}, std::nullopt, is_blocking_mode_active});
    slot_workers[cell_id].executors.push_back(
        {"slot_exec#" + cell_id_str, task_priority::max, {}, std::nullopt, is_blocking_mode_active});

    if (not exec_mng.add_execution_context(create_execution_context(slot_workers[cell_id]))) {
      report_fatal_error("Failed to instantiate {} execution context", slot_workers[cell_id].name);
    }
  }

  // Instantiate DU-high executor mapper.
  du_high_executors.resize(nof_cells);
  for (unsigned i = 0; i != nof_cells; ++i) {
    auto&             du_item     = du_high_executors[i];
    const std::string cell_id_str = std::to_string(i);

    // DU-high executor mapper.
    using exec_list       = std::initializer_list<task_executor*>;
    auto cell_exec_mapper = std::make_unique<cell_executor_mapper>(exec_list{exec_map.at("cell_exec#" + cell_id_str)},
                                                                   exec_list{exec_map.at("slot_exec#" + cell_id_str)});
    auto ue_exec_mapper =
        std::make_unique<pcell_ue_executor_mapper>(exec_list{exec_map.at(fmt::format("du_rb_prio_exec#{}", i))},
                                                   exec_list{exec_map.at(fmt::format("du_rb_ul_exec#{}", i))},
                                                   exec_list{exec_map.at(fmt::format("du_rb_dl_exec#{}", i))});
    du_item.du_high_exec_mapper = std::make_unique<du_high_executor_mapper_impl>(std::move(cell_exec_mapper),
                                                                                 std::move(ue_exec_mapper),
                                                                                 *exec_map.at("ctrl_exec"),
                                                                                 *exec_map.at("timer_exec"),
                                                                                 *exec_map.at("ctrl_exec"));
  }

  const du_low_unit_expert_threads_config& upper_phy_threads_cfg = du_low.expert_execution_cfg.threads;
  create_du_low_executors(is_blocking_mode_active,
                          upper_phy_threads_cfg.nof_ul_threads,
                          upper_phy_threads_cfg.nof_dl_threads,
                          upper_phy_threads_cfg.nof_pusch_decoder_threads,
                          nof_cells);
}

execution_config_helper::worker_pool
worker_manager::create_low_prio_workers(const expert_execution_appconfig& expert_appcfg)
{
  using namespace execution_config_helper;

  // Configure non-RT worker pool.
  worker_pool non_rt_pool{
      "non_rt_pool",
      expert_appcfg.threads.non_rt_threads.nof_non_rt_threads,
      {{concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}, // two task priority levels.
       {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}},
      // Left empty, is filled later.
      {},
      std::chrono::microseconds{100},
      os_thread_realtime_priority::no_realtime(),
      std::vector<os_sched_affinity_bitmask>{expert_appcfg.affinities.low_priority_cpu_cfg.mask}};

  return non_rt_pool;
}

void worker_manager::create_low_prio_executors(const expert_execution_appconfig& expert_appcfg,
                                               const cu_cp_unit_pcap_config&     cu_cp_pcaps,
                                               const cu_up_unit_pcap_config&     cu_up_pcaps,
                                               const du_high_unit_pcap_config&   du_pcaps,
                                               unsigned                          nof_cells,
                                               unsigned                          gtpu_queue_size,
                                               bool                              rt_mode)
{
  using namespace execution_config_helper;
  // TODO: split executor creation and association to workers
  worker_pool non_rt_pool = create_low_prio_workers(expert_appcfg);

  // Associate executors to the worker pool.
  // Used for PCAP writing.
  non_rt_pool.executors.emplace_back("low_prio_exec", task_priority::max - 1);
  // Used for control plane and timer management.
  non_rt_pool.executors.push_back({"high_prio_exec", task_priority::max});
  // Used to serialize all CU-UP tasks, while CU-UP does not support multithreading.
  non_rt_pool.executors.push_back({"cu_up_strand",
                                   task_priority::max - 1,
                                   {}, // define CU-UP strands below.
                                   task_worker_queue_size});

  std::vector<strand>& low_prio_strands  = non_rt_pool.executors[0].strands;
  std::vector<strand>& high_prio_strands = non_rt_pool.executors[1].strands;
  std::vector<strand>& cu_up_strands     = non_rt_pool.executors[2].strands;

  // Configuration of strands for PCAP writing. These strands will use the low priority executor.
  append_pcap_strands(low_prio_strands, cu_cp_pcaps, cu_up_pcaps, du_pcaps);

  // Configuration of strand for the control plane handling (CU-CP and DU-high control plane).
  // This strand will support two priority levels, the highest being for timer management.
  // Note: In case of non-RT operation, we make the timer_exec synchronous. This will have the effect of stopping
  // the lower layers from running faster than this strand.
  strand cp_strand{{{"timer_exec", concurrent_queue_policy::lockfree_spsc, task_worker_queue_size, not rt_mode},
                    {"ctrl_exec", concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}}};
  high_prio_strands.push_back(cp_strand);

  // Setup strands for the data plane of all the instantiated DUs.
  // One strand per DU, each with multiple priority levels.
  for (unsigned i = 0; i != nof_cells; ++i) {
    low_prio_strands.push_back(
        strand{{{fmt::format("du_rb_prio_exec#{}", i), concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                {fmt::format("du_rb_ul_exec#{}", i), concurrent_queue_policy::lockfree_mpmc, gtpu_queue_size},
                {fmt::format("du_rb_dl_exec#{}", i), concurrent_queue_policy::lockfree_mpmc, gtpu_queue_size}}});
  }

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

void worker_manager::associate_low_prio_executors()
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

void worker_manager::create_du_low_executors(bool     is_blocking_mode_active,
                                             unsigned nof_ul_workers,
                                             unsigned nof_dl_workers,
                                             unsigned nof_pusch_decoder_workers,
                                             unsigned nof_cells)
{
  using namespace execution_config_helper;

  du_low_dl_executors.resize(nof_cells);

  if (is_blocking_mode_active) {
    // Create a single worker, shared by the whole PHY. As it is shared for all the PHY, pick the first cell of the
    // affinity manager.
    create_prio_worker("phy_worker",
                       task_worker_queue_size,
                       {{"phy_exec"}},
                       affinity_mng.front().calcute_affinity_mask(sched_affinity_mask_types::l1_dl),
                       os_thread_realtime_priority::no_realtime());

    for (unsigned cell_id = 0, cell_end = nof_cells; cell_id != cell_end; ++cell_id) {
      upper_pusch_exec.push_back(exec_mng.executors().at("phy_exec"));
      upper_pucch_exec.push_back(exec_mng.executors().at("phy_exec"));
      upper_srs_exec.push_back(exec_mng.executors().at("phy_exec"));
      upper_prach_exec.push_back(exec_mng.executors().at("phy_exec"));
      upper_pdsch_exec.push_back(exec_mng.executors().at("phy_exec"));
      du_low_dl_executors[cell_id].emplace_back(exec_mng.executors().at("phy_exec"));
    }
  } else {
    // RF case.
    for (unsigned cell_id = 0, cell_end = nof_cells; cell_id != cell_end; ++cell_id) {
      const std::string cell_id_str = std::to_string(cell_id);
      const std::string name_ul     = "up_phy_ul#" + cell_id_str;
      const std::string name_dl     = "up_phy_dl#" + cell_id_str;
      const auto        prio        = os_thread_realtime_priority::max() - 15;

      std::vector<os_sched_affinity_bitmask> ul_cpu_masks;
      for (unsigned w = 0; w != nof_ul_workers; ++w) {
        ul_cpu_masks.push_back(affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::l1_ul));
      }

      std::vector<os_sched_affinity_bitmask> dl_cpu_masks;
      for (unsigned w = 0; w != nof_dl_workers; ++w) {
        dl_cpu_masks.push_back(affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::l1_dl));
      }

      // Instantiate PHY UL workers.
      create_worker_pool(
          name_ul,
          nof_ul_workers,
          task_worker_queue_size,
          {{"upper_pusch_exec#" + cell_id_str}, {"upper_pucch_exec#" + cell_id_str}, {"upper_srs_exec#" + cell_id_str}},
          prio,
          ul_cpu_masks);
      upper_pusch_exec.push_back(exec_mng.executors().at("upper_pusch_exec#" + cell_id_str));
      upper_pucch_exec.push_back(exec_mng.executors().at("upper_pucch_exec#" + cell_id_str));
      upper_srs_exec.push_back(exec_mng.executors().at("upper_srs_exec#" + cell_id_str));

      // Instantiate dedicated PRACH worker.
      const std::string name_prach = "phy_prach#" + cell_id_str;
      const std::string prach_exec = "prach_exec#" + cell_id_str;
      create_prio_worker(name_prach,
                         task_worker_queue_size,
                         {{prach_exec}},
                         affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::l1_ul),
                         os_thread_realtime_priority::max() - 2);
      upper_prach_exec.push_back(exec_mng.executors().at("prach_exec#" + cell_id_str));

      const std::string exec_name = "du_low_dl_exec#" + cell_id_str;

      // Instantiate dedicated PHY DL workers.
      create_worker_pool(name_dl,
                         nof_dl_workers,
                         task_worker_queue_size,
                         {{exec_name}},
                         os_thread_realtime_priority::max() - 10,
                         dl_cpu_masks);

      for (unsigned w = 0; w != nof_dl_workers; ++w) {
        du_low_dl_executors[cell_id].emplace_back(exec_mng.executors().at(exec_name));
      }
      upper_pdsch_exec.push_back(exec_mng.executors().at(exec_name));
    }
  }

  // Instantiate dedicated PUSCH decoder workers for each cell.
  for (unsigned cell_id = 0, cell_end = nof_cells; cell_id != cell_end; ++cell_id) {
    if (nof_pusch_decoder_workers > 0) {
      const std::string                      cell_id_str        = std::to_string(cell_id);
      const std::string                      name_pusch_decoder = "pusch#" + cell_id_str;
      const auto                             prio               = os_thread_realtime_priority::max() - 30;
      std::vector<os_sched_affinity_bitmask> cpu_masks;
      for (unsigned w = 0; w != nof_pusch_decoder_workers; ++w) {
        cpu_masks.push_back(low_prio_affinity_mng.calcute_affinity_mask(sched_affinity_mask_types::low_priority));
      }

      create_worker_pool(name_pusch_decoder,
                         nof_pusch_decoder_workers,
                         task_worker_queue_size,
                         {{name_pusch_decoder}},
                         prio,
                         cpu_masks);
      upper_pusch_decoder_exec.push_back(exec_mng.executors().at(name_pusch_decoder));
    } else {
      upper_pusch_decoder_exec.push_back(nullptr);
    }
  }
}

void worker_manager::create_ofh_executors(const ru_ofh_unit_expert_execution_config& ru_cfg,
                                          span<const unsigned>                       cell_dl_antennas)
{
  using namespace execution_config_helper;

  // Maximum number of threads per cell. Implementation defined. The 3 threads are: transmission, reception and
  // codification.
  static constexpr unsigned MAX_NUM_THREADS_PER_CELL = 3U;
  const unsigned            nof_cells                = ru_cfg.cell_affinities.size();

  unsigned nof_host_threads = std::max(4U, std::max(std::thread::hardware_concurrency(), 4U) - 3U);

  if (nof_host_threads < (nof_cells * MAX_NUM_THREADS_PER_CELL) + 1) {
    fmt::print(
        "Detected {} threads for Open Fronthaul, but a minimum of {} are needed to achieve a good performance.\n",
        nof_host_threads,
        (nof_cells * MAX_NUM_THREADS_PER_CELL) + 1);
  }

  // Timing executor.
  {
    const std::string name      = "ru_timing";
    const std::string exec_name = "ru_timing_exec";

    // As the timer worker is shared for all the RU cells, pick the first cell from the affinity manager.
    const single_worker ru_worker{name,
                                  {concurrent_queue_policy::lockfree_spsc, 4},
                                  {{exec_name}},
                                  std::chrono::microseconds{0},
                                  os_thread_realtime_priority::max() - 0,
                                  ru_timing_mask};
    if (!exec_mng.add_execution_context(create_execution_context(ru_worker))) {
      report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
    }
    ru_timing_exec = exec_mng.executors().at(exec_name);
  }

  for (unsigned i = 0, e = nof_cells; i != e; ++i) {
    // Executor for the Open Fronthaul User and Control messages codification.
    {
      ru_dl_exec.emplace_back();
      unsigned nof_ofh_dl_workers =
          (ru_cfg.threads.is_downlink_parallelized) ? std::max(cell_dl_antennas[i] / 2U, 1U) : 1U;
      const std::string name      = "ru_dl_#" + std::to_string(i);
      const std::string exec_name = "ru_dl_exec_#" + std::to_string(i);

      const auto                             prio = os_thread_realtime_priority::max() - 5;
      std::vector<os_sched_affinity_bitmask> cpu_masks;
      for (unsigned w = 0; w != nof_ofh_dl_workers; ++w) {
        cpu_masks.push_back(affinity_mng[i].calcute_affinity_mask(sched_affinity_mask_types::ru));
      }
      create_worker_pool(name, nof_ofh_dl_workers, task_worker_queue_size, {{exec_name}}, prio, cpu_masks);
      ru_dl_exec[i] = exec_mng.executors().at(exec_name);
    }
    // Executor for Open Fronthaul messages decoding.
    {
      const std::string name      = "ru_rx_" + std::to_string(i);
      const std::string exec_name = "ru_rx_exec_" + std::to_string(i);

      // The generic locking queue type is used here to avoid polling for new tasks and thus for saving CPU resources
      // with a price of higher latency (it is acceptable for UL tasks that have lower priority compared to DL).
      const single_worker ru_worker{
          name,
          {concurrent_queue_policy::locking_mpmc, task_worker_queue_size},
          {{exec_name}},
          std::nullopt,
          os_thread_realtime_priority::max() - 6,
          low_prio_affinity_mng.calcute_affinity_mask(sched_affinity_mask_types::low_priority)};
      if (not exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_rx_exec.push_back(exec_mng.executors().at(exec_name));
    }
  }
  // Executor for Open Fronthaul messages transmission and reception.
  {
    unsigned nof_txrx_workers = std::max(static_cast<unsigned>(std::ceil(nof_cells / 2.0f)), 1U);

    for (unsigned i = 0; i != nof_txrx_workers; ++i) {
      const std::string name      = "ru_txrx_#" + std::to_string(i);
      const std::string exec_name = "ru_txrx_exec_#" + std::to_string(i);

      const single_worker ru_worker{name,
                                    {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                                    {{exec_name}},
                                    std::chrono::microseconds{1},
                                    os_thread_realtime_priority::max() - 1,
                                    affinity_mng.front().calcute_affinity_mask(sched_affinity_mask_types::ru)};
      if (not exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_txrx_exec.push_back(exec_mng.executors().at(exec_name));
    }
  }
}

void worker_manager::create_lower_phy_executors(lower_phy_thread_profile lower_phy_profile, unsigned nof_cells)
{
  using namespace execution_config_helper;

  // Radio Unit worker and executor. As the radio is unique per gNB, use the first cell of the affinity manager.
  create_prio_worker("radio",
                     task_worker_queue_size,
                     {{"radio_exec"}},
                     affinity_mng.front().calcute_affinity_mask(sched_affinity_mask_types::ru));
  radio_exec = exec_mng.executors().at("radio_exec");

  // Radio Unit statistics worker and executor.
  create_prio_worker("ru_stats_worker",
                     1,
                     {{"ru_printer_exec"}},
                     low_prio_affinity_mng.calcute_affinity_mask(sched_affinity_mask_types::low_priority));
  ru_printer_exec = exec_mng.executors().at("ru_printer_exec");

  for (unsigned cell_id = 0; cell_id != nof_cells; ++cell_id) {
    switch (lower_phy_profile) {
      case lower_phy_thread_profile::blocking: {
        fmt::print("Lower PHY in executor blocking mode.\n");
        std::string name      = "phy_worker";
        std::string exec_name = "phy_exec";

        task_executor* phy_exec = exec_mng.executors().at(exec_name);
        lower_prach_exec.push_back(phy_exec);
        lower_phy_tx_exec.push_back(phy_exec);
        lower_phy_rx_exec.push_back(phy_exec);
        lower_phy_dl_exec.push_back(phy_exec);
        lower_phy_ul_exec.push_back(phy_exec);
        break;
      }
      case lower_phy_thread_profile::single: {
        fmt::print("Lower PHY in single executor mode.\n");
        const std::string name      = "lower_phy#" + std::to_string(cell_id);
        const std::string exec_name = "lower_phy_exec#" + std::to_string(cell_id);

        create_prio_worker(name,
                           128,
                           {{exec_name}},
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max());

        task_executor* phy_exec = exec_mng.executors().at(exec_name);
        lower_phy_tx_exec.push_back(phy_exec);
        lower_phy_rx_exec.push_back(phy_exec);
        lower_phy_dl_exec.push_back(phy_exec);
        lower_phy_ul_exec.push_back(phy_exec);

        lower_prach_exec.push_back(exec_mng.executors().at("prach_exec#" + std::to_string(cell_id)));
        break;
      }
      case lower_phy_thread_profile::dual: {
        fmt::print("Lower PHY in dual executor mode.\n");
        const std::string name_dl = "lower_phy_dl#" + std::to_string(cell_id);
        const std::string exec_dl = "lower_phy_dl_exec#" + std::to_string(cell_id);
        const std::string name_ul = "lower_phy_ul#" + std::to_string(cell_id);
        const std::string exec_ul = "lower_phy_ul_exec#" + std::to_string(cell_id);

        create_prio_worker(name_dl,
                           128,
                           {{exec_dl}},
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max());
        create_prio_worker(name_ul,
                           2,
                           {{exec_ul}},
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max() - 1);

        lower_phy_tx_exec.push_back(exec_mng.executors().at(exec_dl));
        lower_phy_rx_exec.push_back(exec_mng.executors().at(exec_ul));
        lower_phy_dl_exec.push_back(exec_mng.executors().at(exec_dl));
        lower_phy_ul_exec.push_back(exec_mng.executors().at(exec_ul));

        lower_prach_exec.push_back(exec_mng.executors().at("prach_exec#" + std::to_string(cell_id)));
        break;
      }
      case lower_phy_thread_profile::quad: {
        fmt::print("Lower PHY in quad executor mode.\n");
        const std::string name_dl = "lower_phy_dl#" + std::to_string(cell_id);
        const std::string exec_dl = "lower_phy_dl_exec#" + std::to_string(cell_id);
        const std::string name_ul = "lower_phy_ul#" + std::to_string(cell_id);
        const std::string exec_ul = "lower_phy_ul_exec#" + std::to_string(cell_id);
        const std::string name_tx = "lower_phy_tx#" + std::to_string(cell_id);
        const std::string exec_tx = "lower_phy_tx_exec#" + std::to_string(cell_id);
        const std::string name_rx = "lower_phy_rx#" + std::to_string(cell_id);
        const std::string exec_rx = "lower_phy_rx_exec#" + std::to_string(cell_id);

        create_prio_worker(name_tx,
                           128,
                           {{exec_tx}},
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max());
        create_prio_worker(name_rx,
                           1,
                           {{exec_rx}},
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max() - 2);
        create_prio_worker(name_dl,
                           128,
                           {{exec_dl}},
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max() - 1);
        create_prio_worker(name_ul,
                           128,
                           {{exec_ul}},
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max() - 3);

        lower_phy_tx_exec.push_back(exec_mng.executors().at(exec_tx));
        lower_phy_rx_exec.push_back(exec_mng.executors().at(exec_rx));
        lower_phy_dl_exec.push_back(exec_mng.executors().at(exec_dl));
        lower_phy_ul_exec.push_back(exec_mng.executors().at(exec_ul));

        lower_prach_exec.push_back(exec_mng.executors().at("prach_exec#" + std::to_string(cell_id)));
        break;
      }
    }
  }
}

void worker_manager::create_ru_executors(
    const std::variant<ru_sdr_unit_config, ru_ofh_unit_parsed_config, ru_dummy_unit_config>& ru_cfg,
    const du_high_unit_config&                                                               du_high)
{
  if (std::holds_alternative<ru_ofh_unit_parsed_config>(ru_cfg)) {
    std::vector<unsigned> cell_antennas_dl;
    for (const auto& cell : du_high.cells_cfg) {
      cell_antennas_dl.push_back(cell.cell.nof_antennas_dl);
    }

    create_ofh_executors(std::get<ru_ofh_unit_parsed_config>(ru_cfg).config.expert_execution_cfg, cell_antennas_dl);

    return;
  }

  if (std::holds_alternative<ru_sdr_unit_config>(ru_cfg)) {
    const ru_sdr_unit_config& sdr_cfg = std::get<ru_sdr_unit_config>(ru_cfg);
    std::string               driver  = sdr_cfg.device_driver;

    create_lower_phy_executors((driver != "zmq") ? sdr_cfg.expert_execution_cfg.threads.execution_profile
                                                 : lower_phy_thread_profile::blocking,
                               sdr_cfg.expert_execution_cfg.cell_affinities.size());
    return;
  }

  // Use the first cell of the affinity manager for the dummy RU.
  create_prio_worker("ru_dummy",
                     task_worker_queue_size,
                     {{"ru_dummy"}},
                     affinity_mng.front().calcute_affinity_mask(sched_affinity_mask_types::ru),
                     os_thread_realtime_priority::max());
  radio_exec = exec_mng.executors().at("ru_dummy");
}

du_high_executor_mapper& worker_manager::get_du_high_executor_mapper(unsigned du_index)
{
  srsran_assert(du_index < du_high_executors.size(), "Invalid DU index");
  return *du_high_executors[du_index].du_high_exec_mapper;
}

void worker_manager::get_du_low_dl_executors(std::vector<task_executor*>& executors, unsigned sector_id) const
{
  srsran_assert(sector_id < du_low_dl_executors.size(), "Invalid sector configuration");
  const auto& du_low_exec = du_low_dl_executors[sector_id];

  executors.resize(du_low_exec.size());
  for (unsigned i_exec = 0, nof_execs = du_low_exec.size(); i_exec != nof_execs; ++i_exec) {
    executors[i_exec] = du_low_exec[i_exec];
  }
}
