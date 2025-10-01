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

#include "executor_metrics.h"
#include "srsran/adt/lockfree_triple_buffer.h"
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

  /// Maximum number of threads supported by the application.
  static constexpr size_t MAX_THREADS = 256;

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
    name(name_), threads_metrics(MAX_THREADS), last_tp(std::chrono::steady_clock::now())
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
