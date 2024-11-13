/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "metrics_logger_appconfig_cli11_schema.h"
#include "logger_appconfig_cli11_utils.h"
#include "metrics_logger_appconfig.h"
#include "srsran/adt/span.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_log_args(CLI::App& app, metrics_logger_appconfig& log_params)
{
  auto metric_level_check = [](const std::string& value) -> std::string {
    if (auto level = srslog::str_to_basic_level(value); !level.has_value() || !is_valid_metrics_levels(level.value())) {
      return fmt::format("Log level value not supported. Accepted values [{}]",
                         span<const std::string_view>(valid_metrics_levels_names));
    }

    return {};
  };

  add_option_function<std::string>(
      app, "--metrics_level", app_services::capture_log_level_function(log_params.level), "Metrics log level")
      ->default_str(srslog::basic_level_to_string(log_params.level))
      ->check(metric_level_check);
  add_option(
      app, "--hex_max_size", log_params.hex_max_size, "Maximum number of bytes to print in hex (zero for no hex dumps)")
      ->capture_default_str()
      ->check(CLI::Range(0, 1024));
}

void srsran::configure_cli11_with_metrics_logger_appconfig_schema(CLI::App& app, metrics_logger_appconfig& config)
{
  // Logging section.
  CLI::App* log_subcmd = add_subcommand(app, "log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, config);
}
