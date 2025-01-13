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
