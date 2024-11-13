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

#include "srsran/adt/to_array.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// List of possible log levels for the metrics.
constexpr auto valid_metrics_levels =
    to_array<srslog::basic_levels>({srslog::basic_levels::none, srslog::basic_levels::info});

/// List of possible log levels for the metrics in string format.
constexpr auto valid_metrics_levels_names = to_array<std::string_view>({"none", "info"});

/// Returns true if the given level is defined inside de valid metrics level, otherwise false.
inline bool is_valid_metrics_levels(srslog::basic_levels level)
{
  return std::find_if(valid_metrics_levels.cbegin(), valid_metrics_levels.cend(), [level](const auto& lvl) {
           return level == lvl;
         }) != valid_metrics_levels.cend();
}

/// Metrics logger configuration.
struct metrics_logger_appconfig {
  srslog::basic_levels level = srslog::basic_levels::none;
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
};

} // namespace srsran
