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
#include "apps/helpers/metrics/metrics_helpers.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/ru/ofh/ru_ofh_executor_mapper_factory.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/concurrent_metrics_executor.h"
#include "srsran/support/executors/inline_task_executor.h"
#include "srsran/support/executors/strand_executor.h"

using namespace srsran;

static const uint32_t task_worker_queue_size = 2048;

namespace {

/// Observer that preinitializes the byte buffer pool and timers resources in each created thread.
class thread_resource_preinitializer final : public unique_thread::observer
{
public:
  thread_resource_preinitializer(timer_manager& timers_) : timers(timers_) {}

  void on_thread_creation() override
  {
    // Pre-initialize thread-local resources to avoid doing it in the critical path.
    init_byte_buffer_segment_pool_tls();
    // Pre-initialize timer queues to avoid doing it in the critical path.
    inline_task_executor dummy_executor;
    auto                 dummy_timer = timers.create_unique_timer(dummy_executor);
    dummy_timer.stop();
  }
  void on_thread_destruction() override {}

private:
  timer_manager& timers;
};

class pcap_executor_mapper_impl final : public gnb_pcap_executor_mapper
{
public:
  pcap_executor_mapper_impl(const worker_manager_config::pcap_config& config, task_executor& pool_task_exec)
  {
    // These layers have very low throughput, so no point in instantiating more than one strand.
    // This means that there is no parallelization in pcap writing across these layers.
    if (config.is_f1ap_enabled or config.is_ngap_enabled or config.is_e1ap_enabled or config.is_e2ap_enabled) {
      common_exec =
          make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(pool_task_exec, task_worker_queue_size);
    }

    if (config.is_n3_enabled) {
      n3_exec = make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(pool_task_exec, task_worker_queue_size);
    }
    if (config.is_f1u_enabled) {
      f1u_exec = make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(pool_task_exec, task_worker_queue_size);
    }
    if (config.is_mac_enabled) {
      mac_exec = make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(pool_task_exec, task_worker_queue_size);
    }
    if (config.is_rlc_enabled) {
      rlc_exec = make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(pool_task_exec, task_worker_queue_size);
    }
  }

  task_executor& get_f1ap_executor() override { return *common_exec; }
  task_executor& get_ngap_executor() override { return *common_exec; }
  task_executor& get_e1ap_executor() override { return *common_exec; }
  task_executor& get_e2ap_executor() override { return *common_exec; }
  task_executor& get_n3_executor() override { return *n3_exec; }
  task_executor& get_f1u_executor() override { return *f1u_exec; }
  task_executor& get_mac_executor() override { return *mac_exec; }
  task_executor& get_rlc_executor() override { return *rlc_exec; }

private:
  std::unique_ptr<task_executor> common_exec;
  std::unique_ptr<task_executor> n3_exec;
  std::unique_ptr<task_executor> f1u_exec;
  std::unique_ptr<task_executor> mac_exec;
  std::unique_ptr<task_executor> rlc_exec;
};

} // namespace

worker_manager::worker_manager(const worker_manager_config& worker_cfg) :
  app_logger(srslog::fetch_basic_logger("ALL")), low_prio_affinity_mng({worker_cfg.main_pool_affinity_cfg})
{
  // Add preinitialization of resources to created threads.
  unique_thread::add_observer(std::make_unique<thread_resource_preinitializer>(*worker_cfg.app_timers));

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

  // Create the main worker pool for the application.
  create_main_worker_pool(worker_cfg);

  create_support_strands(worker_cfg);

  if (worker_cfg.is_split6_enabled) {
    create_split6_executors();
  }

  if (worker_cfg.cu_cp_cfg) {
    create_cu_cp_executors(*worker_cfg.cu_cp_cfg, *worker_cfg.app_timers);
  }

  if (worker_cfg.cu_up_cfg) {
    create_cu_up_executors(*worker_cfg.cu_up_cfg, *worker_cfg.app_timers);
  }

  if (worker_cfg.du_hi_cfg) {
    create_du_high_executors(*worker_cfg.du_hi_cfg);
  }

  if (worker_cfg.fapi_cfg) {
    create_fapi_executors(*worker_cfg.fapi_cfg);
  }

  if (worker_cfg.du_low_cfg) {
    create_du_low_executors(*worker_cfg.du_low_cfg);
  }

  if (worker_cfg.ru_ofh_cfg) {
    create_ofh_executors(*worker_cfg.ru_ofh_cfg);
  }

  if (worker_cfg.ru_sdr_cfg) {
    create_lower_phy_executors(worker_cfg.ru_sdr_cfg.value());
  }

  if (worker_cfg.ru_dummy_cfg) {
    create_ru_dummy_executors();
  }
}

