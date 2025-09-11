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

#include "metrics_config_cli11_schema.h"
#include "metrics_config.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;
using namespace app_helpers;

static void configure_cli11_metrics_args(CLI::App& app, metrics_config& metrics_params)
{
  // JSON metrics configuration.
  add_option(app, "--enable_json", metrics_params.enable_json_metrics, "Enable JSON metrics reporting")
      ->always_capture_default();

  // Log metrics configuration.
  add_option(app, "--enable_log", metrics_params.enable_log_metrics, "Enable log metrics reporting")
      ->always_capture_default();
  add_option(app, "--enable_verbose", metrics_params.enable_verbose, "Enable extended detail metrics reporting")
      ->always_capture_default();
}

void srsran::app_helpers::configure_cli11_with_metrics_appconfig_schema(CLI::App& app, metrics_config& config)
{
  CLI::App* log_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*log_subcmd, config);
}
