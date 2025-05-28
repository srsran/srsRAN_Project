/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/executor_decoration_factory.h"
#include "srsran/support/executors/executor_tracer.h"
#include "srsran/support/executors/sequential_metrics_executor.h"
#include "srsran/support/executors/sync_task_executor.h"

using namespace srsran;

namespace {

template <typename T>
std::unique_ptr<task_executor> convert_to_unique_executor_ptr(T&& exec)
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
void make_executor_decorator_helper(std::unique_ptr<task_executor>& result, ComposedExecutor&& exec)
{
  result = convert_to_unique_executor_ptr(std::forward<ComposedExecutor>(exec));
}

template <typename ComposedExecutor, typename FirstPolicy, typename... Policies>
void make_executor_decorator_helper(std::unique_ptr<task_executor>&   result,
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
  } else if constexpr (std::is_same_v<Decoration, execution_decoration_config::metrics_option>) {
    make_executor_decorator_helper(
        result,
        sequential_metrics_executor<ComposedExecutor, srslog::basic_logger&>(
            first_policy->name, std::forward<ComposedExecutor>(exec), first_policy->logger, first_policy->period),
        policies...);
  } else if constexpr (std::is_same_v<Decoration, execution_decoration_config::trace_option>) {
    make_executor_decorator_helper(result,
                                   executor_tracer<ComposedExecutor, file_event_tracer<true>>{
                                       std::forward<ComposedExecutor>(exec), *first_policy->tracer, first_policy->name},
                                   std::forward<Policies>(policies)...);
  } else {
    static_assert(false, "Unsupported executor policy in make_executor_decorator_helper");
  }
}

} // namespace

std::unique_ptr<task_executor> srsran::decorate_executor(std::unique_ptr<task_executor>     exec,
                                                         const execution_decoration_config& config)
{
  std::unique_ptr<task_executor> result;
  make_executor_decorator_helper(result, std::move(exec), config.sync, config.metrics, config.trace);
  return result;
}

std::unique_ptr<task_executor> srsran::decorate_executor(task_executor& exec, const execution_decoration_config& config)
{
  std::unique_ptr<task_executor> result;
  make_executor_decorator_helper(result, exec, config.sync, config.metrics, config.trace);
  return result;
}
