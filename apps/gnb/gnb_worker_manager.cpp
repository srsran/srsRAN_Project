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
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/support/executors/sync_task_executor.h"

using namespace srsran;

worker_manager::worker_manager(const gnb_appconfig& appcfg)
{
  optional<lower_phy_thread_profile> lower_phy_profile;
  std::string                        driver = "";
  if (variant_holds_alternative<ru_sdr_appconfig>(appcfg.ru_cfg)) {
    const ru_sdr_appconfig& sdr_cfg = variant_get<ru_sdr_appconfig>(appcfg.ru_cfg);
    driver                          = sdr_cfg.device_driver;

    lower_phy_profile =
        (driver != "zmq") ? sdr_cfg.expert_cfg.lphy_executor_profile : lower_phy_thread_profile::blocking;
  }

  if (appcfg.expert_phy_cfg.nof_pdsch_threads > 1) {
    create_worker_pool(
        "pdsch", appcfg.expert_phy_cfg.nof_pdsch_threads, 2 * MAX_CBS_PER_PDU, os_thread_realtime_priority::max() - 10);
  }

  create_executors(driver == "zmq", lower_phy_profile, appcfg.expert_phy_cfg.nof_ul_threads);
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
}

template <typename... Args>
void worker_manager::create_worker(const std::string& name, Args&&... args)
{
  auto ret = workers.insert(std::make_pair(name, std::make_unique<task_worker>(name, std::forward<Args>(args)...)));
  srsran_assert(ret.second, "Unable to create worker {}.", name);
}

void worker_manager::create_worker_pool(const std::string&          name,
                                        size_t                      nof_workers,
                                        size_t                      queue_size,
                                        os_thread_realtime_priority prio)
{
  auto ret = worker_pools.insert(
      std::make_pair(name, std::make_unique<task_worker_pool>(nof_workers, queue_size, name, prio)));
  srsran_assert(ret.second, "Unable to create worker pool {}.", name);
}

