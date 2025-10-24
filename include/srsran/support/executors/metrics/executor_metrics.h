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

#include "srsran/support/tracing/resource_usage.h"
#include <chrono>
#include <string>

namespace srsran {

/// Task executor metrics.
struct executor_metrics {
  /// Executor name.
  std::string name;
  /// Number of times executor ran a task.
  unsigned nof_executes;
  /// Number of times a task execution was deferred.
  unsigned nof_defers;
  /// Average task enqueueing latency in microseconds.
  std::chrono::microseconds avg_enqueue_latency_us;
  /// Maximum task enqueueing latency in microseconds.
  std::chrono::microseconds max_enqueue_latency_us;
  /// Average task duration in microseconds.
  std::chrono::microseconds avg_task_us;
  /// Maximum task duration in microseconds.
  std::chrono::microseconds max_task_us;
  /// Total rusage accumulated during the report period.
  resource_usage::diff total_rusg;
  /// CPU utilization.
  float cpu_load;
};

} // namespace srsran
