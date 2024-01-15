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

#include "gnb_worker_manager.h"
#include "lib/du_high/du_high_executor_strategies.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/support/event_tracing.h"

using namespace srsran;

static const uint32_t task_worker_queue_size = 2048;

static gnb_os_sched_affinity_config get_mask_from_config(const cpu_affinities_appconfig& config,
                                                         gnb_sched_affinity_mask_types   mask_type)
{
  switch (mask_type) {
    case gnb_sched_affinity_mask_types::l1_dl:
      return config.l1_dl_cpu_cfg;
    case gnb_sched_affinity_mask_types::l1_ul:
      return config.l1_ul_cpu_cfg;
    case gnb_sched_affinity_mask_types::l2_cell:
      return config.l2_cell_cpu_cfg;
    case gnb_sched_affinity_mask_types::ru:
      return config.ru_cpu_cfg;
    case gnb_sched_affinity_mask_types::low_priority:
      return config.low_priority_cpu_cfg;
    default:
      srsran_assert(0, "Invalid affinity mask type '{}'", to_unsigned(mask_type));
  }
  return {};
}

static std::vector<gnb_os_sched_affinity_config> build_affinity_manager_dependencies(const gnb_appconfig& config)
{
  std::vector<gnb_os_sched_affinity_config> out;

  const cpu_affinities_appconfig& affinities_cfg = config.expert_execution_cfg.affinities;

  for (unsigned i = 0, e = to_unsigned(srsran::gnb_sched_affinity_mask_types::last); i != e; ++i) {
    out.push_back(get_mask_from_config(affinities_cfg, to_affinity_mask_type(i)));
  }

  return out;
}

worker_manager::worker_manager(const gnb_appconfig& appcfg) : affinity_mng(build_affinity_manager_dependencies(appcfg))
{
  create_du_cu_executors(appcfg);

  create_ru_executors(appcfg);
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
      name, {concurrent_queue_policy::locking_mpsc, queue_size}, execs, nullopt, prio, mask};
  if (not exec_mng.add_execution_context(create_execution_context(worker_desc))) {
    report_fatal_error("Failed to instantiate {} execution context", worker_desc.name);
  }
}