worker_manager::~worker_manager()
{
  stop();
}

void worker_manager::stop()
{
  if (running) {
    running = false;
    app_logger.info("Stopping workers...");
    exec_mng.stop();
    app_logger.info("Workers stopped successfully.");
  }
}

void worker_manager::create_worker_pool(const std::string&                    name,
                                        unsigned                              nof_workers,
                                        const std::string&                    exec_name,
                                        unsigned                              queue_size,
                                        os_thread_realtime_priority           prio,
                                        span<const os_sched_affinity_bitmask> cpu_masks,
                                        concurrent_queue_policy               queue_policy)
{
  using namespace execution_config_helper;

  const worker_pool pool{name,
                         nof_workers,
                         {{exec_name, queue_policy, queue_size}},
                         std::chrono::microseconds{queue_policy == concurrent_queue_policy::locking_mpmc ? 0 : 20},
                         prio,
                         std::vector<os_sched_affinity_bitmask>{cpu_masks.begin(), cpu_masks.end()}};
  if (not exec_mng.add_execution_context(create_execution_context(pool))) {
    report_fatal_error("Failed to instantiate {} execution context", pool.name);
  }
}

void worker_manager::create_prio_worker(const std::string&               name,
                                        const std::string&               exec_name,
                                        unsigned                         queue_size,
                                        const os_sched_affinity_bitmask& mask,
                                        os_thread_realtime_priority      prio)
{
  using namespace execution_config_helper;

  const single_worker worker_desc{
      name, {exec_name, concurrent_queue_policy::locking_mpsc, queue_size}, std::nullopt, prio, mask};
  if (not exec_mng.add_execution_context(create_execution_context(worker_desc))) {
    report_fatal_error("Failed to instantiate {} execution context", worker_desc.name);
  }
}

void worker_manager::add_pcap_strands(const worker_manager_config::pcap_config& config)
{
  using namespace execution_config_helper;

  pcap_exec_mapper = std::make_unique<pcap_executor_mapper_impl>(config, *non_rt_medium_prio_exec);
}

std::vector<execution_config_helper::single_worker> worker_manager::create_fapi_workers(unsigned nof_cells)
{
  using namespace execution_config_helper;
  std::vector<single_worker> workers;

  for (unsigned cell_id = 0; cell_id != nof_cells; ++cell_id) {
    const std::string name      = "fapi#" + std::to_string(cell_id);
    const std::string exec_name = "fapi_exec#" + std::to_string(cell_id);

    single_worker buffered_worker{name,
                                  {exec_name, concurrent_queue_policy::locking_mpsc, task_worker_queue_size},
                                  std::chrono::microseconds{50},
                                  os_thread_realtime_priority::max() - 6};

    workers.push_back(buffered_worker);
  }

  return workers;
}

void worker_manager::create_cu_cp_executors(const worker_manager_config::cu_cp_config& config, timer_manager& timers)
{
  cu_cp_exec_mapper = srs_cu_cp::make_cu_cp_executor_mapper(
      srs_cu_cp::strand_based_executor_config{*non_rt_hi_prio_exec, config.metrics_period});
}

