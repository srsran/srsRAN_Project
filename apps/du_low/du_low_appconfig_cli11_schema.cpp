/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_low_appconfig_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_schema.h"
#include "apps/services/app_resource_usage/app_resource_usage_config_cli11_schema.h"
#include "apps/services/metrics/metrics_config_cli11_schema.h"
#include "apps/services/remote_control/remote_control_appconfig_cli11_schema.h"
#include "apps/services/worker_manager/worker_manager_cli11_schema.h"
#include "du_low_appconfig.h"

using namespace srsran;

void srsran::configure_cli11_with_du_low_appconfig_schema(CLI::App& app, du_low_appconfig& config)
{
  app.add_flag("--dryrun", config.enable_dryrun, "Enable application dry run mode")->capture_default_str();

  // Loggers section.
  configure_cli11_with_logger_appconfig_schema(app, config.log_cfg);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, config.expert_execution_cfg);

  // Metrics section.
  app_services::configure_cli11_with_app_resource_usage_config_schema(app, config.metrics_cfg.rusage_config);
  app_services::configure_cli11_with_metrics_appconfig_schema(app, config.metrics_cfg.metrics_service_cfg);

  // Remote control section.
  configure_cli11_with_remote_control_appconfig_schema(app, config.remote_control_config);
}

void srsran::autoderive_du_low_parameters_after_parsing(CLI::App& app, du_low_appconfig& config) {}