void worker_manager::create_executors(bool                               blocking_mode_active,
                                      optional<lower_phy_thread_profile> lower_phy_profile,
                                      unsigned                           nof_ul_workers)
{
  static const uint32_t task_worker_queue_size = 2048;

  // Instantiate workers
  create_worker("gnb_ue", 512);
  gnb_ctrl_worker = std::make_unique<du_cell_worker_type>("gnb_ctrl",
                                                          std::array<unsigned, 2>{64, task_worker_queue_size},
                                                          std::chrono::microseconds{100},
                                                          os_thread_realtime_priority::max() - 2,
                                                          os_sched_affinity_bitmask{});
  du_cell_worker  = std::make_unique<du_cell_worker_type>("du_cell",
                                                         std::array<unsigned, 2>{8, task_worker_queue_size},
                                                         std::chrono::microseconds{10},
                                                         os_thread_realtime_priority::max() - 2,
                                                         os_sched_affinity_bitmask{});

  if (blocking_mode_active) {
    create_worker("phy_worker", task_worker_queue_size, os_thread_realtime_priority::max());
  } else {
    create_worker("phy_prach", task_worker_queue_size, os_thread_realtime_priority::max() - 2);
    create_worker("upper_phy_dl", task_worker_queue_size, os_thread_realtime_priority::max() - 10);
    create_worker_pool("upper_phy_ul", nof_ul_workers, task_worker_queue_size, os_thread_realtime_priority::max() - 20);
  }

  if (!blocking_mode_active && lower_phy_profile && *lower_phy_profile == lower_phy_thread_profile::blocking) {
    create_worker("phy_worker", task_worker_queue_size, os_thread_realtime_priority::max());
  }

  create_worker("radio", task_worker_queue_size);
  create_worker("ru_stats_worker", 1);

  // Instantiate task executors
  cu_cp_exec      = make_priority_task_executor_ptr<task_queue_priority::min>(*gnb_ctrl_worker);
  cu_up_exec      = std::make_unique<task_worker_executor>(*workers.at("gnb_ue"));
  gtpu_pdu_exec   = std::make_unique<task_worker_executor>(*workers.at("gnb_ue"), false);
  du_ctrl_exec    = make_priority_task_executor_ptr<task_queue_priority::min>(*gnb_ctrl_worker);
  du_timer_exec   = make_priority_task_executor_ptr<task_queue_priority::max>(*gnb_ctrl_worker);
  du_ue_exec      = std::make_unique<task_worker_executor>(*workers.at("gnb_ue"));
  du_cell_exec    = make_priority_task_executor_ptr<task_queue_priority::min>(*du_cell_worker);
  ru_printer_exec = std::make_unique<task_worker_executor>(*workers.at("ru_stats_worker"));
  if (blocking_mode_active) {
    du_slot_exec = make_sync_executor(make_priority_task_worker_executor<task_queue_priority::max>(*du_cell_worker));
    task_worker& phy_worker = *workers.at("phy_worker");
    lower_phy_tx_exec       = std::make_unique<task_worker_executor>(phy_worker);
    lower_phy_rx_exec       = std::make_unique<task_worker_executor>(phy_worker);
    lower_phy_dl_exec       = std::make_unique<task_worker_executor>(phy_worker);
    lower_phy_ul_exec       = std::make_unique<task_worker_executor>(phy_worker);
    lower_prach_exec        = std::make_unique<task_worker_executor>(phy_worker);
    upper_dl_exec           = std::make_unique<task_worker_executor>(phy_worker);
    upper_pusch_exec        = std::make_unique<task_worker_executor>(phy_worker);
    upper_pucch_exec        = std::make_unique<task_worker_executor>(phy_worker);
    upper_prach_exec        = std::make_unique<task_worker_executor>(phy_worker);
  } else {
    du_slot_exec     = make_priority_task_executor_ptr<task_queue_priority::max>(*du_cell_worker);
    lower_prach_exec = std::make_unique<task_worker_executor>(*workers.at("phy_prach"));
    upper_dl_exec    = std::make_unique<task_worker_executor>(*workers.at("upper_phy_dl"));
    upper_pusch_exec = std::make_unique<task_worker_pool_executor>(*worker_pools.at("upper_phy_ul"));
    upper_pucch_exec = std::make_unique<task_worker_pool_executor>(*worker_pools.at("upper_phy_ul"));
    upper_prach_exec = std::make_unique<task_worker_executor>(*workers.at("phy_prach"));
  }

  if (worker_pools.count("pdsch")) {
    upper_pdsch_exec = std::make_unique<task_worker_pool_executor>(*worker_pools.at("pdsch"));
  }

  if (lower_phy_profile) {
    switch (*lower_phy_profile) {
      case lower_phy_thread_profile::blocking: {
        fmt::print("Lower PHY in executor blocking mode.\n");
        task_worker& phy_worker = *workers.at("phy_worker");
        lower_phy_tx_exec       = std::make_unique<task_worker_executor>(phy_worker);
        lower_phy_rx_exec       = std::make_unique<task_worker_executor>(phy_worker);
        lower_phy_dl_exec       = std::make_unique<task_worker_executor>(phy_worker);
        lower_phy_ul_exec       = std::make_unique<task_worker_executor>(phy_worker);
        break;
      }
      case lower_phy_thread_profile::single: {
        fmt::print("Lower PHY in single executor mode.\n");
        create_worker("lower_phy", 128, os_thread_realtime_priority::max());
        task_worker& lower_phy_worker = *workers.at("lower_phy");
        lower_phy_tx_exec             = std::make_unique<task_worker_executor>(lower_phy_worker);
        lower_phy_rx_exec             = std::make_unique<task_worker_executor>(lower_phy_worker);
        lower_phy_dl_exec             = std::make_unique<task_worker_executor>(lower_phy_worker);
        lower_phy_ul_exec             = std::make_unique<task_worker_executor>(lower_phy_worker);
        break;
      }
      case lower_phy_thread_profile::dual: {
        fmt::print("Lower PHY in dual executor mode.\n");
        create_worker("lower_phy_dl", 128, os_thread_realtime_priority::max());
        create_worker("lower_phy_ul", 2, os_thread_realtime_priority::max() - 1);
        lower_phy_tx_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_dl"));
        lower_phy_rx_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_ul"));
        lower_phy_dl_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_dl"));
        lower_phy_ul_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_ul"));
        break;
      }
      case lower_phy_thread_profile::quad: {
        fmt::print("Lower PHY in quad executor mode.\n");
        create_worker("lower_phy_tx", 128, os_thread_realtime_priority::max());
        create_worker("lower_phy_rx", 1, os_thread_realtime_priority::max() - 2);
        create_worker("lower_phy_dl", 128, os_thread_realtime_priority::max() - 1);
        create_worker("lower_phy_ul", 128, os_thread_realtime_priority::max() - 3);
        lower_phy_tx_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_tx"));
        lower_phy_rx_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_rx"));
        lower_phy_dl_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_dl"));
        lower_phy_ul_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_ul"));
        break;
      }
    }
  }

  radio_exec = std::make_unique<task_worker_executor>(*workers.at("radio"));

  // RU executors.
  create_worker("ru_timing", 1, os_thread_realtime_priority::max() - 1);
  ru_timing_exec = std::make_unique<task_worker_executor>(*workers.at("ru_timing"));
  create_worker("ru_time_not", task_worker_queue_size, os_thread_realtime_priority::max() - 2);
  ru_timing_notifier_exec = std::make_unique<task_worker_executor>(*workers.at("ru_time_not"));
  create_worker("ru_tx",
                128,
                os_thread_realtime_priority::max() - 3,
                os_sched_affinity_bitmask(std::min(srsran::compute_host_nof_hardware_threads() - 1, size_t(1U))));
  ru_tx_exec = std::make_unique<task_worker_executor>(*workers.at("ru_tx"));
  create_worker("ru_rx",
                1,
                os_thread_realtime_priority::max() - 4,
                os_sched_affinity_bitmask(std::min(srsran::compute_host_nof_hardware_threads() - 1, size_t(3U))));
  ru_rx_exec = std::make_unique<task_worker_executor>(*workers.at("ru_rx"));

  // Executor mappers.
  du_high_exec_mapper = std::make_unique<du_high_executor_mapper_impl>(
      std::make_unique<cell_executor_mapper>(std::initializer_list<task_executor*>{du_cell_exec.get()},
                                             std::initializer_list<task_executor*>{du_slot_exec.get()}),
      std::make_unique<pcell_ue_executor_mapper>(std::initializer_list<task_executor*>{du_ue_exec.get()}),
      *du_ctrl_exec,
      *du_timer_exec);
}
