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

#include "srsran/adt/mpmc_queue.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/detail/task_executor_utils.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/rtsan.h"
#include "srsran/support/tracing/resource_usage.h"
#include <chrono>

namespace srsran {

/// \brief Decorator of a task executor that tracks its performance metrics, such as latency.
/// \remark This class should only be used for concurrent executors (e.g. strands, single threads).
template <typename ExecutorType>
class concurrent_metrics_executor : public task_executor
{
  /// Maximum number of elements the pool can hold.
  static constexpr unsigned POOL_SIZE = 32 * 1024;

  using time_point = std::chrono::time_point<std::chrono::steady_clock>;
  using queue_type =
      concurrent_queue<unsigned, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;

  class task_executor_nomalloc
  {
  public:
    using task_type = unique_function<void(), default_unique_task_buffer_size, true>;

    task_executor_nomalloc(task_executor& executor_) : executor(executor_) {}

    bool execute(task_type&& task) { return executor.execute(std::move(task)); }

  private:
    task_executor& executor;
  };

public:
  template <typename U>
  concurrent_metrics_executor(std::string               name_,
                              U&&                       exec_,
                              task_executor&            seq_exec_,
                              srslog::log_channel&      metrics_logger_,
                              std::chrono::milliseconds period_) :
    name(std::move(name_)),
    exec(std::forward<U>(exec_)),
    seq_exec(seq_exec_),
    metrics_logger(metrics_logger_),
    logger(srslog::fetch_basic_logger("APP")),
    period(period_),
    task_pool(POOL_SIZE),
    free_tasks(std::make_unique<queue_type>(POOL_SIZE))
  {
    for (unsigned i = 0, e = task_pool.size(); i != e; ++i) {
      (void)free_tasks->try_push(i);
    }

    last_tp = std::chrono::steady_clock::now();
  }

  [[nodiscard]] bool execute(unique_task task) override
  {
    unsigned task_idx = 0;
    if (not free_tasks->try_pop(task_idx)) {
      logger.warning("Unable to execute new task in the '{}' metrics executor decorator", name);
      return false;
    }

    unique_task& pooled_task = task_pool[task_idx];
    pooled_task              = std::move(task);

    auto enqueue_tp = std::chrono::steady_clock::now();
    bool ret =
        detail::invoke_execute(exec, [this, enqueue_tp, task_idx]() { process_task<false>(task_idx, enqueue_tp); });
    if (not ret) {
      pooled_task = {};
      (void)free_tasks->try_push(task_idx);
    }
    return ret;
  }

  [[nodiscard]] bool defer(unique_task task) override
  {
    unsigned task_idx = 0;
    if (not free_tasks->try_pop(task_idx)) {
      logger.warning("Unable to defer new task in the '{}' metrics executor decorator", name);
      return false;
    }

    unique_task& pooled_task = task_pool[task_idx];
    pooled_task              = std::move(task);

    auto enqueue_tp = std::chrono::steady_clock::now();
    bool ret =
        detail::invoke_defer(exec, [this, enqueue_tp, task_idx]() { process_task<false>(task_idx, enqueue_tp); });
    if (not ret) {
      pooled_task = {};
      (void)free_tasks->try_push(task_idx);
    }
    return ret;
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

  template <bool isExec>
  void process_task(unsigned task_idx, std::chrono::steady_clock::time_point enqueue_tp)
  {
    auto start_tp   = std::chrono::steady_clock::now();
    auto start_rusg = resource_usage::now();

    unique_task& task = task_pool[task_idx];
    task();

    auto end_tp   = std::chrono::steady_clock::now();
    auto end_rusg = resource_usage::now();

    expected<resource_usage::diff, int> diff_rusg;
    if (end_rusg.has_value() && start_rusg.has_value()) {
      diff_rusg = *end_rusg - *start_rusg;
    }

    // Assume the queue latency is less than 2.1 seconds.
    unsigned enqueue_latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(start_tp - enqueue_tp).count();
    unsigned task_latency_ns    = std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - start_tp).count();

    bool success = seq_exec.execute([this, enqueue_latency_ns, task_latency_ns, end_tp, diff_rusg, task_idx]() {
      unique_task& pooled_task = task_pool[task_idx];

      handle_metrics(isExec, enqueue_latency_ns, task_latency_ns, end_tp, diff_rusg);

      pooled_task = {};
      (void)free_tasks->try_push(task_idx);
    });
    if (!success) {
      task = {};
      (void)free_tasks->try_push(task_idx);
      logger.debug("Unsuccessful execution of handle metrics.");
    }
  }

  void handle_metrics(bool                                       is_exec,
                      unsigned                                   enqueue_latency_ns,
                      unsigned                                   task_latency_ns,
                      time_point                                 end_tp,
                      const expected<resource_usage::diff, int>& diff_rusg) noexcept SRSRAN_RTSAN_NONBLOCKING
  {
    using namespace std::chrono;

    auto enqueue_latency = std::chrono::nanoseconds(enqueue_latency_ns);
    auto task_latency    = std::chrono::nanoseconds(task_latency_ns);

    // Update internal metrics.
    ++counters.dispatch_count;
    if (not is_exec) {
      ++counters.defer_count;
    }
    counters.enqueue_sum_latency += enqueue_latency;
    counters.enqueue_max_latency = std::max(counters.enqueue_max_latency, enqueue_latency);
    counters.task_sum_latency += task_latency;
    counters.task_max_latency = std::max(counters.task_max_latency, task_latency);
    if (diff_rusg.has_value()) {
      counters.sum_rusg += *diff_rusg;
    } else {
      ++counters.failed_rusg_captures;
    }

    // Check if it is time for a new metric report.
    auto telapsed_since_last_report = end_tp - last_tp;
    if (telapsed_since_last_report >= period) {
      // Report metrics.
      metrics_logger("Executor metrics \"{}\": nof_executes={} nof_defers={} enqueue_avg={}usec "
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
  task_executor_nomalloc          seq_exec;
  srslog::log_channel&            metrics_logger;
  srslog::basic_logger&           logger;
  const std::chrono::milliseconds period;
  std::vector<unique_task>        task_pool;
  std::unique_ptr<queue_type>     free_tasks;

  non_persistent_data                                counters;
  std::chrono::time_point<std::chrono::steady_clock> last_tp;
};

/// Returns an owning pointer to a task executor that decorates a concurrent executor to track its performance metrics
template <typename ConcurrentExec>
std::unique_ptr<task_executor> make_concurrent_metrics_executor_ptr(std::string               exec_name,
                                                                    ConcurrentExec&&          exec,
                                                                    task_executor&            seq_exec,
                                                                    srslog::log_channel&      logger,
                                                                    std::chrono::milliseconds period)
{
  return std::make_unique<concurrent_metrics_executor<ConcurrentExec>>(
      std::move(exec_name), std::forward<ConcurrentExec>(exec), seq_exec, logger, period);
}

} // namespace srsran
