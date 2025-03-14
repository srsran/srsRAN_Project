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

#include "worker_manager.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/du/du_high/du_high_executor_mapper.h"

using namespace srsran;

static const uint32_t task_worker_queue_size = 2048;

worker_manager::worker_manager(const worker_manager_config& worker_cfg) :
  low_prio_affinity_mng({worker_cfg.low_prio_sched_config})
{
  // Check configuration.
  {
    unsigned ru_config_count = 0;
    if (worker_cfg.ru_ofh_cfg) {
      ++ru_config_count;
    }
    if (worker_cfg.ru_sdr_cfg) {
      ++ru_config_count;
    }
    if (worker_cfg.ru_dummy_cfg) {
      ++ru_config_count;
    }

    srsran_assert(ru_config_count <= 1, "Worker manager received configuration for more than one RU type");
  }

  for (const auto& cell_affinities : worker_cfg.config_affinities) {
    affinity_mng.emplace_back(cell_affinities);
  }

  create_low_prio_executors(worker_cfg);
  associate_low_prio_executors(worker_cfg);

  if (worker_cfg.is_split6_enabled) {
    create_split6_executors();
  }

  if (worker_cfg.cu_up_cfg) {
    create_cu_up_executors(worker_cfg.cu_up_cfg.value(), *worker_cfg.app_timers);
  }

  if (worker_cfg.du_hi_cfg) {
    create_du_executors(
        worker_cfg.du_hi_cfg.value(), worker_cfg.du_low_cfg, worker_cfg.fapi_cfg, *worker_cfg.app_timers);
  }

  if (worker_cfg.ru_ofh_cfg) {
    ru_timing_mask         = worker_cfg.ru_ofh_cfg.value().ru_timing_cpu;
    ru_txrx_affinity_masks = worker_cfg.ru_ofh_cfg.value().txrx_affinities;
    create_ofh_executors(worker_cfg.ru_ofh_cfg.value());
  }
  if (worker_cfg.ru_sdr_cfg) {
    create_lower_phy_executors(worker_cfg.ru_sdr_cfg.value());
  }

  if (worker_cfg.ru_dummy_cfg) {
    create_ru_dummy_executors();
  }
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
                         const worker_manager_config::pcap_config&     config)
{
  using namespace execution_config_helper;

  // Default configuration for each pcap writer strand.
  strand base_strand_cfg{{{"pcap_exec", concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}}};

  // These layers have very low throughput, so no point in instantiating more than one strand.
  // This means that there is no parallelization in pcap writing across these layers.
  if (config.is_f1ap_enabled or config.is_ngap_enabled or config.is_e1ap_enabled or config.is_e2ap_enabled) {
    strand_list.emplace_back(base_strand_cfg);
  }

  if (config.is_n3_enabled) {
    base_strand_cfg.queues[0].name = "n3_pcap_exec";
    strand_list.emplace_back(base_strand_cfg);
  }

  if (config.is_f1u_enabled) {
    base_strand_cfg.queues[0].name = "f1u_pcap_exec";
    strand_list.emplace_back(base_strand_cfg);
  }

  if (config.is_mac_enabled) {
    base_strand_cfg.queues[0].name = "mac_pcap_exec";
    strand_list.emplace_back(base_strand_cfg);
  }

  if (config.is_rlc_enabled) {
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

void worker_manager::create_cu_up_executors(const worker_manager_config::cu_up_config& config, timer_manager& timers)
{
  using namespace execution_config_helper;
  const auto& exec_map = exec_mng.executors();

  cu_up_exec_mapper =
      srs_cu_up::make_cu_up_executor_mapper(srs_cu_up::strand_based_executor_config{config.max_nof_ue_strands,
                                                                                    task_worker_queue_size,
                                                                                    config.gtpu_queue_size,
                                                                                    *exec_map.at("low_prio_exec"),
                                                                                    config.dedicated_io_ul_strand,
                                                                                    &timers});
}

void worker_manager::create_du_executors(const worker_manager_config::du_high_config&        du_hi,
                                         std::optional<worker_manager_config::du_low_config> du_low,
                                         std::optional<worker_manager_config::fapi_config>   fapi_cfg,
                                         timer_manager&                                      timer)
{
  using namespace execution_config_helper;
  const auto& exec_map = exec_mng.executors();

  // FAPI message buffering executors.
  if (fapi_cfg) {
    fapi_exec.resize(fapi_cfg.value().nof_cells);
    std::fill(fapi_exec.begin(), fapi_exec.end(), nullptr);
    // Create workers.
    auto workers = create_fapi_workers(fapi_cfg.value().nof_cells);

    for (unsigned cell_id = 0; cell_id != fapi_cfg.value().nof_cells; ++cell_id) {
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
  auto slot_workers = create_du_hi_slot_workers(du_hi.nof_cells, du_hi.is_rt_mode_enabled);
  for (unsigned cell_id = 0; cell_id != du_hi.nof_cells; ++cell_id) {
    const std::string cell_id_str = std::to_string(cell_id);

    slot_workers[cell_id].executors.push_back(
        {"cell_exec#" + cell_id_str, task_priority::max - 1, {}, std::nullopt, !du_hi.is_rt_mode_enabled});
    slot_workers[cell_id].executors.push_back(
        {"slot_exec#" + cell_id_str, task_priority::max, {}, std::nullopt, !du_hi.is_rt_mode_enabled});

    if (not exec_mng.add_execution_context(create_execution_context(slot_workers[cell_id]))) {
      report_fatal_error("Failed to instantiate {} execution context", slot_workers[cell_id].name);
    }

    // Pre-initialize TLS to avoid doing it in the critical path.
    task_executor* exec_to_init = exec_mng.executors().at("slot_exec#" + cell_id_str);
    bool           result       = exec_to_init->defer([&timer, exec_to_init]() {
      // Pre-initialize byte buffer pool to avoid doing it in the critical path.
      init_byte_buffer_segment_pool_tls();
      // Pre-initialize timer queues to avoid doing it in the critical path.
      timer_factory timer_f{timer, *exec_to_init};
      auto          t = timer_f.create_timer();
      t.set(std::chrono::milliseconds(1), [](timer_id_t tid) {}); // run noop callback.
      t.run();
    });
    report_error_if_not(result, "Unexpected failure to dispatch cell task");
  }

  // Instantiate DU-high executor mapper.
  srs_du::du_high_executor_config cfg;
  // Create one executor mapper as one DU supports multiple cells.
  du_high_executors.resize(1);
  auto&                                                       du_item = du_high_executors[0];
  srs_du::du_high_executor_config::dedicated_cell_worker_list cell_workers;
  for (unsigned i = 0; i != du_hi.nof_cells; ++i) {
    const std::string cell_id_str = std::to_string(i);
    cell_workers.push_back({*exec_map.at("slot_exec#" + cell_id_str), *exec_map.at("cell_exec#" + cell_id_str)});
  }
  cfg.cell_executors.emplace<srs_du::du_high_executor_config::dedicated_cell_worker_list>(std::move(cell_workers));
  cfg.ue_executors.policy            = srs_du::du_high_executor_config::ue_executor_config::map_policy::per_cell;
  cfg.ue_executors.max_nof_strands   = 1;
  cfg.ue_executors.ctrl_queue_size   = task_worker_queue_size;
  cfg.ue_executors.pdu_queue_size    = du_hi.pdu_queue_size;
  cfg.ue_executors.pool_executor     = exec_map.at("low_prio_exec");
  cfg.ctrl_executors.task_queue_size = task_worker_queue_size;
  cfg.ctrl_executors.pool_executor   = exec_map.at("high_prio_exec");
  cfg.is_rt_mode_enabled             = du_hi.is_rt_mode_enabled;
  cfg.trace_exec_tasks               = false;

  du_item.du_high_exec_mapper = srs_du::create_du_high_executor_mapper(cfg);

  if (du_low) {
    create_du_low_executors(du_low.value().is_blocking_mode_active,
                            du_low.value().nof_ul_threads,
                            du_low.value().nof_dl_threads,
                            du_low.value().nof_pusch_decoder_threads,
                            du_low.value().nof_cells);
  }
}

execution_config_helper::worker_pool worker_manager::create_low_prio_workers(unsigned nof_low_prio_threads,
                                                                             unsigned low_prio_task_queue_size,
                                                                             os_sched_affinity_bitmask low_prio_mask)
{
  using namespace execution_config_helper;

  // Configure non-RT worker pool.
  worker_pool non_rt_pool{
      "non_rt_pool",
      nof_low_prio_threads,
      {{concurrent_queue_policy::lockfree_mpmc, low_prio_task_queue_size}, // two task priority levels.
       {concurrent_queue_policy::lockfree_mpmc, low_prio_task_queue_size}},
      // Left empty, is filled later.
      {},
      std::chrono::microseconds{100},
      os_thread_realtime_priority::no_realtime(),
      std::vector<os_sched_affinity_bitmask>{low_prio_mask}};

  return non_rt_pool;
}

void worker_manager::create_low_prio_executors(const worker_manager_config& worker_cfg)
{
  using namespace execution_config_helper;
  // TODO: split executor creation and association to workers
  worker_pool non_rt_pool = create_low_prio_workers(
      worker_cfg.nof_low_prio_threads, worker_cfg.low_prio_task_queue_size, worker_cfg.low_prio_sched_config.mask);

  // Associate executors to the worker pool.
  // Used for PCAP writing and CU-UP.
  non_rt_pool.executors.emplace_back("low_prio_exec", task_priority::max - 1);
  // Used for control plane and timer management.
  non_rt_pool.executors.emplace_back("high_prio_exec", task_priority::max);

  std::vector<strand>& low_prio_strands  = non_rt_pool.executors[0].strands;
  std::vector<strand>& high_prio_strands = non_rt_pool.executors[1].strands;

  // Configuration of strands for PCAP writing. These strands will use the low priority executor.
  append_pcap_strands(low_prio_strands, worker_cfg.pcap_cfg);

  // Configuration of strand for the CU-CP task handling.
  strand cu_cp_strand{{{"ctrl_exec", concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}}};
  high_prio_strands.push_back(cu_cp_strand);

  // Create non-RT worker pool.
  if (not exec_mng.add_execution_context(create_execution_context(non_rt_pool))) {
    report_fatal_error("Failed to instantiate {} execution context", non_rt_pool.name);
  }

  non_rt_low_prio_exec = exec_mng.executors().at("low_prio_exec");
  non_rt_hi_prio_exec  = exec_mng.executors().at("high_prio_exec");
}

void worker_manager::associate_low_prio_executors(const worker_manager_config& config)
{
  using namespace execution_config_helper;
  const auto& exec_map = exec_mng.executors();

  // Update executor pointer mapping
  cu_cp_exec       = exec_map.at("ctrl_exec");
  cu_e2_exec       = exec_map.at("ctrl_exec");
  metrics_hub_exec = exec_map.at("ctrl_exec");
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

void worker_manager::create_ofh_executors(const worker_manager_config::ru_ofh_config& config)
{
  using namespace execution_config_helper;
  const unsigned nof_cells = config.nof_downlink_antennas.size();

  // Timing executor.
  {
    const std::string name      = "ru_timing";
    const std::string exec_name = "ru_timing_exec";

    // Use the ru_timing_cpu configuration if specified, otherwise use the ru_cpus of the first cell, as the timer
    // worker is shared for all the RU cells.
    os_sched_affinity_bitmask ru_timing_cpu_mask =
        ru_timing_mask.any() ? ru_timing_mask
                             : affinity_mng.front().calcute_affinity_mask(sched_affinity_mask_types::ru);
    const single_worker ru_worker{name,
                                  {concurrent_queue_policy::locking_mpsc, 4},
                                  {{exec_name}},
                                  std::nullopt,
                                  os_thread_realtime_priority::max() - 0,
                                  ru_timing_cpu_mask};
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
          (config.is_downlink_parallelized) ? std::max(config.nof_downlink_antennas[i] / 2U, 1U) : 1U;
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
      const single_worker ru_worker{name,
                                    {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                                    {{exec_name}},
                                    std::chrono::microseconds{15},
                                    os_thread_realtime_priority::max() - 5,
                                    affinity_mng[i].calcute_affinity_mask(sched_affinity_mask_types::ru)};
      if (not exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_rx_exec.push_back(exec_mng.executors().at(exec_name));
    }
  }
  // Executor for Open Fronthaul messages transmission and reception.
  {
    for (unsigned i = 0, e = ru_txrx_affinity_masks.size(); i != e; ++i) {
      const std::string name      = "ru_txrx_#" + std::to_string(i);
      const std::string exec_name = "ru_txrx_exec_#" + std::to_string(i);

      const single_worker ru_worker{name,
                                    {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                                    {{exec_name}},
                                    std::chrono::microseconds{1},
                                    os_thread_realtime_priority::max() - 1,
                                    ru_txrx_affinity_masks[i]};
      if (not exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_txrx_exec.push_back(exec_mng.executors().at(exec_name));
    }
  }
}

void worker_manager::create_split6_executors()
{
  using namespace execution_config_helper;

  const std::string name      = "split6";
  const std::string exec_name = "split6_exec";

  const single_worker split6_worker{
      name,
      {concurrent_queue_policy::lockfree_spsc, task_worker_queue_size},
      {{exec_name}},
      std::chrono::microseconds{50},
      os_thread_realtime_priority::max() - 6,
      low_prio_affinity_mng.calcute_affinity_mask(sched_affinity_mask_types::low_priority)};
  if (!exec_mng.add_execution_context(create_execution_context(split6_worker))) {
    report_fatal_error("Failed to instantiate {} execution context", split6_worker.name);
  }

  split6_exec = exec_mng.executors().at(exec_name);
}

void worker_manager::create_lower_phy_executors(const worker_manager_config::ru_sdr_config& config)
{
  using namespace execution_config_helper;

  // Radio Unit worker and executor. As the radio is unique per application, use the first cell of the affinity manager.
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

  for (unsigned cell_id = 0; cell_id != config.nof_cells; ++cell_id) {
    switch (config.profile) {
      case worker_manager_config::ru_sdr_config::lower_phy_thread_profile::blocking: {
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
      case worker_manager_config::ru_sdr_config::lower_phy_thread_profile::single: {
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
      case worker_manager_config::ru_sdr_config::lower_phy_thread_profile::dual: {
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
      case worker_manager_config::ru_sdr_config::lower_phy_thread_profile::quad: {
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

void worker_manager::create_ru_dummy_executors()
{
  // Use the first cell of the affinity manager for the dummy RU.
  create_prio_worker("ru_dummy",
                     task_worker_queue_size,
                     {{"ru_dummy"}},
                     affinity_mng.front().calcute_affinity_mask(sched_affinity_mask_types::ru),
                     os_thread_realtime_priority::max());
  radio_exec = exec_mng.executors().at("ru_dummy");
}

srs_du::du_high_executor_mapper& worker_manager::get_du_high_executor_mapper(unsigned du_index)
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
