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

#include "srsran/support/executors/executor_decoration_factory.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/executor_throttler.h"
#include "srsran/support/executors/executor_tracer.h"
#include "srsran/support/executors/metrics/executor_metrics_channel_registry.h"
#include "srsran/support/executors/metrics/executor_metrics_decorator.h"
#include "srsran/support/executors/sync_task_executor.h"
#include "srsran/support/tracing/event_tracing.h"

using namespace srsran;

static file_event_tracer<true> tracer;

/// Helper function that converts an executor of type T to a unique_ptr<task_executor>.
template <typename T>
static std::unique_ptr<task_executor> convert_to_unique_executor_ptr(T&& exec)
{
  if constexpr (std::is_same_v<std::decay_t<T>, std::unique_ptr<task_executor>>) {
    // If T is already a unique task_executor, we can return it as a unique_ptr.
    return std::move(exec);
  } else if constexpr (std::is_same_v<T, task_executor&>) {
    struct wrapper final : public task_executor {
      task_executor& exec;

      wrapper(task_executor& e) : exec(e) {}

      [[nodiscard]] bool execute(unique_task task) override { return exec.execute(std::move(task)); }

      [[nodiscard]] bool defer(unique_task task) override { return exec.defer(std::move(task)); }
    };

    return std::make_unique<wrapper>(exec);
  } else {
    return std::make_unique<std::decay_t<T>>(std::forward<T>(exec));
  }
}

template <typename ComposedExecutor>
static void make_executor_decorator_helper(std::unique_ptr<task_executor>& result, ComposedExecutor&& exec)
{
  result = convert_to_unique_executor_ptr(std::forward<ComposedExecutor>(exec));
}

template <typename ComposedExecutor, typename FirstPolicy, typename... Policies>
static void make_executor_decorator_helper(std::unique_ptr<task_executor>&   result,
                                           ComposedExecutor&&                exec,
                                           const std::optional<FirstPolicy>& first_policy,
                                           const Policies&... policies)
{
  if (not first_policy.has_value()) {
    // Skip this policy.
    make_executor_decorator_helper(result, std::forward<ComposedExecutor>(exec), policies...);
    return;
  }

  using Decoration = std::decay_t<FirstPolicy>;
  if constexpr (std::is_same_v<Decoration, execution_decoration_config::sync_option>) {
    make_executor_decorator_helper(
        result, sync_task_executor<ComposedExecutor>(std::forward<ComposedExecutor>(exec)), policies...);
  } else if constexpr (std::is_same_v<Decoration, execution_decoration_config::throttle_option>) {
    make_executor_decorator_helper(
        result,
        executor_throttler<ComposedExecutor>(std::forward<ComposedExecutor>(exec), first_policy->nof_task_threshold),
        policies...);
  } else if constexpr (std::is_same_v<Decoration, execution_decoration_config::metrics_option>) {
    if (first_policy->tracing_enabled) {
      make_executor_decorator_helper(result,
                                     executor_metrics_decorator<ComposedExecutor, file_event_tracer<true>>(
                                         first_policy->name,
                                         std::forward<ComposedExecutor>(exec),
                                         first_policy->channel_registry.add_channel(first_policy->name),
                                         first_policy->tracer ? first_policy->tracer : &tracer),
                                     policies...);
    } else {
      make_executor_decorator_helper(
          result,
          executor_metrics_decorator<ComposedExecutor>(first_policy->name,
                                                       std::forward<ComposedExecutor>(exec),
                                                       first_policy->channel_registry.add_channel(first_policy->name)),
          policies...);
    }
  } else if constexpr (std::is_same_v<Decoration, execution_decoration_config::trace_option>) {
    make_executor_decorator_helper(result,
                                   executor_tracer<ComposedExecutor, file_event_tracer<true>>{
                                       std::forward<ComposedExecutor>(exec), tracer, first_policy->name},
                                   policies...);
  } else {
    report_fatal_error("Unsupported executor decoration policy in make_executor_decorator_helper");
  }
}

static void validate_config(const execution_decoration_config& config)
{
  if (config.metrics.has_value() and config.trace.has_value()) {
    report_fatal_error("Metrics and tracing cannot be enabled at the same time.");
  }
  if (config.sync.has_value() and config.throttle.has_value()) {
    report_fatal_error("Synchronous executors cannot be throttled.");
  }
}

std::unique_ptr<task_executor> srsran::decorate_executor(std::unique_ptr<task_executor>     exec,
                                                         const execution_decoration_config& config)
{
  validate_config(config);
  std::unique_ptr<task_executor> result;
  make_executor_decorator_helper(result, std::move(exec), config.sync, config.throttle, config.metrics, config.trace);
  return result;
}

std::unique_ptr<task_executor> srsran::decorate_executor(task_executor& exec, const execution_decoration_config& config)
{
  validate_config(config);
  std::unique_ptr<task_executor> result;
  make_executor_decorator_helper(result, exec, config.sync, config.throttle, config.metrics, config.trace);
  return result;
}