void worker_manager::create_cu_up_executors(const worker_manager_config::cu_up_config& config, timer_manager& timers)
{
  using namespace execution_config_helper;

  cu_up_exec_mapper =
      srs_cu_up::make_cu_up_executor_mapper(srs_cu_up::strand_based_executor_config{config.max_nof_ue_strands,
                                                                                    task_worker_queue_size,
                                                                                    config.dl_ue_executor_queue_size,
                                                                                    config.ul_ue_executor_queue_size,
                                                                                    config.ctrl_ue_executor_queue_size,
                                                                                    config.strand_batch_size,
                                                                                    *non_rt_medium_prio_exec,
                                                                                    *non_rt_low_prio_exec,
                                                                                    *non_rt_hi_prio_exec,
                                                                                    config.dedicated_io_ul_strand,
                                                                                    &timers,
                                                                                    config.executor_tracing_enable,
                                                                                    config.metrics_period});
}

void worker_manager::create_du_high_executors(const worker_manager_config::du_high_config& du_hi)
{
  srs_du::du_high_executor_config cfg;

  srs_du::du_high_executor_config::strand_based_worker_pool pool_desc;
  pool_desc.nof_cells                  = du_hi.nof_cells;
  pool_desc.default_task_queue_size    = task_worker_queue_size;
  pool_desc.pool_executors             = {rt_hi_prio_exec};
  cfg.cell_executors                   = pool_desc;
  cfg.ue_executors.policy              = srs_du::du_high_executor_config::ue_executor_config::map_policy::per_cell;
  cfg.ue_executors.max_nof_strands     = 1;
  cfg.ue_executors.ctrl_queue_size     = task_worker_queue_size;
  cfg.ue_executors.pdu_queue_size      = du_hi.ue_data_tasks_queue_size;
  cfg.ue_executors.pool_executor       = non_rt_medium_prio_exec;
  cfg.ue_executors.f1u_reader_executor = non_rt_low_prio_exec;
  cfg.ctrl_executors.task_queue_size   = task_worker_queue_size;
  cfg.ctrl_executors.pool_executor     = non_rt_hi_prio_exec;
  cfg.is_rt_mode_enabled               = du_hi.is_rt_mode_enabled;
  cfg.trace_exec_tasks                 = du_hi.executor_tracing_enable;
  cfg.metrics_period                   = du_hi.metrics_period;

  du_high_exec_mapper = create_du_high_executor_mapper(cfg);
}

void worker_manager::create_fapi_executors(const worker_manager_config::fapi_config& fapi_cfg)
{
  // FAPI message buffering executors.
  // TODO: Use executor mapper and main pool.
  fapi_exec.resize(fapi_cfg.nof_cells);
  std::fill(fapi_exec.begin(), fapi_exec.end(), nullptr);
  // Create workers.
  auto workers = create_fapi_workers(fapi_cfg.nof_cells);

  for (unsigned cell_id = 0; cell_id != fapi_cfg.nof_cells; ++cell_id) {
    // Create executor and associated workers.
    if (!exec_mng.add_execution_context(create_execution_context(workers[cell_id]))) {
      report_fatal_error("Failed to instantiate {} execution context", workers[cell_id].name);
    }

    // Associate executor.
    fapi_exec[cell_id] = exec_mng.executors().at(workers[cell_id].queue.name);
  }
}

/// Makes an estimation of the needed number of threads for the application.
static unsigned get_default_nof_workers(const worker_manager_config& worker_cfg)
{
  unsigned nof_workers = 0;
  if (worker_cfg.nof_main_pool_threads.has_value()) {
    nof_workers = worker_cfg.nof_main_pool_threads.value();
  } else {
    // Estimation of minimum number of workers needed for the application using the formula:
    // nof_workers = nof_cells * (nof_dl_antennas + nof_ul_antennas + 1) + 2
    if (worker_cfg.du_low_cfg.has_value()) {
      for (unsigned i = 0, e = worker_cfg.du_low_cfg->cell_nof_dl_antennas.size(); i != e; ++i) {
        nof_workers += worker_cfg.du_low_cfg->cell_nof_dl_antennas[i] + worker_cfg.du_low_cfg->cell_nof_ul_antennas[i];
      }
    }
    if (worker_cfg.du_hi_cfg.has_value()) {
      nof_workers += worker_cfg.du_hi_cfg->nof_cells * 1;
    }
    nof_workers += 2;
  }

  unsigned max_cpus = worker_cfg.main_pool_affinity_cfg.mask.count();
  if (max_cpus == 0) {
    max_cpus = cpu_architecture_info::get().get_host_nof_available_cpus();
  }

  // Allow some spare capacity for, e.g., processing of packets by the kernel, RU timing, etc.
  const unsigned spare_cpus  = 3;
  const unsigned min_workers = 4;
  return std::max(std::min(nof_workers, max_cpus - spare_cpus), min_workers);
}

