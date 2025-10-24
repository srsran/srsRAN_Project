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

#include "executor_metrics.h"
#include "srsran/adt/lockfree_triple_buffer.h"
#include "srsran/support/executors/unique_thread.h"
#include "srsran/support/tracing/resource_usage.h"
#include <chrono>
#include <vector>

namespace srsran {

/// \brief Executor metrics channel allows a task executor to pass its metrics to the backend.
///
/// On a request from the metrics backend the saved metrics are aggregated among different threads and forwarded to the
/// backend.
class executor_metrics_channel
{
  /// Metrics aggregated over a reporting period.
  struct aggregated_metrics {
    uint64_t                 count             = 0;
    uint64_t                 total_exec_count  = 0;
    uint64_t                 total_defer_count = 0;
    std::chrono::nanoseconds total_enqueue_latency{0};
    std::chrono::nanoseconds total_task_latency{0};
    std::chrono::nanoseconds max_enqueue_latency{0};
    std::chrono::nanoseconds max_task_latency{0};
    resource_usage::diff     accumulated_rusg = {};
  };

  /// Per thread executor metrics.
  struct executor_thread_metrics {
    /// Aggregated metrics available for the backend to read.
    lockfree_triple_buffer<aggregated_metrics> metrics;
    /// Local copy of aggregated metrics.
    aggregated_metrics temp_aggr_metrics;
    /// Atomic flag set by the backend when aggregated metrics must be reset.
    std::atomic<bool> reset_flag{false};
  };

public:
  /// Metrics of an executed task.
  struct executed_task_metrics {
    uint64_t                 exec_count  = 0;
    uint64_t                 defer_count = 0;
    std::chrono::nanoseconds enqueue_latency{0};
    std::chrono::nanoseconds task_latency{0};
    resource_usage::diff     rusg = {};
  };

  /// Constructor initializes internal storage for the maximum supported number of threads.
  explicit executor_metrics_channel(const std::string& name_) :
    name(name_),
    threads_metrics(unique_thread::get_max_nof_supported_threads()),
    last_tp(std::chrono::steady_clock::now())
  {
  }

  /// Update metrics associated with the calling thread.
  void update(const executed_task_metrics& new_metrics);

  /// Reads aggregated metrics.
  executor_metrics read();

private:
  /// Helper method for updating provided aggregated metrics with the new metrics.
  void update_metrics(aggregated_metrics& stored, const executed_task_metrics& new_metrics);

  /// Executor name.
  std::string name;
  /// Vector of executor metrics associated with each thread.
  std::vector<executor_thread_metrics> threads_metrics;
  /// Last read timestamp.
  std::chrono::time_point<std::chrono::steady_clock> last_tp;
};

} // namespace srsran
