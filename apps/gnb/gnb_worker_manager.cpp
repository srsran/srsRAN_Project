/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gnb_worker_manager.h"
#include "lib/du_high/du_high_executor_strategies.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/support/executors/sync_task_executor.h"

using namespace srsran;

static const uint32_t task_worker_queue_size = 2048;

worker_manager::worker_manager(const gnb_appconfig& appcfg)
{
  bool is_blocking_mode_active = false;
  if (variant_holds_alternative<ru_sdr_appconfig>(appcfg.ru_cfg)) {
    const ru_sdr_appconfig& sdr_cfg = variant_get<ru_sdr_appconfig>(appcfg.ru_cfg);
    is_blocking_mode_active         = sdr_cfg.device_driver == "zmq";
  }

  if (appcfg.expert_config.enable_tuned_affinity_profile) {
    use_tuned_profile = true;
    affinity_manager  = std::make_unique<affinity_mask_manager>(appcfg.expert_config.nof_threads_per_cpu,
                                                               appcfg.expert_config.nof_cores_for_non_prio_workers);
  } else {
    affinity_manager = std::make_unique<affinity_mask_manager>();
  }

  // If an OFH RU is configured, create its executors first.
  if (variant_holds_alternative<ru_ofh_appconfig>(appcfg.ru_cfg)) {
    create_ofh_executors(appcfg.cells_cfg, variant_get<ru_ofh_appconfig>(appcfg.ru_cfg).is_downlink_parallelized);
  }

  // Select the PDSCH concurrent thread only if the PDSCH processor type is set to concurrent or auto.
  unsigned nof_pdsch_threads = 1;
  if ((appcfg.expert_phy_cfg.pdsch_processor_type == "concurrent") ||
      (appcfg.expert_phy_cfg.pdsch_processor_type == "auto")) {
    nof_pdsch_threads = appcfg.expert_phy_cfg.nof_pdsch_threads;
  }

  create_du_cu_executors(is_blocking_mode_active,
                         appcfg.expert_phy_cfg.nof_ul_threads,
                         appcfg.expert_phy_cfg.nof_dl_threads,
                         nof_pdsch_threads,
                         appcfg.cells_cfg,
                         appcfg.expert_phy_cfg.max_processing_delay_slots);

  create_ru_executors(appcfg);
}

void worker_manager::stop()
{
  exec_mng.stop();
}

void worker_manager::create_worker_pool(const std::string&                                                 name,
                                        unsigned                                                           nof_workers,
                                        unsigned                                                           queue_size,
                                        const std::vector<execution_config_helper::worker_pool::executor>& execs,
                                        os_thread_realtime_priority                                        prio,
                                        span<const os_sched_affinity_bitmask>                              cpu_masks)
{
  using namespace execution_config_helper;

  concurrent_queue_policy queue_policy = concurrent_queue_policy::locking_mpmc;

  const worker_pool pool{name,
                         nof_workers,
                         {queue_policy, queue_size},
                         execs,
                         queue_policy == concurrent_queue_policy::locking_mpmc ? optional<std::chrono::microseconds>{}
                                                                               : std::chrono::microseconds{10},
                         prio,
                         std::vector<os_sched_affinity_bitmask>{cpu_masks.begin(), cpu_masks.end()}};
  if (not exec_mng.add_execution_context(create_execution_context(pool))) {
    report_fatal_error("Failed to instantiate {} execution context", pool.name);
  }
}

void worker_manager::create_prio_worker(const std::string&                                                   name,
                                        unsigned                                                             queue_size,
                                        const std::vector<execution_config_helper::single_worker::executor>& execs,
                                        os_thread_realtime_priority                                          prio)
{
  using namespace execution_config_helper;

  const single_worker worker_desc{name,
                                  {concurrent_queue_policy::locking_mpsc, queue_size},
                                  execs,
                                  nullopt,
                                  prio,
                                  calculate_affinity_mask(name, prio)};
  if (not exec_mng.add_execution_context(create_execution_context(worker_desc))) {
    report_fatal_error("Failed to instantiate {} execution context", worker_desc.name);
  }
}

