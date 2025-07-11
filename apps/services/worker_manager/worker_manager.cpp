/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

/// Observer that preinitializes the byte buffer pool and timers resources in each created thread.
class thread_resource_preinitializer : public unique_thread::observer
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

namespace {

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
  low_prio_affinity_mng({worker_cfg.low_prio_sched_config})
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

  create_low_prio_worker_pool(worker_cfg);

  add_low_prio_strands(worker_cfg);

  if (worker_cfg.is_split6_enabled) {
    create_split6_executors();
  }

  if (worker_cfg.cu_cp_cfg) {
    create_cu_cp_executors(worker_cfg.cu_cp_cfg.value(), *worker_cfg.app_timers);
  }

  if (worker_cfg.cu_up_cfg) {
    create_cu_up_executors(worker_cfg.cu_up_cfg.value(), *worker_cfg.app_timers);
  }

  if (worker_cfg.du_hi_cfg) {
    create_du_executors(
        worker_cfg.du_hi_cfg.value(), worker_cfg.du_low_cfg, worker_cfg.fapi_cfg, *worker_cfg.app_timers);
  } else if (worker_cfg.du_low_cfg) {
    create_du_crit_path_prio_executors(*worker_cfg.du_low_cfg);
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
                                        span<const os_sched_affinity_bitmask>                 cpu_masks,
                                        concurrent_queue_policy                               queue_policy)
{
  using namespace execution_config_helper;

  const worker_pool pool{name,
                         nof_workers,
                         {{queue_policy, queue_size}},
                         execs,
                         std::chrono::microseconds{queue_policy == concurrent_queue_policy::locking_mpmc ? 0 : 20},
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

void worker_manager::add_pcap_strands(const worker_manager_config::pcap_config& config)
{
  using namespace execution_config_helper;

  pcap_exec_mapper = std::make_unique<pcap_executor_mapper_impl>(config, *exec_mng.executors().at("medium_prio_exec"));
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

/// Description of a dedicated single thread worker used for a single DU-high cell tasks (e.g. scheduling, RLC DL).
static execution_config_helper::priority_multiqueue_worker
create_dedicated_du_hi_slot_worker_desc(unsigned                                      cell_index,
                                        bool                                          rt_mode,
                                        const std::vector<os_sched_affinity_manager>& affinity_mng)
{
  using namespace execution_config_helper;

  const std::string cell_id_str = std::to_string(cell_index);

  std::string worker_name    = "du_cell#" + std::to_string(cell_index);
  std::string cell_exec_name = "cell_exec#" + std::to_string(cell_index);
  std::string slot_exec_name = "slot_exec#" + std::to_string(cell_index);

  // Description of a single worker.
  return priority_multiqueue_worker{
      worker_name,
      {{concurrent_queue_policy::lockfree_spsc, 4}, {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}},
      std::chrono::microseconds{10},
      {{cell_exec_name, task_priority::max - 1, {}, std::nullopt, !rt_mode},
       {slot_exec_name, task_priority::max, {}, std::nullopt, !rt_mode}},
      rt_mode ? os_thread_realtime_priority::max() - 2 : os_thread_realtime_priority::no_realtime(),
      affinity_mng[cell_index].calcute_affinity_mask(sched_affinity_mask_types::l2_cell)};
}

static srs_du::du_high_executor_config::dedicated_cell_worker_list
create_dedicated_du_hi_cell_executors(task_execution_manager&                       exec_mng,
                                      unsigned                                      nof_cells,
                                      bool                                          rt_mode,
                                      const std::vector<os_sched_affinity_manager>& affinity_mng)
{
  const auto& exec_map = exec_mng.executors();

  srs_du::du_high_executor_config::dedicated_cell_worker_list cell_workers;
  cell_workers.reserve(nof_cells);

  // Add one worker per cell.
  for (unsigned cell_idx = 0; cell_idx != nof_cells; ++cell_idx) {
    auto worker_desc = create_dedicated_du_hi_slot_worker_desc(cell_idx, rt_mode, affinity_mng);
    if (!exec_mng.add_execution_context(create_execution_context(worker_desc))) {
      report_fatal_error("Failed to instantiate {} execution context", worker_desc.name);
    }
    cell_workers.push_back(srs_du::du_high_executor_config::dedicated_cell_worker{
        exec_map.at("slot_exec#" + std::to_string(cell_idx)), exec_map.at("cell_exec#" + std::to_string(cell_idx))});
  }
  return cell_workers;
}

void worker_manager::create_cu_cp_executors(const worker_manager_config::cu_cp_config& config, timer_manager& timers)
{
  cu_cp_exec_mapper = srs_cu_cp::make_cu_cp_executor_mapper(
      srs_cu_cp::strand_based_executor_config{*exec_mng.executors().at("high_prio_exec"), config.metrics_period});
}

void worker_manager::create_cu_up_executors(const worker_manager_config::cu_up_config& config, timer_manager& timers)
{
  using namespace execution_config_helper;
  const auto& exec_map = exec_mng.executors();

  cu_up_exec_mapper =
      srs_cu_up::make_cu_up_executor_mapper(srs_cu_up::strand_based_executor_config{config.max_nof_ue_strands,
                                                                                    task_worker_queue_size,
                                                                                    config.dl_ue_executor_queue_size,
                                                                                    config.ul_ue_executor_queue_size,
                                                                                    config.ctrl_ue_executor_queue_size,
                                                                                    config.strand_batch_size,
                                                                                    *exec_map.at("medium_prio_exec"),
                                                                                    *exec_map.at("low_prio_exec"),
                                                                                    config.dedicated_io_ul_strand,
                                                                                    &timers,
                                                                                    config.executor_tracing_enable,
                                                                                    config.metrics_period});
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

  // Create L1 and L2 critical path executors.
  auto crit_path_exec_desc = du_low ? create_du_crit_path_prio_executors(*du_low)
                                    : create_du_crit_path_prio_executors(du_hi.nof_cells, du_hi.is_rt_mode_enabled);

  // Instantiate DU-high executor mapper.
  srs_du::du_high_executor_config cfg;
  cfg.cell_executors                 = crit_path_exec_desc.l2_execs;
  cfg.ue_executors.policy            = srs_du::du_high_executor_config::ue_executor_config::map_policy::per_cell;
  cfg.ue_executors.max_nof_strands   = 1;
  cfg.ue_executors.ctrl_queue_size   = task_worker_queue_size;
  cfg.ue_executors.pdu_queue_size    = du_hi.ue_data_tasks_queue_size;
  cfg.ue_executors.pool_executor     = exec_map.at("medium_prio_exec");
  cfg.ctrl_executors.task_queue_size = task_worker_queue_size;
  cfg.ctrl_executors.pool_executor   = exec_map.at("high_prio_exec");
  cfg.is_rt_mode_enabled             = du_hi.is_rt_mode_enabled;
  cfg.trace_exec_tasks               = du_hi.executor_tracing_enable;
  cfg.metrics_period                 = du_hi.metrics_period;

  du_high_exec_mapper = create_du_high_executor_mapper(cfg);
}

void worker_manager::create_low_prio_worker_pool(const worker_manager_config& worker_cfg)
{
  using namespace execution_config_helper;

  // Configure non-RT worker pool.
  const unsigned                  nof_workers = worker_cfg.nof_low_prio_threads;
  const std::chrono::microseconds sleep_time{100};
  const auto                      worker_pool_prio = os_thread_realtime_priority::min();
  const task_queue                qparams{concurrent_queue_policy::lockfree_mpmc, worker_cfg.low_prio_task_queue_size};

  worker_pool non_rt_pool{"non_rt_pool",
                          nof_workers,
                          {qparams, qparams, qparams},
                          // Used for control plane and timer management.
                          {{"high_prio_exec", task_priority::max},
                           // Used for PCAP writing and CU-UP.
                           {"medium_prio_exec", task_priority::max - 1},
                           // Used for receiving data from external nodes.
                           {"low_prio_exec", task_priority::max - 2}},
                          sleep_time,
                          worker_pool_prio,
                          {worker_cfg.low_prio_sched_config.mask}};

  // Create non-RT worker pool.
  if (not exec_mng.add_execution_context(create_execution_context(non_rt_pool))) {
    report_fatal_error("Failed to instantiate {} execution context", non_rt_pool.name);
  }
}

void worker_manager::add_low_prio_strands(const worker_manager_config& worker_cfg)
{
  using namespace execution_config_helper;
  non_rt_low_prio_exec    = exec_mng.executors().at("low_prio_exec");
  non_rt_medium_prio_exec = exec_mng.executors().at("medium_prio_exec");
  non_rt_hi_prio_exec     = exec_mng.executors().at("high_prio_exec");

  // Configuration of strands for PCAP writing. These strands will use the low priority executor.
  add_pcap_strands(worker_cfg.pcap_cfg);

  // Metrics strand configuration and instantiation.
  auto metric_strand =
      make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(non_rt_medium_prio_exec, task_worker_queue_size);
  metrics_exec = metric_strand.get();
  executor_decorators_exec.push_back(std::move(metric_strand));
}

worker_manager::du_crit_path_executor_desc worker_manager::create_du_crit_path_prio_executors(unsigned nof_cells,
                                                                                              bool     rt_mode)
{
  using namespace execution_config_helper;

  du_crit_path_executor_desc desc;
  // Need to create dedicated DU-high L2 threads as there is no DU-low.
  desc.l2_execs = create_dedicated_du_hi_cell_executors(exec_mng, nof_cells, rt_mode, affinity_mng);

  return desc;
}

worker_manager::du_crit_path_executor_desc
worker_manager::create_du_crit_path_prio_executors(const worker_manager_config::du_low_config& du_low)
{
  using namespace execution_config_helper;

  du_crit_path_executor_desc desc;

  // DU low executor mapper configuration.
  srs_du::du_low_executor_mapper_config du_low_exec_mapper_config;

  const unsigned nof_cells = du_low.nof_cells;
  const bool     rt_mode   = !du_low.is_blocking_mode_active;

  // Instantiate workers for the DU-low.
  if (not rt_mode) {
    // Create a single worker, shared by the whole PHY. As it is shared for all the PHY, pick the first cell of the
    // affinity manager.
    create_prio_worker("phy_worker",
                       task_worker_queue_size,
                       {{"phy_exec"}},
                       affinity_mng.front().calcute_affinity_mask(sched_affinity_mask_types::l1_dl),
                       os_thread_realtime_priority::no_realtime());

    // Get common physical layer executor.
    task_executor* phy_exec = exec_mng.executors().at("phy_exec");

    // Fill the task executors for each cell.
    for (unsigned cell_id = 0, cell_end = nof_cells; cell_id != cell_end; ++cell_id) {
      du_low_exec_mapper_config.cells.emplace_back(
          srs_du::du_low_executor_mapper_single_exec_config{.common_executor = phy_exec});
    }

    // Save DL executors for the higher layers.
    for (unsigned cell_id = 0, cell_end = nof_cells; cell_id != cell_end; ++cell_id) {
      crit_path_prio_executors.push_back(exec_mng.executors().at("phy_exec"));
    }

    // Need to create dedicated DU-high L2 threads as there is only one DU-low thread.
    desc.l2_execs = create_dedicated_du_hi_cell_executors(exec_mng, nof_cells, rt_mode, affinity_mng);

  } else {
    // RF case.
    unsigned nof_ul_workers = du_low.nof_ul_threads;
    unsigned nof_dl_workers = du_low.nof_dl_threads;

    std::vector<task_executor*> l2_execs;

    for (unsigned cell_id = 0, cell_end = nof_cells; cell_id != cell_end; ++cell_id) {
      const std::string               cell_id_str         = std::to_string(cell_id);
      const std::string               name_ul             = "up_phy_ul#" + cell_id_str;
      const std::string               name_dl             = "up_phy_dl#" + cell_id_str;
      const std::string               l1_dl_exec_name     = "du_low_dl_exec#" + cell_id_str;
      const std::string               l1_pdsch_exec_name  = "du_low_pdsch_exec#" + cell_id_str;
      const std::string               l1_pucch_exec_name  = "du_low_pucch_exec#" + cell_id_str;
      const std::string               l1_pusch_exec_name  = "du_low_pusch_exec#" + cell_id_str;
      const std::string               l1_srs_exec_name    = "du_low_srs_exec#" + cell_id_str;
      const std::string               l2_exec_name        = "l2_exec#" + cell_id_str;
      const std::string               l1_high_prio_name   = "l1_high_prio#" + cell_id_str;
      const auto                      ul_worker_pool_prio = os_thread_realtime_priority::max() - 15;
      const auto                      dl_worker_pool_prio = os_thread_realtime_priority::max() - 2;
      const std::chrono::microseconds ul_worker_sleep_time{20};
      const std::chrono::microseconds dl_worker_sleep_time{50};

      std::vector<os_sched_affinity_bitmask> ul_cpu_masks;
      for (unsigned w = 0; w != nof_ul_workers; ++w) {
        ul_cpu_masks.push_back(affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::l1_ul));
      }

      std::vector<os_sched_affinity_bitmask> dl_cpu_masks;
      for (unsigned w = 0; w != nof_dl_workers; ++w) {
        dl_cpu_masks.push_back(affinity_mng[cell_id].calcute_affinity_mask(sched_affinity_mask_types::l1_dl));
      }

      // Instantiate dedicated worker pool for the upper physical layer uplink processing such as PUCCH, PUSCH, and
      // SRS processing. This worker pool comprises three different priority queues.
      const worker_pool ul_worker_pool{name_ul,
                                       nof_ul_workers,
                                       {{concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                                        {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                                        {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}},
                                       {{l1_pucch_exec_name, task_priority::max},
                                        {l1_pusch_exec_name, task_priority::max - 1},
                                        {l1_srs_exec_name, task_priority::max - 2}},
                                       ul_worker_sleep_time,
                                       ul_worker_pool_prio,
                                       ul_cpu_masks};

      if (not exec_mng.add_execution_context(create_execution_context(ul_worker_pool))) {
        report_fatal_error("Failed to instantiate {} execution context", ul_worker_pool.name);
      }

      // Instantiate dedicated worker pool for high priority tasks such as L2, the upper physical layer downlink
      // processing, and the PRACH detector. This worker pool comprises four different priority queues where the L2 and
      // the PRACH detector queues have the highest priority.
      const worker_pool dl_worker_pool{name_dl,
                                       nof_dl_workers,
                                       {{concurrent_queue_policy::moodycamel_lockfree_mpmc, task_worker_queue_size},
                                        {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                                        {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}},
                                       {{l2_exec_name, task_priority::max},
                                        {l1_dl_exec_name, task_priority::max - 1},
                                        {l1_pdsch_exec_name, task_priority::max - 2},
                                        {l1_high_prio_name, task_priority::max - 2}},
                                       dl_worker_sleep_time,
                                       dl_worker_pool_prio,
                                       dl_cpu_masks};

      if (not exec_mng.add_execution_context(create_execution_context(dl_worker_pool))) {
        report_fatal_error("Failed to instantiate {} execution context", dl_worker_pool.name);
      }

      // Instantiate dedicated PUSCH decoder workers for each cell.
      unsigned    nof_pusch_decoder_workers = du_low.nof_pusch_decoder_threads;
      std::string pusch_decoder_exec_name   = l1_pusch_exec_name;
      if (nof_pusch_decoder_workers > 0) {
        pusch_decoder_exec_name                            = "du_low_pusch_dec_exec#" + cell_id_str;
        const std::string               name_pusch_decoder = "pusch#" + cell_id_str;
        const auto                      pusch_decoder_prio = os_thread_realtime_priority::max() - 30;
        const std::chrono::microseconds pusch_decoder_sleep_time{20};

        // As the PUSCH decoding is not time-critical, assign CPUs dedicated for low priority.
        std::vector<os_sched_affinity_bitmask> pusch_decoder_cpu_masks;
        for (unsigned w = 0; w != nof_pusch_decoder_workers; ++w) {
          pusch_decoder_cpu_masks.push_back(
              low_prio_affinity_mng.calcute_affinity_mask(sched_affinity_mask_types::low_priority));
        }

        // Instantiate dedicated worker pool for the dedicated upper physical layer PUSCH decoding. This worker pool
        // comprises a single priority queue.
        const worker_pool pusch_decoder_worker_pool{name_pusch_decoder,
                                                    nof_pusch_decoder_workers,
                                                    {{concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}},
                                                    {{pusch_decoder_exec_name}},
                                                    pusch_decoder_sleep_time,
                                                    pusch_decoder_prio,
                                                    pusch_decoder_cpu_masks};

        if (not exec_mng.add_execution_context(create_execution_context(pusch_decoder_worker_pool))) {
          report_fatal_error("Failed to instantiate {} execution context", pusch_decoder_worker_pool.name);
        }
      }

      // Fill the task executors for each cell.
      du_low_exec_mapper_config.cells.emplace_back(srs_du::du_low_executor_mapper_manual_exec_config{
          .high_priority_executor = exec_mng.executors().at(l1_high_prio_name),
          .dl_executor            = exec_mng.executors().at(l1_dl_exec_name),
          .pdsch_executor         = exec_mng.executors().at(l1_pdsch_exec_name),
          .pusch_executor         = exec_mng.executors().at(l1_pusch_exec_name),
          .pucch_executor         = exec_mng.executors().at(l1_pucch_exec_name),
          .srs_executor           = exec_mng.executors().at(l1_srs_exec_name),
          .pusch_decoder_executor = exec_mng.executors().at(pusch_decoder_exec_name)});

      // Save DL executors for the higher layers.
      crit_path_prio_executors.push_back(exec_mng.executors().at(l1_dl_exec_name));
      l2_execs.push_back(exec_mng.executors().at(l2_exec_name));
    }

    // Setup metrics configuration.
    if (du_low.metrics_period.has_value()) {
      du_low_exec_mapper_config.metrics.emplace(
          srs_du::du_low_executor_mapper_metric_config{.period              = *du_low.metrics_period,
                                                       .sequential_executor = *metrics_exec,
                                                       .logger = app_helpers::fetch_logger_metrics_log_channel()});
    }

    // Reuse DU-low DL executors for L2 critical path.
    srs_du::du_high_executor_config::strand_based_worker_pool pool_desc;
    pool_desc.nof_cells               = nof_cells;
    pool_desc.default_task_queue_size = task_worker_queue_size;
    pool_desc.pool_executors          = l2_execs;
    desc.l2_execs                     = pool_desc;
  }

  // Create DU low executor mapper.
  du_low_exec_mapper = srs_du::create_du_low_executor_mapper(du_low_exec_mapper_config);

  return desc;
}

void worker_manager::create_ofh_executors(const worker_manager_config::ru_ofh_config& config)
{
  using namespace execution_config_helper;
  const unsigned nof_cells = config.nof_downlink_antennas.size();

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
                                  {concurrent_queue_policy::locking_mpsc, 4},
                                  {{exec_name}},
                                  std::nullopt,
                                  os_thread_realtime_priority::max() - 0,
                                  ru_timing_cpu_mask};
    if (!exec_mng.add_execution_context(create_execution_context(ru_worker))) {
      report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
    }
    exec_mapper_config.timing_executor = exec_mng.executors().at(exec_name);
  }

  for (unsigned i = 0, e = nof_cells; i != e; ++i) {
    // Executor for the Open Fronthaul User and Control messages codification.
    {
      unsigned nof_ofh_dl_workers =
          (config.is_downlink_parallelized) ? std::max(config.nof_downlink_antennas[i] / 2U, 1U) : 1U;
      const std::string name      = "ru_dl_#" + std::to_string(i);
      const std::string exec_name = "ru_dl_exec_#" + std::to_string(i);

      const auto                             prio = os_thread_realtime_priority::max() - 5;
      std::vector<os_sched_affinity_bitmask> cpu_masks;
      for (unsigned w = 0; w != nof_ofh_dl_workers; ++w) {
        cpu_masks.push_back(affinity_mng[i].calcute_affinity_mask(sched_affinity_mask_types::ru));
      }
      create_worker_pool(name,
                         nof_ofh_dl_workers,
                         task_worker_queue_size,
                         {{exec_name}},
                         prio,
                         cpu_masks,
                         concurrent_queue_policy::lockfree_mpmc);
      exec_mapper_config.downlink_executors.push_back(exec_mng.executors().at(exec_name));
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
      exec_mapper_config.uplink_executors.push_back(exec_mng.executors().at(exec_name));
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
      exec_mapper_config.txrx_executors.push_back(exec_mng.executors().at(exec_name));
    }
  }

  exec_mapper_config.nof_sectors = nof_cells;

  // Create executor mapper.
  ofh_exec_mapper = create_ofh_ru_executor_mapper(exec_mapper_config);
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

        lower_prach_exec.push_back(&du_low_exec_mapper->get_cell_mapper(cell_id).prach_executor());
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

        lower_prach_exec.push_back(&du_low_exec_mapper->get_cell_mapper(cell_id).prach_executor());
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

        lower_prach_exec.push_back(&du_low_exec_mapper->get_cell_mapper(cell_id).prach_executor());
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

task_executor& worker_manager::get_du_low_dl_executor(unsigned sector_id) const
{
  srsran_assert(sector_id < crit_path_prio_executors.size(), "Invalid sector configuration");
  return *crit_path_prio_executors[sector_id];
}
