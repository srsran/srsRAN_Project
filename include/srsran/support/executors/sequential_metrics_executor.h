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
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/tracing/resource_usage.h"
#include <chrono>

namespace srsran {

/// \brief Decorator of a task executor that tracks its performance metrics, such as latency.
/// \remark This class should only be used for sequential executors (e.g. strands, single threads).
template <typename ExecutorType, typename Logger>
class sequential_metrics_executor : public task_executor
{
  using time_point = std::chrono::time_point<std::chrono::steady_clock>;

public:
  template <typename U>
  sequential_metrics_executor(std::string name_, U&& exec_, Logger& logger_, std::chrono::milliseconds period_) :
    name(std::move(name_)), exec(std::forward<U>(exec_)), logger(logger_), period(period_)
  {
    last_tp = std::chrono::steady_clock::now();
  }

  [[nodiscard]] bool execute(unique_task task) override
  {
    auto enqueue_tp = std::chrono::steady_clock::now();
    return detail::invoke_execute(exec, [this, t = std::move(task), enqueue_tp]() {
      auto start_tp   = std::chrono::steady_clock::now();
      auto start_rusg = resource_usage::now();
      t();
      handle_metrics(true, enqueue_tp, start_tp, start_rusg);
    });
  }

  [[nodiscard]] bool defer(unique_task task) override
  {
    auto enqueue_tp = std::chrono::steady_clock::now();
    return detail::invoke_defer(exec, [this, t = std::move(task), enqueue_tp]() {
      auto start_tp   = std::chrono::steady_clock::now();
      auto start_rusg = resource_usage::now();
      t();
      handle_metrics(false, enqueue_tp, start_tp, start_rusg);
    });
  }

private:
  struct non_persistent_data {
    uint64_t                 dispatch_count = 0;
    uint64_t                 defer_count    = 0;
    std::chrono::nanoseconds enqueue_sum_latency{0};
    std::chrono::nanoseconds enqueue_max_latency{0};
    std::chrono::nanoseconds task_sum_latency{0};
    std::chrono::nanoseconds task_max_latency{0};
    resource_usage::diff     sum_rusg             = {};
    unsigned                 failed_rusg_captures = 0;
  };

  void handle_metrics(bool                                           is_exec,
                      time_point                                     enqueue_tp,
                      time_point                                     start_tp,
                      const expected<resource_usage::snapshot, int>& start_rusg)
  {
    using namespace std::chrono;

    auto end_tp          = steady_clock::now();
    auto end_rusg        = resource_usage::now();
    auto enqueue_latency = start_tp - enqueue_tp;
    auto task_latency    = end_tp - start_tp;

    // Update internal metrics.
    counters.dispatch_count++;
    if (not is_exec) {
      counters.defer_count++;
    }
    counters.enqueue_sum_latency += enqueue_latency;
    counters.enqueue_max_latency = std::max(counters.enqueue_max_latency, enqueue_latency);
    counters.task_sum_latency += task_latency;
    counters.task_max_latency = std::max(counters.task_max_latency, task_latency);
    if (end_rusg.has_value() and start_rusg.has_value()) {
      counters.sum_rusg += *end_rusg - *start_rusg;
    } else {
      counters.failed_rusg_captures++;
    }

    // Check if it is time for a new metric report.
    auto telapsed_since_last_report = end_tp - last_tp;
    if (telapsed_since_last_report >= period) {
      // Report metrics.
      logger.info("Executor metrics \"{}\": nof_executes={} nof_defers={} enqueue_avg={}usec "
                  "enqueue_max={}usec task_avg={}usec task_max={}usec cpu_load={:.1f}% {}",
                  name,
                  counters.dispatch_count - counters.defer_count,
                  counters.defer_count,
                  duration_cast<microseconds>(counters.enqueue_sum_latency / counters.dispatch_count).count(),
                  duration_cast<microseconds>(counters.enqueue_max_latency).count(),
                  duration_cast<microseconds>(counters.task_sum_latency / counters.dispatch_count).count(),
                  duration_cast<microseconds>(counters.task_max_latency).count(),
                  counters.task_sum_latency.count() * 100.0 / telapsed_since_last_report.count(),
                  counters.sum_rusg);

      counters = {};
      last_tp  = end_tp;
    }
  }

  std::string                     name;
  ExecutorType                    exec;
  Logger&                         logger;
  const std::chrono::milliseconds period;

  non_persistent_data                                counters;
  std::chrono::time_point<std::chrono::steady_clock> last_tp;
};

/// Returns a task executor that decorates a sequential executor to track its performance metrics
template <typename SequentialExec, typename Logger>
sequential_metrics_executor<SequentialExec, Logger>
make_metrics_executor(std::string exec_name, SequentialExec&& exec, Logger& logger, std::chrono::milliseconds period)
{
  return sequential_metrics_executor<SequentialExec, Logger>(
      std::move(exec_name), std::forward<SequentialExec>(exec), logger, period);
}

/// Returns an owning pointer to a task executor that decorates a sequential executor to track its performance metrics
template <typename SequentialExec, typename Logger>
std::unique_ptr<task_executor> make_metrics_executor_ptr(std::string               exec_name,
                                                         SequentialExec&&          exec,
                                                         Logger&                   logger,
                                                         std::chrono::milliseconds period)
{
  return std::make_unique<sequential_metrics_executor<SequentialExec, Logger>>(
      std::move(exec_name), std::forward<SequentialExec>(exec), logger, period);
}

} // namespace srsran
