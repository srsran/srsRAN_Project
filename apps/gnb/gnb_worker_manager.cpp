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

  create_du_cu_executors(is_blocking_mode_active,
                         appcfg.expert_phy_cfg.nof_ul_threads,
                         appcfg.expert_phy_cfg.nof_dl_threads,
                         appcfg.expert_phy_cfg.nof_pdsch_threads,
                         appcfg.cells_cfg,
                         appcfg.expert_phy_cfg.max_processing_delay_slots);

  create_ru_executors(appcfg);
}

void worker_manager::stop()
{
  du_cell_worker->stop();
  gnb_ctrl_worker->stop();
  for (auto& worker : workers) {
    worker.second->stop();
  }
  for (auto& pool : worker_pools) {
    pool.second->stop();
  }
  for (auto& worker : ru_spsc_workers) {
    worker->stop();
  }
  for (auto& worker : ru_mpsc_workers) {
    worker->stop();
  }
}

template <typename... Args>
void worker_manager::create_worker(const std::string& name, Args&&... args)
{
  auto ret = workers.insert(std::make_pair(name, std::make_unique<task_worker>(name, std::forward<Args>(args)...)));
  srsran_assert(ret.second, "Unable to create worker {}.", name);
}

void worker_manager::create_worker_pool(const std::string&                    name,
                                        size_t                                nof_workers,
                                        size_t                                queue_size,
                                        os_thread_realtime_priority           prio,
                                        span<const os_sched_affinity_bitmask> cpu_masks)
{
  auto ret = worker_pools.insert(
      std::make_pair(name, std::make_unique<task_worker_pool>(nof_workers, queue_size, name, prio, cpu_masks)));
  srsran_assert(ret.second, "Unable to create worker pool {}.", name);
}

void worker_manager::create_prio_worker(const std::string& name, size_t queue_size, os_thread_realtime_priority prio)
{
  os_sched_affinity_bitmask cpu_mask;
  if (use_tuned_profile) {
    cpu_mask = affinity_manager->reserve_cpu(name, prio);
  }
  create_worker(name, queue_size, prio, cpu_mask);
}

