/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "os_sched_affinity_manager.h"

namespace srsran {

/// CPU affinities configuration for the application.
struct cpu_affinities_appconfig {
  /// Main thread pool workers CPU affinity mask.
  os_sched_affinity_config main_pool_cpu_cfg = {sched_affinity_mask_types::main, {}, sched_affinity_mask_policy::mask};
};

/// Main thread configuration for the application.
struct main_thread_pool_appconfig {
  /// Number of threads for processing of upper phy, DU-high, CU-CP and CU-UP.
  std::optional<unsigned> nof_threads;
  /// Task worker pool queue size.
  unsigned task_queue_size = 2048;
  /// Thread back-off period, in microseconds, when the task queue is empty.
  unsigned backoff_period = 50;
};

/// Expert threads configuration of the application.
struct expert_threads_appconfig {
  /// Main thread pool configuration of the application.
  main_thread_pool_appconfig main_pool;
};

/// Expert configuration of the application.
struct expert_execution_appconfig {
  /// Application CPU affinities.
  cpu_affinities_appconfig affinities;
  /// Expert thread configuration of the application.
  expert_threads_appconfig threads;
};

} // namespace srsran
