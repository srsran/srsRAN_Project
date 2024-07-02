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

#include "logger_appconfig_cli11_schema.h"
#include "logger_appconfig.h"
#include "logger_appconfig_cli11_utils.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_log_args(CLI::App& app, logger_appconfig& log_params)
{
  auto metric_level_check = [](const std::string& value) -> std::string {
    if (auto level = srslog::str_to_basic_level(value); !level.has_value() ||
                                                        level.value() == srslog::basic_levels::error ||
                                                        level.value() == srslog::basic_levels::warning) {
      return "Log level value not supported. Accepted values [none,info,debug]";
    }

    return {};
  };

  app.add_option("--filename", log_params.filename, "Log file output path")->capture_default_str();

  app_services::add_log_option(
      app, log_params.all_level, "--all_level", "Default log level for PHY, MAC, RLC, PDCP, RRC, SDAP, NGAP and GTPU");
  app_services::add_log_option(app, log_params.lib_level, "--lib_level", "Generic log level ");
  app_services::add_log_option(app, log_params.e2ap_level, " --e2ap_level", "E2AP log level");

  add_option_function<std::string>(
      app, " --config_level", app_services::capture_log_level_function(log_params.config_level), "Config log level")
      ->default_str(srslog::basic_level_to_string(log_params.config_level))
      ->check(metric_level_check);
  add_option_function<std::string>(
      app, "--metrics_level", app_services::capture_log_level_function(log_params.metrics_level), "Metrics log level")
      ->default_str(srslog::basic_level_to_string(log_params.metrics_level))
      ->check(metric_level_check);
  add_option(
      app, "--hex_max_size", log_params.hex_max_size, "Maximum number of bytes to print in hex (zero for no hex dumps)")
      ->capture_default_str()
      ->check(CLI::Range(0, 1024));
  add_option(app, "--tracing_filename", log_params.tracing_filename, "Set to a valid file path to enable tracing")
      ->always_capture_default();

  // Post-parsing callback. This allows us to set the log level to "all" level, if no level is provided.
  app.callback([&]() {
    // Do nothing when all_level is not defined or it is defined as warning.
    if (app.count("--all_level") == 0 || log_params.all_level == srslog::basic_levels::warning) {
      return;
    }

    const auto options = app.get_options();
    for (auto* option : options) {
      // Skip all_level option and unrelated options to log level.
      if (option->check_name("--all_level") || option->get_name().find("level") == std::string::npos) {
        continue;
      }

      // Do nothing if option is present.
      if (option->count()) {
        continue;
      }

      // Config and metrics loggers have only subset of levels.
      if (option->check_name("--config_level") || option->check_name("--metrics_level")) {
        if (log_params.all_level == srslog::basic_levels::error) {
          option->default_val<std::string>("none");
          continue;
        }
      }

      option->default_val<std::string>(srslog::basic_level_to_string(log_params.all_level));
    }
  });
}

void srsran::configure_cli11_with_logger_appconfig_schema(CLI::App& app, logger_appconfig& config)
{
  // Logging section.
  CLI::App* log_subcmd = app.add_subcommand("log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, config);
}
