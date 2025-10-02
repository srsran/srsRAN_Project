/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gnb_appconfig_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "gnb_appconfig.h"

using namespace srsran;
using namespace std::chrono_literals;

void srsran::fill_gnb_worker_manager_config(worker_manager_config& config, const gnb_appconfig& app_cfg)
{
  srsran_assert(config.cu_up_cfg, "CU-UP worker config does not exist");
  srsran_assert(config.du_hi_cfg, "DU high worker config does not exist");

  config.nof_main_pool_threads     = app_cfg.expert_execution_cfg.threads.main_pool.nof_threads;
  config.main_pool_task_queue_size = app_cfg.expert_execution_cfg.threads.main_pool.task_queue_size;
  config.main_pool_backoff_period =
      std::chrono::microseconds{app_cfg.expert_execution_cfg.threads.main_pool.backoff_period};
  config.main_pool_affinity_cfg = app_cfg.expert_execution_cfg.affinities.main_pool_cpu_cfg;

  if (app_cfg.metrics_cfg.executors_metrics_cfg.enable_executor_metrics) {
    auto& exec_metrics_cfg         = config.exec_metrics_cfg.emplace();
    exec_metrics_cfg.report_period = app_cfg.metrics_cfg.executors_metrics_cfg.report_period_ms;
  }
}