void worker_manager::create_du_cu_executors(const gnb_appconfig& appcfg)
{
  using namespace execution_config_helper;
  const auto& exec_map = exec_mng.executors();

  // Determine whether the gnb app is running in realtime or in simulated environment.
  bool is_blocking_mode_active = false;
  if (variant_holds_alternative<ru_sdr_appconfig>(appcfg.ru_cfg)) {
    const ru_sdr_appconfig& sdr_cfg = variant_get<ru_sdr_appconfig>(appcfg.ru_cfg);
    is_blocking_mode_active         = sdr_cfg.device_driver == "zmq";
  }
  span<const cell_appconfig> cells_cfg = appcfg.cells_cfg;
  const unsigned             nof_cells = cells_cfg.size();

  // Configure non-RT worker pool.
  worker_pool non_rt_pool{
      "non_rt_pool",
      std::max(3U, (unsigned)appcfg.expert_execution_cfg.affinities.low_priority_cpu_cfg.mask.count()),
      {{concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}, // two task priority levels.
       {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}},
      {{"low_prio_exec", task_priority::max - 1}, // used for pcap writing.
       {"high_prio_exec", task_priority::max},    // used for control plane and timer management.
       {"cu_up_strand", // used to serialize all CU-UP tasks, while CU-UP does not support multithreading.
        task_priority::max,
        {}, // define strands below.
        task_worker_queue_size}},
      std::chrono::microseconds{100},
      os_thread_realtime_priority::no_realtime(),
      std::vector<os_sched_affinity_bitmask>{appcfg.expert_execution_cfg.affinities.low_priority_cpu_cfg.mask}};
  std::vector<strand>& low_prio_strands  = non_rt_pool.executors[0].strands;
  std::vector<strand>& high_prio_strands = non_rt_pool.executors[1].strands;
  std::vector<strand>& cu_up_strands     = non_rt_pool.executors[2].strands;

  // Configuration of strands for PCAP writing. These strands will use the low priority executor.
  // The low priority executor will be used for PCAP writing via dedicated strands.
  strand strand_cfg{{{"pcap_exec", concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}}};
  low_prio_strands.emplace_back(strand_cfg);
  if (appcfg.pcap_cfg.gtpu.enabled) {
    strand_cfg.queues[0].name = "gtpu_pcap_exec";
    low_prio_strands.emplace_back(strand_cfg);
  }
  if (appcfg.pcap_cfg.mac.enabled) {
    strand_cfg.queues[0].name = "mac_pcap_exec";
    low_prio_strands.emplace_back(strand_cfg);
  }
  if (appcfg.pcap_cfg.rlc.enabled) {
    strand_cfg.queues[0].name = "rlc_pcap_exec";
    low_prio_strands.emplace_back(strand_cfg);
  }

  // Configuration of strand for the control plane handling (CU-CP and DU-high control plane). This strand will
  // support two priority levels, the highest being for timer management.
  strand cp_strand{{{"timer_exec", concurrent_queue_policy::lockfree_spsc, task_worker_queue_size},
                    {"ctrl_exec", concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}}};
  high_prio_strands.push_back(cp_strand);

  // Configuration of strands for user plane handling (CU-UP and DU-low user plane). Given that the CU-UP doesn't
  // currently support multithreading, these strands will point to a strand that interfaces with the non-RT thread pool.
  // Each UE strand will have three queues, one for timer management and configuration, one for DL data plane and one
  // for UL data plane.
  const unsigned nof_up_strands = 1; // TODO: Fix, once CU-UP supports new gnb-cu exec mapper.
  for (unsigned i = 0; i != nof_up_strands; ++i) {
    cu_up_strands.push_back(
        strand{{{fmt::format("ue_up_ctrl_exec#{}", i), concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                {fmt::format("ue_up_ul_exec#{}", i),
                 concurrent_queue_policy::lockfree_mpmc,
                 appcfg.cu_up_cfg.gtpu_queue_size}, // TODO: Consider separate param for size of UL queue if needed.
                {fmt::format("ue_up_dl_exec#{}", i),
                 concurrent_queue_policy::lockfree_spsc,
                 appcfg.cu_up_cfg.gtpu_queue_size}}});
  }

  // Create non-RT worker pool.
  if (not exec_mng.add_execution_context(create_execution_context(non_rt_pool))) {
    report_fatal_error("Failed to instantiate {} execution context", non_rt_pool.name);
  }

  // Workers for handling cell slot indications of different cells.
  for (unsigned cell_id = 0; cell_id != cells_cfg.size(); ++cell_id) {
    const std::string                cell_id_str = std::to_string(cell_id);
    const priority_multiqueue_worker du_cell_worker{
        "du_cell#" + cell_id_str,
        {{concurrent_queue_policy::lockfree_spsc, 4},
         {concurrent_queue_policy::lockfree_spsc, 8},
         {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}},
        std::chrono::microseconds{10},
        // Create Cell and slot indication executors. In case of ZMQ, we make the slot indication executor
        // synchronous.
        {{"cell_exec#" + cell_id_str, task_priority::max - 2},
         {"err_ind#" + cell_id_str, task_priority ::max - 1},
         {"slot_exec#" + cell_id_str, task_priority::max, {}, nullopt, is_blocking_mode_active}},
        os_thread_realtime_priority::max() - 2,
        affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::l2_cell)};

    if (not exec_mng.add_execution_context(create_execution_context(du_cell_worker))) {
      report_fatal_error("Failed to instantiate {} execution context", du_cell_worker.name);
    }
  }

  // Update executor pointer mapping
  cu_cp_exec       = exec_map.at("ctrl_exec");
  cu_cp_e2_exec    = exec_map.at("ctrl_exec");
  metrics_hub_exec = exec_map.at("ctrl_exec");
  cu_up_ctrl_exec  = exec_map.at("ue_up_ctrl_exec#0");
  cu_up_ul_exec    = exec_map.at("ue_up_ul_exec#0");
  cu_up_dl_exec    = exec_map.at("ue_up_dl_exec#0");
  cu_up_e2_exec    = exec_map.at("ue_up_ctrl_exec#0");

  // Create CU-UP execution mapper object.
  std::vector<task_executor*> ue_up_dl_execs(nof_up_strands, nullptr);
  std::vector<task_executor*> ue_up_ul_execs   = {nof_up_strands, nullptr};
  std::vector<task_executor*> ue_up_ctrl_execs = {nof_up_strands, nullptr};
  for (unsigned i = 0; i != nof_up_strands; ++i) {
    ue_up_dl_execs[i]   = exec_map.at(fmt::format("ue_up_dl_exec#{}", i));
    ue_up_ul_execs[i]   = exec_map.at(fmt::format("ue_up_ul_exec#{}", i));
    ue_up_ctrl_execs[i] = exec_map.at(fmt::format("ue_up_ctrl_exec#{}", i));
  }
  cu_up_exec_mapper = make_cu_up_executor_mapper(ue_up_dl_execs, ue_up_ul_execs, ue_up_ctrl_execs);

  // Instantiate DU-high executor mapper.
  du_high_executors.resize(nof_cells);
  for (unsigned i = 0; i != nof_cells; ++i) {
    auto&             du_item     = du_high_executors[i];
    const std::string cell_id_str = std::to_string(i);

    // DU-high executor mapper.
    using exec_list       = std::initializer_list<task_executor*>;
    auto cell_exec_mapper = std::make_unique<cell_executor_mapper>(exec_list{exec_map.at("cell_exec#" + cell_id_str)},
                                                                   exec_list{exec_map.at("slot_exec#" + cell_id_str)},
                                                                   exec_list{exec_map.at("err_ind#" + cell_id_str)});
    auto ue_exec_mapper   = std::make_unique<pcell_ue_executor_mapper>(exec_list{exec_map.at("ue_up_ctrl_exec#0")},
                                                                     exec_list{exec_map.at("ue_up_ul_exec#0")},
                                                                     exec_list{exec_map.at("ue_up_dl_exec#0")});
    du_item.du_high_exec_mapper = std::make_unique<du_high_executor_mapper_impl>(std::move(cell_exec_mapper),
                                                                                 std::move(ue_exec_mapper),
                                                                                 *exec_map.at("ctrl_exec"),
                                                                                 *exec_map.at("timer_exec"),
                                                                                 *exec_map.at("ctrl_exec"));
  }

  const upper_phy_threads_appconfig& upper_phy_threads_cfg = appcfg.expert_execution_cfg.threads.upper_threads;
  create_du_low_executors(is_blocking_mode_active,
                          upper_phy_threads_cfg.nof_ul_threads,
                          upper_phy_threads_cfg.nof_dl_threads,
                          upper_phy_threads_cfg.nof_pusch_decoder_threads,
                          cells_cfg,
                          appcfg.expert_phy_cfg.max_processing_delay_slots);
}

