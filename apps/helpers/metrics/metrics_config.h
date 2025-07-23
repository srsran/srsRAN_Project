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

#include <cstdint>
#include <string>

namespace srsran {
namespace app_helpers {

/// Metrics configuration structure.
struct metrics_config {
  /// Enables the metrics in the log.
  bool enable_log_metrics = false;
  /// Enables the metrics in JSON format.
  bool enable_json_metrics = false;
  /// Enable verbose metrics.
  bool enable_verbose = false;

  /// Returns true if the metrics are enabled, otherwise false.
  bool enabled() const { return enable_log_metrics || enable_json_metrics; }
};

} // namespace app_helpers
} // namespace srsran
