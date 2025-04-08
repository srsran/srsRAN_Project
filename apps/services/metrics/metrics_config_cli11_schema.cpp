/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
