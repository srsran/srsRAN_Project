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
#include "srsran/support/cli11_utils.h"

namespace srsran {
namespace app_services {

/// Function to capture the log level.
inline auto capture_log_level_function = [](srslog::basic_levels& level) {
  return [&level](const std::string& value) {
    auto val = srslog::str_to_basic_level(value);
    level    = (val) ? val.value() : srslog::basic_levels::none;
  };
};

/// Helper function to add log options to CLI11.
inline CLI::Option*
add_log_option(CLI::App& app, srslog::basic_levels& level, const std::string& name, const std::string& descriptrion)
{
  /// Function to check that the log level is correct.
  auto check_log_level = [](const std::string& value) -> std::string {
    if (srslog::str_to_basic_level(value).has_value()) {
      return {};
    }

    return fmt::format("Log level '{}' not supported. Accepted values [none,info,debug,warning,error]", value);
  };

  return add_option_function<std::string>(app, name, capture_log_level_function(level), descriptrion)
      ->default_str(srslog::basic_level_to_string(level))
      ->check(check_log_level);
}

} // namespace app_services
} // namespace srsran