void worker_manager::create_du_cu_executors(bool                       is_blocking_mode_active,
                                            unsigned                   nof_ul_workers,
                                            unsigned                   nof_dl_workers,
                                            unsigned                   nof_pdsch_workers,
                                            span<const cell_appconfig> cells_cfg,
                                            unsigned                   pipeline_depth)
{
  using namespace execution_config_helper;

  // Worker for handling UE PDU traffic.
  const priority_multiqueue_worker gnb_ue_worker{"gnb_ue",
                                                 // Two queues, one for generate UE tasks, and one for GTPU PDUs.
                                                 {{concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                                                  {concurrent_queue_policy::lockfree_spsc, task_worker_queue_size}},
                                                 std::chrono::microseconds{200},
                                                 {{"cu_up_exec", task_priority::max},
                                                  {"gtpu_pdu_exec", task_priority::min, false},
                                                  {"du_ue_exec", task_priority::max},
                                                  {"cu_up_e2_exec", task_priority::max}}};
  if (not exec_mng.add_execution_context(create_execution_context(gnb_ue_worker))) {
    report_fatal_error("Failed to instantiate gNB UE execution context");
  }
  cu_up_exec    = exec_mng.executors().at("cu_up_exec");
  gtpu_pdu_exec = exec_mng.executors().at("gtpu_pdu_exec");
  cu_up_e2_exec = exec_mng.executors().at("cu_up_e2_exec");

  // Worker for handling DU, CU and UE control procedures.
  const priority_multiqueue_worker gnb_ctrl_worker{
      "gnb_ctrl",
      {{concurrent_queue_policy::lockfree_spsc, 64}, {concurrent_queue_policy::locking_mpsc, task_worker_queue_size}},
      std::chrono::microseconds{200},
      // The handling of timer ticks has higher priority.
      {{"cu_cp_exec", task_priority::min},
       {"cu_cp_e2_exec", task_priority::min},
       {"metrics_hub_exec", task_priority::min},
       {"du_ctrl_exec", task_priority::min},
       {"du_timer_exec", task_priority::max},
       {"du_e2_exec", task_priority::min}},
      os_thread_realtime_priority::max() - 20,
      calculate_affinity_mask("gnb_ctrl", os_thread_realtime_priority::max() - 20)};
  if (not exec_mng.add_execution_context(create_execution_context(gnb_ctrl_worker))) {
    report_fatal_error("Failed to instantiate gNB control execution context");
  }
  cu_cp_exec       = exec_mng.executors().at("cu_cp_exec");
  cu_cp_e2_exec    = exec_mng.executors().at("cu_cp_e2_exec");
  metrics_hub_exec = exec_mng.executors().at("metrics_hub_exec");

  // Workers for handling cell slot indications of different cells.
  for (unsigned cell_id = 0; cell_id != cells_cfg.size(); ++cell_id) {
    const std::string                cell_id_str = std::to_string(cell_id);
    const priority_multiqueue_worker du_cell_worker{
        "du_cell#" + cell_id_str,
        {{concurrent_queue_policy::lockfree_spsc, 8}, {concurrent_queue_policy::locking_mpsc, task_worker_queue_size}},
        std::chrono::microseconds{10},
        // Create Cell and slot indication executors. In case of ZMQ, we make the slot indication executor synchronous.
        {{"cell_exec#" + cell_id_str, task_priority::min},
         {"slot_exec#" + cell_id_str, task_priority::max, true, is_blocking_mode_active}},
        os_thread_realtime_priority::max() - 2,
        calculate_affinity_mask("du_cell#" + cell_id_str, os_thread_realtime_priority::max() - 2)};

    if (not exec_mng.add_execution_context(create_execution_context(du_cell_worker))) {
      report_fatal_error("Failed to instantiate {} execution context", du_cell_worker.name);
    }
  }

  // Instantiate DU-high executor mapper.
  du_high_executors.resize(cells_cfg.size());
  for (unsigned i = 0, e = cells_cfg.size(); i != e; ++i) {
    auto&             du_item     = du_high_executors[i];
    const std::string cell_id_str = std::to_string(i);

    // DU-high executor mapper.
    const auto& exec_map  = exec_mng.executors();
    using exec_list       = std::initializer_list<task_executor*>;
    auto cell_exec_mapper = std::make_unique<cell_executor_mapper>(exec_list{exec_map.at("cell_exec#" + cell_id_str)},
                                                                   exec_list{exec_map.at("slot_exec#" + cell_id_str)});
    auto ue_exec_mapper   = std::make_unique<pcell_ue_executor_mapper>(exec_list{exec_map.at("du_ue_exec")});
    du_item.du_high_exec_mapper = std::make_unique<du_high_executor_mapper_impl>(std::move(cell_exec_mapper),
                                                                                 std::move(ue_exec_mapper),
                                                                                 *exec_map.at("du_ctrl_exec"),
                                                                                 *exec_map.at("du_timer_exec"),
                                                                                 *exec_map.at("du_e2_exec"));
  }

  create_du_low_executors(
      is_blocking_mode_active, nof_ul_workers, nof_dl_workers, nof_pdsch_workers, cells_cfg, pipeline_depth);
}

void worker_manager::create_du_low_executors(bool                       is_blocking_mode_active,
                                             unsigned                   nof_ul_workers,
                                             unsigned                   nof_dl_workers,
                                             unsigned                   nof_pdsch_workers,
                                             span<const cell_appconfig> cells_cfg,
                                             unsigned                   pipeline_depth)
{
  using namespace execution_config_helper;

  du_low_dl_executors.resize(cells_cfg.size());
  upper_pdsch_exec.resize(cells_cfg.size());

  if (is_blocking_mode_active) {
    // Create a single worker, shared by the whole PHY.
    create_prio_worker("phy_worker", task_worker_queue_size, {{"phy_exec"}}, os_thread_realtime_priority::max());

    for (unsigned cell_id = 0, cell_end = cells_cfg.size(); cell_id != cell_end; ++cell_id) {
      upper_pusch_exec.push_back(exec_mng.executors().at("phy_exec"));
      upper_pucch_exec.push_back(exec_mng.executors().at("phy_exec"));
      upper_prach_exec.push_back(exec_mng.executors().at("phy_exec"));
      du_low_dl_executors[cell_id].emplace_back(exec_mng.executors().at("phy_exec"));
    }

  } else {
    // RF case.
    for (unsigned cell_id = 0, cell_end = cells_cfg.size(); cell_id != cell_end; ++cell_id) {
      const std::string                      cell_id_str = std::to_string(cell_id);
      const std::string                      name_ul     = "up_phy_ul#" + cell_id_str;
      const auto                             prio        = os_thread_realtime_priority::max() - 20;
      std::vector<os_sched_affinity_bitmask> cpu_masks;
      if (use_tuned_profile) {
        for (unsigned w = 0; w != nof_ul_workers; ++w) {
          cpu_masks.push_back(affinity_manager->reserve_cpu(name_ul, prio));
        }
      }

      // Instantiate PHY UL workers.
      create_worker_pool(name_ul,
                         nof_ul_workers,
                         task_worker_queue_size,
                         {{"upper_pusch_exec#" + cell_id_str}, {"upper_pucch_exec#" + cell_id_str}},
                         prio,
                         cpu_masks);
      upper_pusch_exec.push_back(exec_mng.executors().at("upper_pusch_exec#" + cell_id_str));
      upper_pucch_exec.push_back(exec_mng.executors().at("upper_pucch_exec#" + cell_id_str));

      // Instantiate dedicated PRACH worker.
      const std::string name_prach = "phy_prach#" + cell_id_str;
      const std::string prach_exec = "prach_exec#" + cell_id_str;
      create_prio_worker(name_prach, task_worker_queue_size, {{prach_exec}}, os_thread_realtime_priority::max() - 2);
      upper_prach_exec.push_back(exec_mng.executors().at("prach_exec#" + cell_id_str));

      // Instantiate dedicated PHY DL workers.
      for (unsigned i_dl_worker = 0; i_dl_worker != nof_dl_workers; ++i_dl_worker) {
        // Create upper PHY DL executors.
        const std::string suffix      = std::to_string(cell_id) + "#" + std::to_string(i_dl_worker);
        const std::string worker_name = "up_phy_dl#" + suffix;
        const std::string exec_name   = "du_low_dl_exec#" + suffix;
        create_prio_worker(worker_name, task_worker_queue_size, {{exec_name}}, os_thread_realtime_priority::max() - 10);
        du_low_dl_executors[cell_id].emplace_back(exec_mng.executors().at("du_low_dl_exec#" + suffix));
      }
    }
  }

  for (unsigned cell_id = 0, cell_end = cells_cfg.size(); cell_id != cell_end; ++cell_id) {
    if (nof_pdsch_workers > 1) {
      const std::string name_pdsch = "pdsch#" + std::to_string(cell_id);
      unsigned          max_nof_pdsch_cb_slot =
          ((pdsch_constants::MAX_NRE_PER_RB * MAX_RB * get_bits_per_symbol(modulation_scheme::QAM256) *
            cells_cfg[cell_id].cell.nof_antennas_dl) /
           ldpc::MAX_MESSAGE_SIZE) *
          pipeline_depth;

      const auto                             prio = os_thread_realtime_priority::max() - 10;
      std::vector<os_sched_affinity_bitmask> cpu_masks;
      if (use_tuned_profile) {
        for (unsigned w = 0; w != nof_pdsch_workers; ++w) {
          cpu_masks.push_back(affinity_manager->reserve_cpu(name_pdsch, prio));
        }
      }

      create_worker_pool(name_pdsch,
                         nof_pdsch_workers,
                         max_nof_pdsch_cb_slot,
                         {{"upper_pdsch_exec#" + std::to_string(cell_id)}},
                         prio,
                         cpu_masks);
      upper_pdsch_exec[cell_id] = exec_mng.executors().at("upper_pdsch_exec#" + std::to_string(cell_id));
    }
  }
}

/// Returns an affinity bitmask using the given affinity mask manager.
static os_sched_affinity_bitmask
get_affinity_mask(affinity_mask_manager& manager, const std::string& name, unsigned priority_from_max)
{
  const os_thread_realtime_priority priority = os_thread_realtime_priority::max() - priority_from_max;
  return manager.reserve_cpu(name, priority);
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
                                  get_affinity_mask(*affinity_manager, name, 0)};
    if (not exec_mng.add_execution_context(create_execution_context(ru_worker))) {
      report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
    }
    ru_timing_exec = exec_mng.executors().at(exec_name);
  }

  for (unsigned i = 0, e = cells.size(); i != e; ++i) {
    ru_dl_exec.emplace_back();
    // Executor for the Open Fronthaul User and Control messages codification.
    unsigned dl_end = (is_downlink_parallelized) ? std::max(cells[i].cell.nof_antennas_dl / 2U, 1U) : 1U;
    for (unsigned dl_id = 0; dl_id != dl_end; ++dl_id) {
      const std::string name      = "ru_dl_#" + std::to_string(i) + "#" + std::to_string(dl_id);
      const std::string exec_name = "ru_dl_exec_#" + std::to_string(i) + "#" + std::to_string(dl_id);

      const single_worker ru_worker{name,
                                    {concurrent_queue_policy::locking_mpsc, task_worker_queue_size},
                                    {{exec_name}},
                                    nullopt,
                                    os_thread_realtime_priority::max() - 5,
                                    get_affinity_mask(*affinity_manager, name, 5)};
      if (not exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_dl_exec[i].push_back(exec_mng.executors().at(exec_name));
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
                                    get_affinity_mask(*affinity_manager, name, 1)};
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
                                    get_affinity_mask(*affinity_manager, name, 1)};
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
  create_prio_worker("radio", task_worker_queue_size, {{"radio_exec"}});
  radio_exec = exec_mng.executors().at("radio_exec");

  // Radio Unit statistics worker and executor.
  create_prio_worker("ru_stats_worker", 1, {{"ru_printer_exec"}});
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

        create_prio_worker(name, 128, {{exec_name}}, os_thread_realtime_priority::max());

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

        create_prio_worker(name_dl, 128, {{exec_dl}}, os_thread_realtime_priority::max());
        create_prio_worker(name_ul, 2, {{exec_ul}}, os_thread_realtime_priority::max() - 1);

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

        create_prio_worker(name_tx, 128, {{exec_tx}}, os_thread_realtime_priority::max());
        create_prio_worker(name_rx, 1, {{exec_rx}}, os_thread_realtime_priority::max() - 2);
        create_prio_worker(name_dl, 128, {{exec_dl}}, os_thread_realtime_priority::max() - 1);
        create_prio_worker(name_ul, 128, {{exec_ul}}, os_thread_realtime_priority::max() - 3);

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
    return;
  }

  const ru_sdr_appconfig& sdr_cfg = variant_get<ru_sdr_appconfig>(appcfg.ru_cfg);
  std::string             driver  = sdr_cfg.device_driver;

  create_lower_phy_executors((driver != "zmq") ? sdr_cfg.expert_cfg.lphy_executor_profile
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

os_sched_affinity_bitmask worker_manager::calculate_affinity_mask(const std::string&          worker_name,
                                                                  os_thread_realtime_priority prio)
{
  if (use_tuned_profile) {
    return affinity_manager->reserve_cpu(worker_name, prio);
  }
  return os_sched_affinity_bitmask{};
}
