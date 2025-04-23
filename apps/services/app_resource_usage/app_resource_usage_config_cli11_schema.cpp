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

#include "app_resource_usage_config_cli11_schema.h"
#include "app_resource_usage_config.h"
#include "apps/helpers/metrics/metrics_config_cli11_schema.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;
using namespace app_services;

static void configure_cli11_metrics_args(CLI::App& app, bool& enable_metrics)
{
  auto* layers_subcmd = add_subcommand(app, "layers", "Layer basis metrics configuration")->configurable();
  add_option(*layers_subcmd, "--enable_app_usage", enable_metrics, "Enable application usage metrics")
      ->capture_default_str();
}

void srsran::app_services::configure_cli11_with_app_resource_usage_config_schema(CLI::App&                  app,
                                                                                 app_resource_usage_config& config)
{
  // Metrics section.
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, config.enable_app_usage);
  app_helpers::configure_cli11_with_metrics_appconfig_schema(app, config.metrics_consumers_cfg);
}
