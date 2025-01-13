/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
