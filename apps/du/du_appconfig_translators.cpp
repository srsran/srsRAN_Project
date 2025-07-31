/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_appconfig_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "du_appconfig.h"

using namespace srsran;
using namespace std::chrono_literals;

void srsran::fill_du_worker_manager_config(worker_manager_config& config, const du_appconfig& unit_cfg)
{
  srsran_assert(config.du_hi_cfg, "DU high worker config does not exist");

  config.nof_main_pool_threads     = unit_cfg.expert_execution_cfg.threads.main_pool.nof_threads;
  config.main_pool_task_queue_size = unit_cfg.expert_execution_cfg.threads.main_pool.task_queue_size;
  config.main_pool_backoff_period =
      std::chrono::microseconds{unit_cfg.expert_execution_cfg.threads.main_pool.backoff_period};
  config.main_pool_affinity_cfg = unit_cfg.expert_execution_cfg.affinities.main_pool_cpu_cfg;
}
