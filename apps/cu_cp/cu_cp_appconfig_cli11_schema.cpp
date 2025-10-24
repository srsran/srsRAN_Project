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

#include "cu_cp_appconfig_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_schema.h"
#include "apps/helpers/tracing/tracer_appconfig_cli11_schema.h"
#include "apps/services/app_resource_usage/app_resource_usage_config_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/services/metrics/metrics_config_cli11_schema.h"
#include "apps/services/remote_control/remote_control_appconfig_cli11_schema.h"
#include "apps/services/worker_manager/worker_manager_cli11_schema.h"
#include "cu_cp_appconfig.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_e1ap_args(CLI::App& app, srs_cu_cp::e1ap_appconfig& e1ap_params)
{
  add_option(app, "--bind_addr", e1ap_params.bind_addr, "E1AP bind address")->capture_default_str();
}

static void configure_cli11_f1ap_args(CLI::App& app, srs_cu_cp::f1ap_appconfig& f1ap_params)
{
  add_option(app, "--bind_addr", f1ap_params.bind_addr, "F1-C bind address")->capture_default_str();
}

void srsran::configure_cli11_with_cu_cp_appconfig_schema(CLI::App& app, cu_cp_appconfig& cu_cp_cfg)
{
  app.add_flag("--dryrun", cu_cp_cfg.enable_dryrun, "Enable application dry run mode")->capture_default_str();

  // Logging section.
  configure_cli11_with_logger_appconfig_schema(app, cu_cp_cfg.log_cfg);

  // Tracers section.
  configure_cli11_with_tracer_appconfig_schema(app, cu_cp_cfg.trace_cfg);

  // Buffer pool section.
  configure_cli11_with_buffer_pool_appconfig_schema(app, cu_cp_cfg.buffer_pool_config);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, cu_cp_cfg.expert_execution_cfg);

  // Remote control section.
  configure_cli11_with_remote_control_appconfig_schema(app, cu_cp_cfg.remote_control_config);

  // Metrics section.
  app_services::configure_cli11_with_app_resource_usage_config_schema(app, cu_cp_cfg.metrics_cfg.rusage_config);
  app_services::configure_cli11_with_metrics_appconfig_schema(app, cu_cp_cfg.metrics_cfg.metrics_service_cfg);

  CLI::App* cu_cp_subcmd = add_subcommand(app, "cu_cp", "CU-CP parameters")->configurable();
  // E1AP section.
  CLI::App* e1ap_subcmd = add_subcommand(*cu_cp_subcmd, "e1ap", "E1AP parameters")->configurable();
  configure_cli11_e1ap_args(*e1ap_subcmd, cu_cp_cfg.e1ap_cfg);
  // F1AP section.
  CLI::App* f1ap_subcmd = add_subcommand(*cu_cp_subcmd, "f1ap", "F1AP parameters")->configurable();
  configure_cli11_f1ap_args(*f1ap_subcmd, cu_cp_cfg.f1ap_cfg);
}
