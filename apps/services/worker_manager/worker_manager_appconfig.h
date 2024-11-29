/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
  /// CPUs isolation.
  std::optional<os_sched_affinity_bitmask> isolated_cpus;
  /// Low priority workers CPU affinity mask.
  os_sched_affinity_config low_priority_cpu_cfg = {sched_affinity_mask_types::low_priority,
                                                   {},
                                                   sched_affinity_mask_policy::mask};
};

/// Non real time thread configuration for the application.
struct non_rt_threads_appconfig {
  /// Number of non real time threads for processing of CP and UP data in the upper layers
  unsigned nof_non_rt_threads = 4;
  /// Non real time task worker queue size
  unsigned non_rt_task_queue_size = 2048;
};

/// Expert threads configuration of the application.
struct expert_threads_appconfig {
  /// Non real time thread configuration of the application.
  non_rt_threads_appconfig non_rt_threads;
};

/// Expert configuration of the application.
struct expert_execution_appconfig {
  /// Application CPU affinities.
  cpu_affinities_appconfig affinities;
  /// Expert thread configuration of the application.
  expert_threads_appconfig threads;
};

} // namespace srsran