void worker_manager::create_du_low_executors(bool                       is_blocking_mode_active,
                                             unsigned                   nof_ul_workers,
                                             unsigned                   nof_dl_workers,
                                             unsigned                   nof_pusch_decoder_workers,
                                             span<const cell_appconfig> cells_cfg,
                                             unsigned                   pipeline_depth)
{
  using namespace execution_config_helper;

  du_low_dl_executors.resize(cells_cfg.size());

  if (is_blocking_mode_active) {
    // Create a single worker, shared by the whole PHY.
    create_prio_worker("phy_worker",
                       task_worker_queue_size,
                       {{"phy_exec"}},
                       affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::l1_dl),
                       os_thread_realtime_priority::max());

    for (unsigned cell_id = 0, cell_end = cells_cfg.size(); cell_id != cell_end; ++cell_id) {
      upper_pusch_exec.push_back(exec_mng.executors().at("phy_exec"));
      upper_pucch_exec.push_back(exec_mng.executors().at("phy_exec"));
      upper_prach_exec.push_back(exec_mng.executors().at("phy_exec"));
      upper_pdsch_exec.push_back(exec_mng.executors().at("phy_exec"));
      du_low_dl_executors[cell_id].emplace_back(exec_mng.executors().at("phy_exec"));
    }

  } else {
    // RF case.
    for (unsigned cell_id = 0, cell_end = cells_cfg.size(); cell_id != cell_end; ++cell_id) {
      const std::string cell_id_str = std::to_string(cell_id);
      const std::string name_ul     = "up_phy_ul#" + cell_id_str;
      const std::string name_dl     = "up_phy_dl#" + cell_id_str;
      const auto        prio        = os_thread_realtime_priority::max() - 15;

      std::vector<os_sched_affinity_bitmask> ul_cpu_masks;
      for (unsigned w = 0; w != nof_ul_workers; ++w) {
        ul_cpu_masks.push_back(affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::l1_ul));
      }

      std::vector<os_sched_affinity_bitmask> dl_cpu_masks;
      for (unsigned w = 0; w != nof_dl_workers; ++w) {
        dl_cpu_masks.push_back(affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::l1_dl));
      }

      // Instantiate PHY UL workers.
      create_worker_pool(name_ul,
                         nof_ul_workers,
                         task_worker_queue_size,
                         {{"upper_pusch_exec#" + cell_id_str}, {"upper_pucch_exec#" + cell_id_str}},
                         prio,
                         ul_cpu_masks);
      upper_pusch_exec.push_back(exec_mng.executors().at("upper_pusch_exec#" + cell_id_str));
      upper_pucch_exec.push_back(exec_mng.executors().at("upper_pucch_exec#" + cell_id_str));

      // Instantiate dedicated PRACH worker.
      const std::string name_prach = "phy_prach#" + cell_id_str;
      const std::string prach_exec = "prach_exec#" + cell_id_str;
      create_prio_worker(name_prach,
                         task_worker_queue_size,
                         {{prach_exec}},
                         affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::l1_ul),
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
  for (unsigned cell_id = 0, cell_end = cells_cfg.size(); cell_id != cell_end; ++cell_id) {
    if (nof_pusch_decoder_workers > 0) {
      const std::string                      cell_id_str        = std::to_string(cell_id);
      const std::string                      name_pusch_decoder = "pusch#" + cell_id_str;
      const auto                             prio               = os_thread_realtime_priority::max() - 30;
      std::vector<os_sched_affinity_bitmask> cpu_masks;
      for (unsigned w = 0; w != nof_pusch_decoder_workers; ++w) {
        cpu_masks.push_back(affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::low_priority));
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

void worker_manager::create_ofh_executors(span<const cell_appconfig> cells, bool is_downlink_parallelized)
{
  using namespace execution_config_helper;

  // Maximum number of threads per cell. Implementation defined. The 3 threads are: transmission, reception and
  // codification.
  static constexpr unsigned MAX_NUM_THREADS_PER_CELL = 3U;

  unsigned nof_host_threads = std::max(4U, std::max(std::thread::hardware_concurrency(), 4U) - 3U);

  if (nof_host_threads < (cells.size() * MAX_NUM_THREADS_PER_CELL) + 1) {
    fmt::print(
        "Detected {} threads for Open Fronthaul, but a minimum of {} are needed to achieve a good performance.\n",
        nof_host_threads,
        (cells.size() * MAX_NUM_THREADS_PER_CELL) + 1);
  }

  // Timing executor.
  {
    const std::string name      = "ru_timing";
    const std::string exec_name = "ru_timing_exec";

    const single_worker ru_worker{name,
                                  {concurrent_queue_policy::lockfree_spsc, 4},
                                  {{exec_name}},
                                  std::chrono::microseconds{0},
                                  os_thread_realtime_priority::max() - 0,
                                  affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::ru)};
    if (!exec_mng.add_execution_context(create_execution_context(ru_worker))) {
      report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
    }
    ru_timing_exec = exec_mng.executors().at(exec_name);
  }

  for (unsigned i = 0, e = cells.size(); i != e; ++i) {
    // Executor for the Open Fronthaul User and Control messages codification.
    {
      ru_dl_exec.emplace_back();
      unsigned nof_ofh_dl_workers = (is_downlink_parallelized) ? std::max(cells[i].cell.nof_antennas_dl / 2U, 1U) : 1U;
      const std::string name      = "ru_dl_#" + std::to_string(i);
      const std::string exec_name = "ru_dl_exec_#" + std::to_string(i);

      const auto                             prio = os_thread_realtime_priority::max() - 5;
      std::vector<os_sched_affinity_bitmask> cpu_masks;
      for (unsigned w = 0; w != nof_ofh_dl_workers; ++w) {
        cpu_masks.push_back(affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::ru));
      }
      create_worker_pool(name, nof_ofh_dl_workers, task_worker_queue_size, {{exec_name}}, prio, cpu_masks);
      ru_dl_exec[i] = exec_mng.executors().at(exec_name);
    }

    // Executor for Open Fronthaul messages transmission.
    {
      const std::string name      = "ru_tx_" + std::to_string(i);
      const std::string exec_name = "ru_tx_exec_" + std::to_string(i);

      const single_worker ru_worker{name,
                                    {concurrent_queue_policy::lockfree_spsc, task_worker_queue_size},
                                    {{exec_name}},
                                    std::chrono::microseconds{5},
                                    os_thread_realtime_priority::max() - 1,
                                    affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::ru)};
      if (not exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_tx_exec.push_back(exec_mng.executors().at(exec_name));
    }

    // Executor for Open Fronthaul messages reception.
    {
      const std::string name      = "ru_rx_" + std::to_string(i);
      const std::string exec_name = "ru_rx_exec_" + std::to_string(i);

      const single_worker ru_worker{name,
                                    {concurrent_queue_policy::lockfree_spsc, 2},
                                    {{exec_name}},
                                    std::chrono::microseconds{1},
                                    os_thread_realtime_priority::max() - 1,
                                    affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::ru)};
      if (not exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_rx_exec.push_back(exec_mng.executors().at(exec_name));
    }
  }
}

