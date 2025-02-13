/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gnb_appconfig_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/services/hal/hal_cli11_schema.h"
#include "apps/services/logger/logger_appconfig_cli11_schema.h"
#include "apps/services/remote_control/remote_control_appconfig_cli11_schema.h"
#include "apps/services/worker_manager/worker_manager_cli11_schema.h"
#include "gnb_appconfig.h"
#include "srsran/support/cli11_utils.h"
#include "CLI/CLI11.hpp"

using namespace srsran;

static void configure_cli11_metrics_args(CLI::App& app, metrics_appconfig& metrics_params)
{
  app.add_option("--addr", metrics_params.addr, "Metrics address.")->capture_default_str();
  app.add_option("--port", metrics_params.port, "Metrics UDP port.")
      ->capture_default_str()
      ->check(CLI::Range(0, 65535));
  add_option(app, "--enable_json_metrics", metrics_params.enable_json_metrics, "Enable JSON metrics reporting")
      ->always_capture_default();
}

static void manage_hal_optional(CLI::App& app, gnb_appconfig& gnb_cfg)
{
  if (!is_hal_section_present(app)) {
    gnb_cfg.hal_config.reset();
  }
}

void srsran::configure_cli11_with_gnb_appconfig_schema(CLI::App& app, gnb_appconfig& gnb_parsed_cfg)
{
  gnb_appconfig& gnb_cfg = gnb_parsed_cfg;
  add_option(app, "--gnb_id", gnb_cfg.gnb_id.id, "gNodeB identifier")->capture_default_str();
  add_option(app, "--gnb_id_bit_length", gnb_cfg.gnb_id.bit_length, "gNodeB identifier length in bits")
      ->capture_default_str()
      ->check(CLI::Range(22, 32));
  add_option(app, "--ran_node_name", gnb_cfg.ran_node_name, "RAN node name")->capture_default_str();

  // Loggers section.
  configure_cli11_with_logger_appconfig_schema(app, gnb_cfg.log_cfg);

  // Buffer pool section.
  configure_cli11_with_buffer_pool_appconfig_schema(app, gnb_cfg.buffer_pool_config);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, gnb_cfg.expert_execution_cfg);

  // Metrics section.
  CLI::App* metrics_subcmd = app.add_subcommand("metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, gnb_cfg.metrics_cfg);

  // HAL section.
  gnb_cfg.hal_config.emplace();
  configure_cli11_with_hal_appconfig_schema(app, *gnb_cfg.hal_config);

  // Remote control section.
  configure_cli11_with_remote_control_appconfig_schema(app, gnb_cfg.remote_control_config);
}

void srsran::autoderive_gnb_parameters_after_parsing(CLI::App& app, gnb_appconfig& config)
{
  manage_hal_optional(app, config);
}
