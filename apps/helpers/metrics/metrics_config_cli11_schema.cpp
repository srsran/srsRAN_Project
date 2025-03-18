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
#include "metrics_config.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;
using namespace app_helpers;

static void configure_cli11_metrics_args(CLI::App& app, metrics_config& metrics_params)
{
  // JSON metrics configuration.
  add_option(app, "--addr", metrics_params.json_config.addr, "IP address JSON metrics will be reported to")
      ->capture_default_str();
  add_option(app, "--port", metrics_params.json_config.port, "UDP port JSON metrics will be reported to")
      ->capture_default_str()
      ->check(CLI::Range(0, 65535));
  add_option(app, "--enable_json", metrics_params.json_config.enable_json_metrics, "Enable JSON metrics reporting")
      ->always_capture_default();

  // Log metrics configuration.
  add_option(app, "--enable_log", metrics_params.enable_log_metrics, "Enable log metrics reporting")
      ->always_capture_default();
}

void srsran::app_helpers::configure_cli11_with_metrics_appconfig_schema(CLI::App& app, metrics_config& config)
{
  CLI::App* log_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*log_subcmd, config);
}
