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
  double         power_usage_watts;
};

} // namespace srsran
