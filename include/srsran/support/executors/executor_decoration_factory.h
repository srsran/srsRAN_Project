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

#include "srsran/srslog/logger.h"
#include "srsran/support/executors/detail/task_executor_utils.h"
#include "srsran/support/tracing/event_tracing.h"
#include <optional>
#include <string>

namespace srsran {
/// Description of the decorators to be applied to an executor.
struct execution_decoration_config {
  struct sync_option {};
  struct trace_option {
    std::string              name;
    file_event_tracer<true>* tracer;
  };
  struct metrics_option {
    std::string               name;
    srslog::basic_logger&     logger;
    std::chrono::milliseconds period;
  };

  /// If set, the executor will block the caller until the task is executed.
  std::optional<sync_option> sync;
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
