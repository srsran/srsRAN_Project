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

/// Metrics JSON configuration.
struct metrics_json_config {
  /// Enables the metrics in JSON format.
  bool enable_json_metrics = false;
  /// IP address the JSON metrics will be written into.
  std::string addr = "127.0.0.1";
  /// Port the JSON metrics will be written into.
  uint16_t port = 55555;
};

/// Metrics configuration structure.
struct metrics_config {
  /// Enables the metrics in the log.
  bool enable_log_metrics = false;
  /// JSON metrics configuration.
  metrics_json_config json_config;

  /// Returns true if the metrics are enabled, otherwise false.
  bool enabled() const { return enable_log_metrics || json_config.enable_json_metrics; }
};

} // namespace app_helpers
} // namespace srsran
