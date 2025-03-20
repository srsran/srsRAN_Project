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

#include "../support/metrics_helpers.h"
#include "srsran/ofh/receiver/ofh_receiver_metrics.h"
#include <array>
#include <atomic>
#include <chrono>

namespace srsran {
namespace ofh {

/// Open Fronthaul receiver performance metrics collector.
class receiver_performance_metrics_collector
{
  /// Type of the processed message.
  enum message_type { DATA, PRACH, NOF_MSG_TYPES };

public:
  explicit receiver_performance_metrics_collector(bool is_enabled) : is_disabled(!is_enabled) {}

  /// Collect the performance metrics of an Open Fronthaul receiver.
  void collect_metrics(receiver_performance_metrics& metric);

  /// Returns true if the metrics collector is disabled, false otherwise.
  bool disabled() const { return is_disabled; }

  /// Returns new measurement object.
  time_execution_measurer create_time_execution_measurer() const { return time_execution_measurer(!disabled()); }

  /// Updates the processing performance statistics
  ///
  /// \param[in] exec_latency Execution time taken by processing a received message.
  /// \param[in] is_prach     True if a PRACH message processing was measured, false otherwise.
  void update_receiver_stats(std::chrono::nanoseconds exec_latency, bool is_prach = false);

private:
  static constexpr uint32_t default_min_latency_ns = std::numeric_limits<uint32_t>::max();
  static constexpr uint32_t default_max_latency_ns = std::numeric_limits<uint32_t>::min();

  /// Internal structure storing accumulated statistics. It is used for PRACH and data messages.
  struct message_processing_stats {
    std::atomic<uint32_t> count          = {};
    std::atomic<uint64_t> sum_elapsed_ns = {};
    std::atomic<uint32_t> min_latency_ns = default_min_latency_ns;
    std::atomic<uint32_t> max_latency_ns = default_max_latency_ns;
  };

  /// Updates passed metrics given the accumulated message processing stats.
  void update_metrics(receiver_performance_metrics::unpacking_perf_metrics& metrics,
                      const message_processing_stats&                       stats) const;

  /// Resets values of all internal counters.
  void reset();

  std::array<message_processing_stats, NOF_MSG_TYPES> processing_stats;
  const bool                                          is_disabled;
};

} // namespace ofh
} // namespace srsran
