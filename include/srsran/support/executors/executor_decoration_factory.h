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

#include "srsran/support/executors/detail/task_executor_utils.h"
#include <chrono>
#include <memory>
#include <optional>
#include <string>

namespace srsran {

/// Description of the decorators to be applied to an executor.
struct execution_decoration_config {
  struct sync_option {};
  struct throttle_option {
    /// Number of tasks pending after which the caller to the executor starts being throttled.
    unsigned nof_task_threshold;
  };
  struct trace_option {
    /// Name of the executor to be traced.
    std::string name;
  };
  struct metrics_option {
    /// Name of the executor for which metrics are to be reported.
    std::string name;
    /// Period at which the metrics are collected.
    std::chrono::milliseconds period;
  };

  /// If set, the executor will block the caller until the task is executed.
  std::optional<sync_option> sync;
  /// \brief If set, the executor will throttle the execute/defer caller if the number of pending tasks exceeds the
  /// specified threshold.
  std::optional<throttle_option> throttle;
  /// \brief If set, the executor will collect metrics on the task execution latencies.
  /// \remark This decorator should be only used with sequential executors (e.g. strands, single threads).
  std::optional<metrics_option> metrics;
  /// If set, the executor will trace the task execution latencies using the specified tracer.
  std::optional<trace_option> trace;
};

/// \brief Creates an executor decorator that applies the specified policies to the given executor.
std::unique_ptr<task_executor> decorate_executor(std::unique_ptr<task_executor>     exec,
                                                 const execution_decoration_config& config);
std::unique_ptr<task_executor> decorate_executor(task_executor& exec, const execution_decoration_config& config);

} // namespace srsran