void worker_manager::create_du_cu_executors(bool                       is_blocking_mode_active,
                                            unsigned                   nof_ul_workers,
                                            unsigned                   nof_dl_workers,
                                            unsigned                   nof_pdsch_workers,
                                            span<const cell_appconfig> cells_cfg,
                                            unsigned                   pipeline_depth)
{
  // Instantiate workers
  create_prio_worker("gnb_ue", 512);
  os_sched_affinity_bitmask cpu_mask;
  if (use_tuned_profile) {
    cpu_mask = affinity_manager->reserve_cpu("gnb_ctrl", os_thread_realtime_priority::max() - 20);
  }
  gnb_ctrl_worker = std::make_unique<du_cell_worker_type>("gnb_ctrl",
                                                          std::array<unsigned, 2>{64, task_worker_queue_size},
                                                          std::chrono::microseconds{200},
                                                          os_thread_realtime_priority::max() - 20,
                                                          cpu_mask);
  if (use_tuned_profile) {
    cpu_mask = affinity_manager->reserve_cpu("du_cell", os_thread_realtime_priority::max() - 2);
  }
  du_cell_worker = std::make_unique<du_cell_worker_type>("du_cell",
                                                         std::array<unsigned, 2>{8, task_worker_queue_size},
                                                         std::chrono::microseconds{10},
                                                         os_thread_realtime_priority::max() - 2,
                                                         cpu_mask);

  // Instantiate task executors
  cu_cp_exec    = make_priority_task_executor_ptr<task_queue_priority::min>(*gnb_ctrl_worker);
  cu_up_exec    = std::make_unique<task_worker_executor>(*workers.at("gnb_ue"));
  gtpu_pdu_exec = std::make_unique<task_worker_executor>(*workers.at("gnb_ue"), false);

  // Create E2 executors
  cu_cp_e2_exec = make_priority_task_executor_ptr<task_queue_priority::min>(*gnb_ctrl_worker);
  cu_up_e2_exec = make_priority_task_executor_ptr<task_queue_priority::min>(*gnb_ctrl_worker);

  // metrics hub executor
  metrics_hub_exec = make_priority_task_executor_ptr<task_queue_priority::min>(*gnb_ctrl_worker);

  du_high_executors.resize(cells_cfg.size());
  for (unsigned i = 0, e = cells_cfg.size(); i != e; ++i) {
    auto& du_item = du_high_executors[i];

    // Create Ctrl executors. The timer has higher priority.
    du_item.du_ctrl_exec  = make_priority_task_executor_ptr<task_queue_priority::min>(*gnb_ctrl_worker);
    du_item.du_timer_exec = make_priority_task_executor_ptr<task_queue_priority::max>(*gnb_ctrl_worker);

    // Create UE executors.
    du_item.du_ue_exec = std::make_unique<task_worker_executor>(*workers.at("gnb_ue"));

    // Create Cell executors.
    du_item.du_cell_exec = make_priority_task_executor_ptr<task_queue_priority::min>(*du_cell_worker);
    du_item.du_slot_exec =
        is_blocking_mode_active
            ? make_sync_executor(make_priority_task_worker_executor<task_queue_priority::max>(*du_cell_worker))
            : make_priority_task_executor_ptr<task_queue_priority::max>(*du_cell_worker);

    // Create E2 executors
    du_item.du_e2_exec = make_priority_task_executor_ptr<task_queue_priority::min>(*gnb_ctrl_worker);

    // DU-high executor mapper.
    const std::initializer_list<task_executor*> cell_execs{du_item.du_cell_exec.get()};
    const std::initializer_list<task_executor*> slot_execs{du_item.du_slot_exec.get()};
    auto cell_exec_mapper = std::make_unique<cell_executor_mapper>(cell_execs, slot_execs);
    auto ue_exec_mapper =
        std::make_unique<pcell_ue_executor_mapper>(std::initializer_list<task_executor*>{du_item.du_ue_exec.get()});
    du_item.du_high_exec_mapper = std::make_unique<du_high_executor_mapper_impl>(std::move(cell_exec_mapper),
                                                                                 std::move(ue_exec_mapper),
                                                                                 *du_item.du_ctrl_exec,
                                                                                 *du_item.du_timer_exec,
                                                                                 *du_item.du_e2_exec);
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
  du_low_dl_executors.resize(cells_cfg.size());
  upper_pdsch_exec.resize(cells_cfg.size());
  for (unsigned cell_id = 0, cell_end = cells_cfg.size(); cell_id != cell_end; ++cell_id) {
    if (is_blocking_mode_active) {
      if (cell_id == 0) {
        create_prio_worker("phy_worker", task_worker_queue_size, os_thread_realtime_priority::max());
      }
      task_worker& phy_worker = *workers.at("phy_worker");
      upper_pusch_exec.push_back(std::make_unique<task_worker_executor>(phy_worker));
      upper_pucch_exec.push_back(std::make_unique<task_worker_executor>(phy_worker));
      upper_prach_exec.push_back(std::make_unique<task_worker_executor>(phy_worker));
      du_low_dl_executors[cell_id].emplace_back(std::make_unique<task_worker_executor>(phy_worker));
    } else {
      const std::string&                     name_ul = "up_phy_ul#" + std::to_string(cell_id);
      const auto                             prio    = os_thread_realtime_priority::max() - 20;
      std::vector<os_sched_affinity_bitmask> cpu_masks;
      if (use_tuned_profile) {
        for (unsigned w = 0; w != nof_ul_workers; ++w) {
          cpu_masks.push_back(affinity_manager->reserve_cpu(name_ul, prio));
        }
      }
      create_worker_pool(name_ul, nof_ul_workers, task_worker_queue_size, prio, cpu_masks);
      upper_pusch_exec.push_back(std::make_unique<task_worker_pool_executor>(*worker_pools.at(name_ul)));
      upper_pucch_exec.push_back(std::make_unique<task_worker_pool_executor>(*worker_pools.at(name_ul)));

      const std::string& name_prach = "phy_prach#" + std::to_string(cell_id);
      create_prio_worker(name_prach, task_worker_queue_size, os_thread_realtime_priority::max() - 2);
      upper_prach_exec.push_back(std::make_unique<task_worker_executor>(*workers.at(name_prach)));
      for (unsigned i_dl_worker = 0; i_dl_worker != nof_dl_workers; ++i_dl_worker) {
        // Create upper PHY DL executors.
        std::string worker_name = "up_phy_dl#" + std::to_string(cell_id) + "#" + std::to_string(i_dl_worker);
        create_prio_worker(worker_name, task_worker_queue_size, os_thread_realtime_priority::max() - 10);
        du_low_dl_executors[cell_id].emplace_back(std::make_unique<task_worker_executor>(*workers.at(worker_name)));
      }
    }

    if (nof_pdsch_workers > 1) {
      const std::string& name_pdsch = "pdsch#" + std::to_string(cell_id);
      unsigned           max_nof_pdsch_cb_slot =
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
      create_worker_pool(name_pdsch, nof_pdsch_workers, max_nof_pdsch_cb_slot, prio, cpu_masks);

      upper_pdsch_exec[cell_id] = std::make_unique<task_worker_pool_executor>(*worker_pools.at(name_pdsch));
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
    const std::string& name = "ru_timing";
    ru_spsc_workers.push_back(std::make_unique<ru_spsc_worker_type>("ru_timing",
                                                                    4,
                                                                    std::chrono::microseconds{0},
                                                                    os_thread_realtime_priority::max() - 0,
                                                                    get_affinity_mask(*affinity_manager, name, 0)));
    ru_timing_exec = std::make_unique<
        general_task_worker_executor<concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>>(
        *ru_spsc_workers.back());
  }

  for (unsigned i = 0, e = cells.size(); i != e; ++i) {
    ru_dl_exec.emplace_back();
    // Executor for the Open Fronthaul User and Control messages codification.
    unsigned dl_end = (is_downlink_parallelized) ? std::max(cells[i].cell.nof_antennas_dl / 2U, 1U) : 1U;
    for (unsigned dl_id = 0; dl_id != dl_end; ++dl_id) {
      const std::string& name = "ru_dl_#" + std::to_string(i) + "#" + std::to_string(dl_id);
      ru_mpsc_workers.push_back(std::make_unique<ru_mpsc_worker_type>(name,
                                                                      task_worker_queue_size,
                                                                      os_thread_realtime_priority::max() - 5,
                                                                      get_affinity_mask(*affinity_manager, name, 5)));
      ru_dl_exec[i].push_back(make_task_executor(*ru_mpsc_workers.back()));
    }

    // Executor for Open Fronthaul messages transmission.
    {
      const std::string& name = "ru_tx_" + std::to_string(i);
      ru_spsc_workers.push_back(std::make_unique<ru_spsc_worker_type>(name,
                                                                      task_worker_queue_size,
                                                                      std::chrono::microseconds{1},
                                                                      os_thread_realtime_priority::max() - 1,
                                                                      get_affinity_mask(*affinity_manager, name, 1)));
      ru_tx_exec.push_back(make_task_executor(*ru_spsc_workers.back()));
    }

    // Executor for Open Fronthaul messages reception and decodification.
    {
      const std::string& name = "ru_rx_" + std::to_string(i);
      ru_spsc_workers.push_back(std::make_unique<ru_spsc_worker_type>(name,
                                                                      2,
                                                                      std::chrono::microseconds{1},
                                                                      os_thread_realtime_priority::max() - 1,
                                                                      get_affinity_mask(*affinity_manager, name, 1)));
      ru_rx_exec.push_back(make_task_executor(*ru_spsc_workers.back()));
    }
  }
}

void worker_manager::create_lower_phy_executors(lower_phy_thread_profile lower_phy_profile, unsigned nof_cells)
{
  // Radio Unit worker and executor.
  create_prio_worker("radio", task_worker_queue_size);
  radio_exec = std::make_unique<task_worker_executor>(*workers.at("radio"));

  // Radio Unit statistics worker and executor.
  create_prio_worker("ru_stats_worker", 1);
  ru_printer_exec = std::make_unique<task_worker_executor>(*workers.at("ru_stats_worker"));

  for (unsigned cell_id = 0; cell_id != nof_cells; ++cell_id) {
    switch (lower_phy_profile) {
      case lower_phy_thread_profile::blocking: {
        std::string name = "phy_worker";
        if (nof_cells > 1) {
          name = "lower_phy#" + std::to_string(cell_id);
          create_worker(name, 128, os_thread_realtime_priority::max());
        } else {
          fmt::print("Lower PHY in executor blocking mode.\n");
        }
        task_worker& lower_phy_worker = *workers.at(name);
        lower_prach_exec.push_back(std::make_unique<task_worker_executor>(lower_phy_worker));
        lower_phy_tx_exec.push_back(std::make_unique<task_worker_executor>(lower_phy_worker));
        lower_phy_rx_exec.push_back(std::make_unique<task_worker_executor>(lower_phy_worker));
        lower_phy_dl_exec.push_back(std::make_unique<task_worker_executor>(lower_phy_worker));
        lower_phy_ul_exec.push_back(std::make_unique<task_worker_executor>(lower_phy_worker));
        break;
      }
      case lower_phy_thread_profile::single: {
        fmt::print("Lower PHY in single executor mode.\n");
        const std::string& name = "lower_phy#" + std::to_string(cell_id);
        create_prio_worker(name, 128, os_thread_realtime_priority::max());
        task_worker& lower_phy_worker = *workers.at(name);
        lower_phy_tx_exec.push_back(std::make_unique<task_worker_executor>(lower_phy_worker));
        lower_phy_rx_exec.push_back(std::make_unique<task_worker_executor>(lower_phy_worker));
        lower_phy_dl_exec.push_back(std::make_unique<task_worker_executor>(lower_phy_worker));
        lower_phy_ul_exec.push_back(std::make_unique<task_worker_executor>(lower_phy_worker));

        const std::string& name_prach = "phy_prach#" + std::to_string(cell_id);
        lower_prach_exec.push_back(std::make_unique<task_worker_executor>(*workers.at(name_prach)));
        break;
      }
      case lower_phy_thread_profile::dual: {
        fmt::print("Lower PHY in dual executor mode.\n");
        const std::string& name_dl = "lower_phy_dl#" + std::to_string(cell_id);
        const std::string& name_ul = "lower_phy_ul#" + std::to_string(cell_id);
        create_prio_worker(name_dl, 128, os_thread_realtime_priority::max());
        create_prio_worker(name_ul, 2, os_thread_realtime_priority::max() - 1);
        lower_phy_tx_exec.push_back(std::make_unique<task_worker_executor>(*workers.at(name_dl)));
        lower_phy_rx_exec.push_back(std::make_unique<task_worker_executor>(*workers.at(name_ul)));
        lower_phy_dl_exec.push_back(std::make_unique<task_worker_executor>(*workers.at(name_dl)));
        lower_phy_ul_exec.push_back(std::make_unique<task_worker_executor>(*workers.at(name_ul)));

        const std::string& name_prach = "phy_prach#" + std::to_string(cell_id);
        lower_prach_exec.push_back(std::make_unique<task_worker_executor>(*workers.at(name_prach)));
        break;
      }
      case lower_phy_thread_profile::quad: {
        fmt::print("Lower PHY in quad executor mode.\n");
        const std::string& name_dl = "lower_phy_dl#" + std::to_string(cell_id);
        const std::string& name_ul = "lower_phy_ul#" + std::to_string(cell_id);
        const std::string& name_tx = "lower_phy_tx#" + std::to_string(cell_id);
        const std::string& name_rx = "lower_phy_rx#" + std::to_string(cell_id);
        create_prio_worker(name_tx, 128, os_thread_realtime_priority::max());
        create_prio_worker(name_rx, 1, os_thread_realtime_priority::max() - 2);
        create_prio_worker(name_dl, 128, os_thread_realtime_priority::max() - 1);
        create_prio_worker(name_ul, 128, os_thread_realtime_priority::max() - 3);
        lower_phy_tx_exec.push_back(std::make_unique<task_worker_executor>(*workers.at(name_tx)));
        lower_phy_rx_exec.push_back(std::make_unique<task_worker_executor>(*workers.at(name_rx)));
        lower_phy_dl_exec.push_back(std::make_unique<task_worker_executor>(*workers.at(name_dl)));
        lower_phy_ul_exec.push_back(std::make_unique<task_worker_executor>(*workers.at(name_ul)));

        const std::string& name_prach = "phy_prach#" + std::to_string(cell_id);
        lower_prach_exec.push_back(std::make_unique<task_worker_executor>(*workers.at(name_prach)));
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
    executors[i_exec] = du_low_exec[i_exec].get();
  }
}
