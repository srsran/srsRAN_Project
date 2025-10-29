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

namespace srsran {
namespace ofh {

/// Open Fronthaul timing metrics.
struct timing_metrics {
  /// Number of symbols skipped when the timing worker wakes up late.
  unsigned nof_skipped_symbols;
  /// Max burst of skipped symbols.
  unsigned skipped_symbols_max_burst;
  /// Latency of a symbol notification.
  float notification_min_latency_us;
  float notification_max_latency_us;
  float notification_avg_latency_us;
};

} // namespace ofh
} // namespace srsran
