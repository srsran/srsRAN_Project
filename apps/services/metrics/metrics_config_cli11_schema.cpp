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
#include "metrics_appconfig.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;
using namespace app_services;

static void configure_cli11_metrics_args(CLI::App& app, metrics_appconfig& metrics_params)
{
  auto* periodicity_subcmd = add_subcommand(app, "periodicity", "Metrics periodicity configuration")->configurable();
  add_option(*periodicity_subcmd,
             "--app_usage_report_period",
             metrics_params.app_usage_report_period,
             "Application resource usage metrics report period (in milliseconds)")
      ->capture_default_str();
}

void srsran::app_services::configure_cli11_with_metrics_appconfig_schema(CLI::App& app, metrics_appconfig& config)
{
  CLI::App* metircs_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metircs_subcmd, config);
}
