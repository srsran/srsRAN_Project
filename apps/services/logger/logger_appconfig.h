/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
  srslog::basic_levels lib_level     = srslog::basic_levels::warning;
  srslog::basic_levels e2ap_level    = srslog::basic_levels::warning;
  srslog::basic_levels config_level  = srslog::basic_levels::none;
  srslog::basic_levels metrics_level = srslog::basic_levels::none;
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Set to a valid file path to enable tracing and write the trace to the file.
  std::string tracing_filename;
};

} // namespace srsran
