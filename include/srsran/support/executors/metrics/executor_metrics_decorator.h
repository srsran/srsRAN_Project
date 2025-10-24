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

#include "executor_metrics_channel.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/detail/task_executor_utils.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"
#include "srsran/support/rtsan.h"
#include "srsran/support/tracing/event_tracing.h"
#include "srsran/support/tracing/resource_usage.h"
#include <chrono>
#include <string>

namespace srsran {

/// Decorator of a task executor that tracks its performance metrics, such as latency.
template <typename ExecutorType, typename Tracer = detail::null_event_tracer>
class executor_metrics_decorator : public task_executor
{
  /// Maximum number of elements the pool can hold.
  static constexpr unsigned POOL_SIZE = 32 * 1024;

  /// Called before an allocated task is reclaimed by the pool, to ensure its capture is reset.
  struct task_pool_resetter {
    void operator()(unique_task& t)
    {
      // Reset task to release any captured resources.
      t = {};
    }
  };

  using snapshot        = srsran::resource_usage::snapshot;
  using time_point      = trace_clock::time_point;
  using task_pool_type  = bounded_object_pool<unique_task, task_pool_resetter>;
  using unique_task_ptr = typename task_pool_type::ptr;

public:
  template <typename U>
  executor_metrics_decorator(std::string               name_,
                             U&&                       exec_,
                             executor_metrics_channel& metrics_channel_,
                             Tracer*                   tracer_ = nullptr) :
    name(std::move(name_)),
    exec(std::forward<U>(exec_)),
    metric_channel(metrics_channel_),
    logger(srslog::fetch_basic_logger("APP")),
    tracer(tracer_),
    trace_name(tracer == nullptr ? "" : fmt::format("{}_run", name)),
    task_pool(POOL_SIZE)
  {
  }

  // See interface for documentation.
  [[nodiscard]] bool execute(unique_task task) override
  {
    time_point enqueue_tp = std::chrono::steady_clock::now();

    unique_task_ptr pooled_task_ptr = task_pool.get();
    if (!pooled_task_ptr) {
      logger.warning("Unable to execute new task in the '{}' metrics executor decorator", name);
      return false;
    }

    *pooled_task_ptr = std::move(task);

    bool ret = detail::invoke_execute(exec, [this, enqueue_tp, pool_task_ptr = std::move(pooled_task_ptr)]() mutable {
      run_task<true>(enqueue_tp, std::move(pool_task_ptr));
    });

    return ret;
  }

  // See interface for documentation.
  [[nodiscard]] bool defer(unique_task task) override
  {
    time_point enqueue_tp = std::chrono::steady_clock::now();

    unique_task_ptr pooled_task_ptr = task_pool.get();
    if (pooled_task_ptr == nullptr) {
      logger.warning("Unable to execute new task in the '{}' metrics executor decorator", name);
      return false;
    }

    *pooled_task_ptr = std::move(task);

    bool ret = detail::invoke_defer(exec, [this, enqueue_tp, pool_task_ptr = std::move(pooled_task_ptr)]() mutable {
      run_task<false>(enqueue_tp, std::move(pool_task_ptr));
    });

    return ret;
  }

private:
  /// Calculates and pushes execution metrics to the metrics channel.
  template <bool isExec>
  void run_task(time_point enqueue_tp, unique_task_ptr task_ptr)
  {
    auto start_tp   = std::chrono::steady_clock::now();
    auto start_rusg = resource_usage::now();

    // Run task.
    (*task_ptr)();

    // Release task back to the pool (this will also clear the task captured resources).
    task_ptr.reset();

    handle_metrics(isExec, enqueue_tp, start_tp, start_rusg);
  }

  /// Calculates and pushes execution metrics to the metrics channel.
  void handle_metrics(bool is_exec, time_point enqueue_tp, time_point start_tp, expected<snapshot, int> start_rusg)
  {
    auto end_tp   = std::chrono::steady_clock::now();
    auto end_rusg = resource_usage::now();

    // Trace if enabled.
    if (tracer != nullptr and tracer->is_enabled()) {
      (*tracer) << trace_event(trace_name.c_str(), start_tp);
    }

    expected<resource_usage::diff, int> diff_rusg;
    if (end_rusg.has_value() && start_rusg.has_value()) {
      diff_rusg = *end_rusg - *start_rusg;
    }

    executor_metrics_channel::executed_task_metrics metrics;
    metrics.exec_count      = is_exec;
    metrics.defer_count     = !is_exec;
    metrics.enqueue_latency = std::chrono::duration_cast<std::chrono::nanoseconds>(start_tp - enqueue_tp);
    metrics.task_latency    = std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - start_tp);
    if (diff_rusg.has_value()) {
      metrics.rusg = *diff_rusg;
    }

    // Report new metrics to the channel.
    metric_channel.update(metrics);
  }

  std::string               name;
  ExecutorType              exec;
  executor_metrics_channel& metric_channel;
  srslog::basic_logger&     logger;
  Tracer*                   tracer;
  const std::string         trace_name;

  /// Object pool of unique_tasks.
  task_pool_type task_pool;
};

} // namespace srsran