void worker_manager::create_lower_phy_executors(lower_phy_thread_profile lower_phy_profile, unsigned nof_cells)
{
  using namespace execution_config_helper;

  // Radio Unit worker and executor.
  create_prio_worker("radio",
                     task_worker_queue_size,
                     {{"radio_exec"}},
                     affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::ru));
  radio_exec = exec_mng.executors().at("radio_exec");

  // Radio Unit statistics worker and executor.
  create_prio_worker("ru_stats_worker",
                     1,
                     {{"ru_printer_exec"}},
                     affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::low_priority));
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
                           affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::ru),
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
                           affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max());
        create_prio_worker(name_ul,
                           2,
                           {{exec_ul}},
                           affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::ru),
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
                           affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max());
        create_prio_worker(name_rx,
                           1,
                           {{exec_rx}},
                           affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max() - 2);
        create_prio_worker(name_dl,
                           128,
                           {{exec_dl}},
                           affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max() - 1);
        create_prio_worker(name_ul,
                           128,
                           {{exec_ul}},
                           affinity_mng.calcute_affinity_mask(gnb_sched_affinity_mask_types::ru),
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

void worker_manager::create_ru_executors(const gnb_appconfig& appcfg)
{
  if (variant_holds_alternative<ru_ofh_appconfig>(appcfg.ru_cfg)) {
    create_ofh_executors(appcfg.cells_cfg, appcfg.expert_execution_cfg.threads.ofh_threads.is_downlink_parallelized);

    return;
  }

  const ru_sdr_appconfig& sdr_cfg = variant_get<ru_sdr_appconfig>(appcfg.ru_cfg);
  std::string             driver  = sdr_cfg.device_driver;

  create_lower_phy_executors((driver != "zmq") ? appcfg.expert_execution_cfg.threads.lower_threads.execution_profile
                                               : lower_phy_thread_profile::blocking,
                             appcfg.cells_cfg.size());
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
