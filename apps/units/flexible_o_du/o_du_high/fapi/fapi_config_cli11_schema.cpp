/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "fapi_config_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_utils.h"
#include "fapi_config.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_log_args(CLI::App& app, fapi_unit_config& log_params)
{
  app_helpers::add_log_option(app, log_params.fapi_level, "--fapi_level", "FAPI log level");
}

void srsran::configure_cli11_with_fapi_config_schema(CLI::App& app, fapi_unit_config& parsed_cfg)
{
  // Loggers section.
  CLI::App* log_subcmd = add_subcommand(app, "log", "Logging configuration")->configurable();
  configure_cli11_log_args(*log_subcmd, parsed_cfg);
}
