/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/helpers/metrics/metrics_config_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;
using namespace app_services;

static void configure_cli11_buffer_pool_args(CLI::App& app, buffer_pool_appconfig& config)
{
  app.add_option("--nof_segments", config.nof_segments, "Number of segments allocated by the buffer pool")
      ->capture_default_str();
  app.add_option("--segment_size", config.segment_size, "Size of each buffer pool segment in bytes")
      ->capture_default_str();
}

static void configure_cli11_metrics_args(CLI::App& app, bool& enable_metrics)
{
  auto* layers_subcmd = add_subcommand(app, "layers", "Layer basis metrics configuration")->configurable();
  add_option(*layers_subcmd, "--enable_app_usage", enable_metrics, "Enable application usage metrics")
      ->capture_default_str();
}

void srsran::app_services::configure_cli11_with_buffer_pool_appconfig_schema(CLI::App&              app,
                                                                             buffer_pool_appconfig& config)
{
  // Buffer pool section.
  CLI::App* buffer_pool_subcmd = app.add_subcommand("buffer_pool", "Buffer pool configuration")->configurable();
  configure_cli11_buffer_pool_args(*buffer_pool_subcmd, config);

  // Metrics section.
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, config.metrics_config.enable_metrics);
  app_helpers::configure_cli11_with_metrics_appconfig_schema(app, config.metrics_config.common_metrics_cfg);
}