/// Helper to calculate an upper bound on the number of preallocated producers for the main worker pool.
static unsigned get_nof_prealloc_producers(const worker_manager_config& worker_cfg, unsigned nof_workers_main_pool)
{
  // +2 to account for epoll broker and main.
  unsigned nof_producers = nof_workers_main_pool + 2;
  if (worker_cfg.ru_ofh_cfg.has_value()) {
    // Include RU timing thread.
    nof_producers += 1;
    // Include a TxRx thread for each RU cell.
    nof_producers += worker_cfg.ru_ofh_cfg->nof_cells;
  }
  if (worker_cfg.ru_dummy_cfg.has_value()) {
    // Include RU dummy thread.
    nof_producers += 1;
  }
  if (worker_cfg.ru_sdr_cfg.has_value()) {
    // Include threads for each SDR cell and layer.
    nof_producers += 4 * worker_cfg.ru_sdr_cfg->nof_cells;
  }

  return nof_producers;
}

void worker_manager::create_main_worker_pool(const worker_manager_config& worker_cfg)
{
  using namespace execution_config_helper;

  // Configure main worker pool.
  nof_workers_general_pool        = get_default_nof_workers(worker_cfg);
  const auto     worker_pool_prio = os_thread_realtime_priority::max() - 2;
  const unsigned qsize            = worker_cfg.main_pool_task_queue_size;
  // Estimation of an upper bound on the number of implicit producers that are required.
  const unsigned nof_producers = get_nof_prealloc_producers(worker_cfg, nof_workers_general_pool);

  worker_pool main_pool{"main_pool",
                        nof_workers_general_pool,
                        // Used for upper PHY DL and MAC scheduling
                        {{"rt_prio_exec", concurrent_queue_policy::moodycamel_lockfree_mpmc, qsize, nof_producers},
                         // Used for control plane and timer management.
                         {"high_prio_exec", concurrent_queue_policy::moodycamel_lockfree_mpmc, qsize, nof_producers},
                         // Used for PCAP writing and CU-UP.
                         {"medium_prio_exec", concurrent_queue_policy::moodycamel_lockfree_mpmc, qsize, nof_producers},
                         // Used for receiving data from external nodes.
                         {"low_prio_exec", concurrent_queue_policy::moodycamel_lockfree_mpmc, qsize, nof_producers}},
                        worker_cfg.main_pool_backoff_period,
                        worker_pool_prio,
                        std::vector<os_sched_affinity_bitmask>{worker_cfg.main_pool_affinity_cfg.mask}};

  // Create main worker pool.
  if (not exec_mng.add_execution_context(create_execution_context(main_pool))) {
    report_fatal_error("Failed to instantiate {} execution context", main_pool.name);
  }

  non_rt_low_prio_exec    = exec_mng.executors().at("low_prio_exec");
  non_rt_medium_prio_exec = exec_mng.executors().at("medium_prio_exec");
  non_rt_hi_prio_exec     = exec_mng.executors().at("high_prio_exec");
  rt_hi_prio_exec         = exec_mng.executors().at("rt_prio_exec");

  srslog::fetch_basic_logger("CONFIG").info("Worker pool \"{}\" instantiated with #workers={}, "
                                            "task_queue_size={}, backoff_period={}us and priority={}.",
                                            main_pool.name,
                                            nof_workers_general_pool,
                                            qsize,
                                            worker_cfg.main_pool_backoff_period.count(),
                                            worker_pool_prio.native());
}

