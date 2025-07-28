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
