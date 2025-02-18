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

#include "srsran/srslog/logger.h"
#include <string>

namespace srsran {

/// Configuration of logging functionalities.
struct logger_appconfig {
  /// Path to log file or "stdout" to print to console.
  std::string filename = "stdout";
  /// Default log level for all layers.
  srslog::basic_levels all_level = srslog::basic_levels::warning;
  /// Generic log level assigned to library components without layer-specific level.
  srslog::basic_levels lib_level    = srslog::basic_levels::warning;
  srslog::basic_levels e2ap_level   = srslog::basic_levels::warning;
  srslog::basic_levels config_level = srslog::basic_levels::none;

  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Set to a valid file path to enable tracing and write the trace to the file.
  std::string tracing_filename;
};

} // namespace srsran
