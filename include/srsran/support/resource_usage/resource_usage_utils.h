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

#include "srsran/adt/expected.h"
#include "srsran/support/resource_usage/resource_usage_metrics.h"
#include <chrono>
#include <sys/resource.h>

namespace srsran {
namespace resource_usage_utils {

using rusage_meas_clock      = std::chrono::high_resolution_clock;
using rusage_meas_time_point = rusage_meas_clock::time_point;
using rusage_meas_duration   = std::chrono::microseconds;

enum class rusage_measurement_type : __rusage_who_t { THREAD = RUSAGE_THREAD, PROCESS = RUSAGE_SELF };

/// Used to store the CPU time used by a thread or process, as well as process's memory usage.
struct measurements {
  /// Duration of the measurement.
  std::chrono::microseconds duration;
  /// User CPU time.
  std::chrono::microseconds user_time;
  /// System CPU time.
  std::chrono::microseconds system_time;
  /// Maximum resident set size (in kilobytes).
  long max_rss = -1;
  /// Resets measurements making them invalid.
  void reset()
  {
    duration    = std::chrono::microseconds::zero();
    user_time   = std::chrono::microseconds::zero();
    system_time = std::chrono::microseconds::zero();
    max_rss     = -1;
  }
  /// Returns true if the struct contains valid measurements, false otherwise.
  bool valid() const { return max_rss >= 0; }
};

/// Sums up two resource usage measurements.
measurements operator+(const measurements& lhs, const measurements& rhs);

/// Snapshot of the resource usage statistics of a specific thread or process at a given point in time.
struct cpu_snapshot {
  /// Time point when the snapshot was taken.
  rusage_meas_time_point tp;
  /// User CPU time.
  std::chrono::microseconds user_time = {};
  /// System CPU time.
  std::chrono::microseconds system_time = {};
  /// Maximum resident set size (in kilobytes).
  long max_rss;
};

/// On success returns a snapshot of the resource usage, otherwise returns an errno value.
expected<cpu_snapshot, int> cpu_usage_now(rusage_measurement_type type);

/// Convert measurements to metrics.
resource_usage_metrics res_usage_measurements_to_metrics(measurements measurements, std::chrono::microseconds period);

} // namespace resource_usage_utils
} // namespace srsran
