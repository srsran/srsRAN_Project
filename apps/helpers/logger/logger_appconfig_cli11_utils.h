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
#include "srsran/support/cli11_utils.h"

namespace srsran {
namespace app_helpers {

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

} // namespace app_helpers
} // namespace srsran
