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

#include "detail/task_executor_utils.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/tracing/resource_usage.h"
#include <chrono>

namespace srsran {
template <typename ExecutorType, typename Logger>
class metrics_executor : public task_executor
{
  using time_point = std::chrono::time_point<std::chrono::steady_clock>;

public:
  template <typename U>
  explicit metrics_executor(std::string name_, U&& exec_, Logger& logger_, std::chrono::milliseconds period_) :
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
    auto end_tp          = std::chrono::steady_clock::now();
    auto end_rusg        = resource_usage::now();
    auto enqueue_latency = start_tp - enqueue_tp;
    auto task_latency    = end_tp - start_tp;

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

    if (end_tp - last_tp >= period) {
      // Report metrics.
      logger.info("\"{}\" executor metrics: nof_executes={} nof_defers={} enqueue_avg={}nsec "
                  "enqueue_max={}nsec task_avg={}nsec task_max={}nsec {}",
                  name,
                  counters.dispatch_count - counters.defer_count,
                  counters.defer_count,
                  counters.enqueue_sum_latency.count() / counters.dispatch_count,
                  counters.enqueue_max_latency.count(),
                  counters.task_sum_latency.count() / counters.dispatch_count,
                  counters.task_max_latency.count(),
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

template <typename ExecType, typename Logger>
std::unique_ptr<task_executor>
make_metrics_executor(std::string exec_name, ExecType&& exec, Logger& logger, std::chrono::milliseconds period)
{
  return std::make_unique<metrics_executor<ExecType, Logger>>(
      std::move(exec_name), std::forward<ExecType>(exec), logger, period);
}

} // namespace srsran