void worker_manager::create_support_strands(const worker_manager_config& worker_cfg)
{
  using namespace execution_config_helper;

  // Configuration of strands for PCAP writing. These strands will use the medium priority executor.
  add_pcap_strands(worker_cfg.pcap_cfg);

  // Metrics strand configuration and instantiation.
  auto metric_strand =
      make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(non_rt_medium_prio_exec, task_worker_queue_size);
  metrics_exec = metric_strand.get();
  executor_decorators_exec.push_back(std::move(metric_strand));

  // Timer source strand configuration and instantiation.
  auto timer_source_strand =
      make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(non_rt_hi_prio_exec, task_worker_queue_size);
  timer_source_exec = timer_source_strand.get();
  executor_decorators_exec.push_back(std::move(timer_source_strand));
}

void worker_manager::create_du_low_executors(const worker_manager_config::du_low_config& du_low)
{
  using namespace execution_config_helper;

  // DU low executor mapper configuration.
  srs_du::du_low_executor_mapper_config du_low_exec_mapper_config;

  const bool rt_mode = !du_low.is_sequential_mode_active;

  // Instantiate workers for the DU-low.
  if (not rt_mode) {
    // Create a single worker, shared by the whole PHY. As it is shared for all the PHY, pick the first cell of the
    // affinity manager.
    create_prio_worker("phy_worker",
                       "phy_exec",
                       task_worker_queue_size,
                       affinity_mng.front().calcute_affinity_mask(sched_affinity_mask_types::l1_dl),
                       os_thread_realtime_priority::no_realtime());

    // Get common physical layer executor.
    task_executor* phy_exec = exec_mng.executors().at("phy_exec");

    // Fill the task executors.
    du_low_exec_mapper_config.executors =
        srs_du::du_low_executor_mapper_single_exec_config{.common_executor = {phy_exec, nof_workers_general_pool}};

  } else {
    // RF case.

    // Fill the task executors for each cell.
    du_low_exec_mapper_config.executors = srs_du::du_low_executor_mapper_flexible_exec_config{
        .rt_hi_prio_exec               = {rt_hi_prio_exec, nof_workers_general_pool},
        .non_rt_hi_prio_exec           = {non_rt_hi_prio_exec, nof_workers_general_pool},
        .non_rt_medium_prio_exec       = {non_rt_medium_prio_exec, nof_workers_general_pool},
        .non_rt_low_prio_exec          = {non_rt_low_prio_exec, nof_workers_general_pool},
        .max_pucch_concurrency         = du_low.max_pucch_concurrency,
        .max_pusch_and_srs_concurrency = du_low.max_pusch_and_srs_concurrency,
        .max_pdsch_concurrency         = du_low.max_pdsch_concurrency};

    // Setup metrics configuration.
    if (du_low.metrics_period.has_value()) {
      du_low_exec_mapper_config.metrics.emplace(
          srs_du::du_low_executor_mapper_metric_config{.period              = *du_low.metrics_period,
                                                       .sequential_executor = *metrics_exec,
                                                       .logger = app_helpers::fetch_logger_metrics_log_channel()});
    }
  }

  // Create DU low executor mapper.
  du_low_exec_mapper = srs_du::create_du_low_executor_mapper(du_low_exec_mapper_config);
}

