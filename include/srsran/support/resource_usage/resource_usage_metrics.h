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

#include "srsran/support/units.h"

namespace srsran {

/// CPU usage of a measured block, e.g. some processing chain executed in a UNIX thread, or the CPU usage of the entire
/// UNIX process.
struct cpu_metrics {
  float cpu_usage_percentage;
  float cpu_utilization_nof_cpus;
};

/// Memory consumption of a UNIX process.
struct memory_metrics {
  units::bytes memory_usage;
};

/// Aggregates CPU usage of a measured block and memory consumption of the UNIX process.
struct resource_usage_metrics {
  cpu_metrics    cpu_stats;
  memory_metrics memory_stats;
};

} // namespace srsran
