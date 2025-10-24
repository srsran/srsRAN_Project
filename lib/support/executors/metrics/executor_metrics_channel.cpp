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

#include "srsran/support/executors/metrics/executor_metrics_channel.h"
#include "srsran/support/executors/unique_thread.h"

using namespace srsran;

void executor_metrics_channel::update(const executed_task_metrics& new_metrics)
{
  unsigned thread_idx = get_thread_index();
  report_error_if_not(thread_idx < threads_metrics.size(), "Thread ID exceeds the size of per-thread metrics array");

  auto& aggr_metrics = threads_metrics[thread_idx].temp_aggr_metrics;

  bool reset = threads_metrics[thread_idx].reset_flag.exchange(false, std::memory_order_acq_rel);
  if (reset) {
    aggr_metrics = {};
  }
  update_metrics(aggr_metrics, new_metrics);

  // Write the latest aggregated metrics to the triple buffer.
  threads_metrics[thread_idx].metrics.write_and_commit(aggr_metrics);
}

void executor_metrics_channel::update_metrics(aggregated_metrics& stored, const executed_task_metrics& new_metrics)
{
  // Add new metrics to the aggregated ones.
  stored.total_defer_count += new_metrics.defer_count;
  stored.total_exec_count += new_metrics.exec_count;
  stored.total_enqueue_latency += new_metrics.enqueue_latency;
  stored.total_task_latency += new_metrics.task_latency;
  if (new_metrics.enqueue_latency > stored.max_enqueue_latency) {
    stored.max_enqueue_latency = new_metrics.enqueue_latency;
  }
  if (new_metrics.task_latency > stored.max_task_latency) {
    stored.max_task_latency = new_metrics.task_latency;
  }
  stored.accumulated_rusg += new_metrics.rusg;
  ++stored.count;
}

executor_metrics executor_metrics_channel::read()
{
  using namespace std::chrono;

  executor_metrics new_report  = {};
  unsigned         total_count = 0;

  nanoseconds max_enqueue_latency{0};
  nanoseconds max_task_latency{0};
  nanoseconds total_enqueue_latency{0};
  nanoseconds total_task_latency{0};

  auto current_tp = steady_clock::now();
  auto duration   = current_tp - last_tp;

  // Aggregate metrics among all threads, calculate statistics and return final metrics to the backend.
  for (auto& thread_metric : threads_metrics) {
    if (thread_metric.reset_flag.load(std::memory_order_acquire)) {
      // The thread didn't run any task since last report.
      continue;
    }

    // Read the latest values from the triple buffer.
    const auto& metrics = thread_metric.metrics.read();
    thread_metric.reset_flag.store(true, std::memory_order_release);

    new_report.nof_defers += metrics.total_defer_count;
    new_report.nof_executes += metrics.total_exec_count;
    if (metrics.max_enqueue_latency > max_enqueue_latency) {
      max_enqueue_latency = metrics.max_enqueue_latency;
    }
    if (metrics.max_task_latency > max_task_latency) {
      max_task_latency = metrics.max_task_latency;
    }
    new_report.total_rusg += metrics.accumulated_rusg;

    total_enqueue_latency += metrics.total_enqueue_latency;
    total_task_latency += metrics.total_task_latency;
    total_count += metrics.count;
  }

  if (SRSRAN_LIKELY(total_count > 0)) {
    new_report.avg_enqueue_latency_us = duration_cast<microseconds>(total_enqueue_latency / total_count);
    new_report.avg_task_us            = duration_cast<microseconds>(total_task_latency / total_count);
    new_report.max_enqueue_latency_us = duration_cast<microseconds>(max_enqueue_latency);
    new_report.max_task_us            = duration_cast<microseconds>(max_task_latency);
    new_report.cpu_load               = total_task_latency.count() * 100.f / duration.count();
  } else {
    new_report = {};
  }
  new_report.name = name;

  last_tp = std::chrono::steady_clock::now();

  return new_report;
}