void worker_manager::create_ofh_executors(const worker_manager_config::ru_ofh_config& config)
{
  using namespace execution_config_helper;

  ru_timing_mask         = config.ru_timing_cpu;
  ru_txrx_affinity_masks = config.txrx_affinities;

  ru_ofh_executor_mapper_config exec_mapper_config;

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
                                  {exec_name, concurrent_queue_policy::locking_mpsc, 4},
                                  std::nullopt,
                                  os_thread_realtime_priority::max() - 0,
                                  ru_timing_cpu_mask};
    if (!exec_mng.add_execution_context(create_execution_context(ru_worker))) {
      report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
    }
    exec_mapper_config.timing_executor = exec_mng.executors().at(exec_name);
  }

  // Downlink and uplink executors.
  exec_mapper_config.downlink_executor = rt_hi_prio_exec;
  exec_mapper_config.uplink_executor   = rt_hi_prio_exec;

  // Executor for Open Fronthaul messages transmission and reception.
  {
    for (unsigned i = 0, e = ru_txrx_affinity_masks.size(); i != e; ++i) {
      const std::string name      = "ru_txrx_#" + std::to_string(i);
      const std::string exec_name = "ru_txrx_exec_#" + std::to_string(i);

      const single_worker ru_worker{name,
                                    {exec_name, concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                                    std::chrono::microseconds{1},
                                    os_thread_realtime_priority::max() - 1,
                                    ru_txrx_affinity_masks[i]};
      if (not exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      exec_mapper_config.txrx_executors.push_back(exec_mng.executors().at(exec_name));
    }
  }

  exec_mapper_config.nof_sectors = config.nof_cells;

  // Create executor mapper.
  ofh_exec_mapper = create_ofh_ru_executor_mapper(exec_mapper_config);
}

void worker_manager::create_split6_executors()
{
  using namespace execution_config_helper;

  const std::string name      = "split6";
  const std::string exec_name = "split6_exec";

  const single_worker split6_worker{name,
                                    {exec_name, concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                                    std::chrono::microseconds{10},
                                    os_thread_realtime_priority::max() - 1};
  if (!exec_mng.add_execution_context(create_execution_context(split6_worker))) {
    report_fatal_error("Failed to instantiate {} execution context", split6_worker.name);
  }

  split6_exec = exec_mng.executors().at(exec_name);

  // Split6 control executor.
  auto split6_ctrl_strand =
      make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(non_rt_medium_prio_exec, task_worker_queue_size);
  split6_crtl_exec = split6_ctrl_strand.get();
  executor_decorators_exec.push_back(std::move(split6_ctrl_strand));
}

void worker_manager::create_lower_phy_executors(const worker_manager_config::ru_sdr_config& config)
{
  using namespace execution_config_helper;

  // Radio Unit worker and executor. As the radio is unique per application, use the first cell of the affinity manager.
  create_prio_worker("radio",
                     "radio_exec",
                     task_worker_queue_size,
                     affinity_mng.front().calcute_affinity_mask(sched_affinity_mask_types::ru));

  switch (config.profile) {
    case worker_manager_config::ru_sdr_config::lower_phy_thread_profile::sequential: {
      fmt::print("Lower PHY in executor sequential baseband mode.\n");
      std::string    exec_name = "phy_exec";
      task_executor* phy_exec  = exec_mng.executors().at(exec_name);

      ru_generic_executor_mapper_sequential_configuration ru_sdr_exec_map_config;
      ru_sdr_exec_map_config.asynchronous_exec = exec_mng.executors().at("radio_exec");
      ru_sdr_exec_map_config.common_exec       = phy_exec;
      ru_sdr_exec_map_config.nof_sectors       = config.nof_cells;
      sdr_exec_mapper                          = create_ru_generic_executor_mapper(ru_sdr_exec_map_config);
      break;
    }
    case worker_manager_config::ru_sdr_config::lower_phy_thread_profile::single: {
      fmt::print("Lower PHY in single baseband executor mode.\n");
      ru_generic_executor_mapper_single_configuration ru_sdr_exec_map_config;
      ru_sdr_exec_map_config.radio_exec         = exec_mng.executors().at("radio_exec");
      ru_sdr_exec_map_config.high_prio_executor = rt_hi_prio_exec;

      for (unsigned cell_id = 0; cell_id != config.nof_cells; ++cell_id) {
        const std::string name      = "lower_phy#" + std::to_string(cell_id);
        const std::string exec_name = "lower_phy_exec#" + std::to_string(cell_id);

        create_prio_worker(name,
                           exec_name,
                           128,
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max());

        task_executor* baseband_exec = exec_mng.executors().at(exec_name);
        ru_sdr_exec_map_config.baseband_exec.emplace_back(baseband_exec);
      }
      sdr_exec_mapper = create_ru_generic_executor_mapper(ru_sdr_exec_map_config);
      break;
    }
    case worker_manager_config::ru_sdr_config::lower_phy_thread_profile::dual: {
      fmt::print("Lower PHY in dual baseband executor mode.\n");
      ru_generic_executor_mapper_dual_configuration ru_sdr_exec_map_config;
      ru_sdr_exec_map_config.radio_exec         = exec_mng.executors().at("radio_exec");
      ru_sdr_exec_map_config.high_prio_executor = rt_hi_prio_exec;

      for (unsigned cell_id = 0; cell_id != config.nof_cells; ++cell_id) {
        const std::string name_tx = "lower_phy_tx#" + std::to_string(cell_id);
        const std::string exec_tx = "lower_phy_tx_exec#" + std::to_string(cell_id);
        const std::string name_rx = "lower_phy_rx#" + std::to_string(cell_id);
        const std::string exec_rx = "lower_phy_rx_exec#" + std::to_string(cell_id);

        create_prio_worker(name_tx,
                           exec_tx,
                           128,
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max());
        create_prio_worker(name_rx,
                           exec_rx,
                           2,
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max() - 1);

        ru_sdr_exec_map_config.baseband_exec.emplace_back(ru_generic_executor_mapper_dual_configuration::cell_executors{
            .tx_exec = exec_mng.executors().at(exec_tx), .rx_exec = exec_mng.executors().at(exec_rx)});
      }
      sdr_exec_mapper = create_ru_generic_executor_mapper(ru_sdr_exec_map_config);
      break;
    }
    case worker_manager_config::ru_sdr_config::lower_phy_thread_profile::triple: {
      fmt::print("Lower PHY in triple executor mode.\n");
      ru_generic_executor_mapper_triple_configuration ru_sdr_exec_map_config;
      ru_sdr_exec_map_config.radio_exec         = exec_mng.executors().at("radio_exec");
      ru_sdr_exec_map_config.high_prio_executor = rt_hi_prio_exec;

      for (unsigned cell_id = 0; cell_id != config.nof_cells; ++cell_id) {
        const std::string name_tx = "lower_phy_tx#" + std::to_string(cell_id);
        const std::string exec_tx = "lower_phy_tx_exec#" + std::to_string(cell_id);
        const std::string name_rx = "lower_phy_rx#" + std::to_string(cell_id);
        const std::string exec_rx = "lower_phy_rx_exec#" + std::to_string(cell_id);
        const std::string name_ul = "lower_phy_ul#" + std::to_string(cell_id);
        const std::string exec_ul = "lower_phy_ul_exec#" + std::to_string(cell_id);

        create_prio_worker(name_tx,
                           exec_tx,
                           128,
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max());
        create_prio_worker(name_rx,
                           exec_rx,
                           1,
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max() - 2);
        create_prio_worker(name_ul,
                           exec_ul,
                           128,
                           affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::ru),
                           os_thread_realtime_priority::max() - 1);

        ru_sdr_exec_map_config.baseband_exec.emplace_back(
            ru_generic_executor_mapper_triple_configuration::cell_executors{.tx_exec = exec_mng.executors().at(exec_tx),
                                                                            .rx_exec = exec_mng.executors().at(exec_rx),
                                                                            .ul_exec =
                                                                                exec_mng.executors().at(exec_ul)});
      }
      sdr_exec_mapper = create_ru_generic_executor_mapper(ru_sdr_exec_map_config);
      break;
    }
  }

  srsran_assert(sdr_exec_mapper, "Failed to create SDR executor mapper.");
}

void worker_manager::create_ru_dummy_executors()
{
  // Use the first cell of the affinity manager for the dummy RU.
  create_prio_worker("ru_dummy",
                     "ru_dummy_exec",
                     task_worker_queue_size,
                     affinity_mng.front().calcute_affinity_mask(sched_affinity_mask_types::ru),
                     os_thread_realtime_priority::max());
  dummy_exec_mapper = create_ru_dummy_executor_mapper(*exec_mng.executors().at("ru_dummy_exec"));
}
