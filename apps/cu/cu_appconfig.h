/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <string>

namespace srsran {

/// Configuration of logging functionalities.
struct log_appconfig {
  /// Path to log file or "stdout" to print to console.
  std::string filename = "/tmp/gnb.log";
  /// Default log level for all layers.
  std::string all_level = "warning";
  /// Generic log level assigned to library components without layer-specific level.
  std::string lib_level     = "warning";
  std::string e2ap_level    = "warning";
  std::string config_level  = "none";
  std::string metrics_level = "none";
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Set to a valid file path to enable tracing and write the trace to the file.
  std::string tracing_filename;
};

/// Monolithic gnb application configuration.
struct cu_appconfig {
  /// Logging configuration.
  log_appconfig log_cfg;
  /// TODO fill in the rest of the configuration
};

} // namespace srsran
