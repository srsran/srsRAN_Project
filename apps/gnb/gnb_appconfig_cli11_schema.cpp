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

#include "gnb_appconfig_cli11_schema.h"
#include "apps/helpers/hal/hal_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_schema.h"
#include "apps/helpers/tracing/tracer_appconfig_cli11_schema.h"
#include "apps/services/app_execution_metrics/executor_metrics_config_cli11_schema.h"
#include "apps/services/app_resource_usage/app_resource_usage_config_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/services/metrics/metrics_config_cli11_schema.h"
#include "apps/services/remote_control/remote_control_appconfig_cli11_schema.h"
#include "apps/services/worker_manager/worker_manager_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config.h"
#include "apps/units/o_cu_cp/cu_cp/cu_cp_unit_config.h"
#include "gnb_appconfig.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/cli11_utils.h"
#include "CLI/CLI11.hpp"

using namespace srsran;

static void configure_cli11_metrics_args(CLI::App& app, metrics_appconfig& metrics_params)
{
  add_option(
      app, "--autostart_stdout_metrics", metrics_params.autostart_stdout_metrics, "Autostart stdout metrics reporting")
      ->capture_default_str();
}

#ifdef DPDK_FOUND
static void manage_hal_optional(CLI::App& app, gnb_appconfig& gnb_cfg)
{
  if (!is_hal_section_present(app)) {
    gnb_cfg.hal_config.reset();
  }
}
#endif

void srsran::configure_cli11_with_gnb_appconfig_schema(CLI::App& app, gnb_appconfig& gnb_parsed_cfg)
{
  gnb_appconfig& gnb_cfg = gnb_parsed_cfg;
  app.add_flag("--dryrun", gnb_cfg.enable_dryrun, "Enable application dry run mode")->capture_default_str();

  add_option(app, "--gnb_id", gnb_cfg.gnb_id.id, "gNodeB identifier")->capture_default_str();
  add_option(app, "--gnb_id_bit_length", gnb_cfg.gnb_id.bit_length, "gNodeB identifier length in bits")
      ->capture_default_str()
      ->check(CLI::Range(22, 32));
  add_option(app, "--ran_node_name", gnb_cfg.ran_node_name, "RAN node name")->capture_default_str();

  // Loggers section.
  configure_cli11_with_logger_appconfig_schema(app, gnb_cfg.log_cfg);

  // Tracers section.
  configure_cli11_with_tracer_appconfig_schema(app, gnb_cfg.trace_cfg);

  // Buffer pool section.
  configure_cli11_with_buffer_pool_appconfig_schema(app, gnb_cfg.buffer_pool_config);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, gnb_cfg.expert_execution_cfg);

  // Metrics section.
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, gnb_cfg.metrics_cfg);
  app_services::configure_cli11_with_executor_metrics_appconfig_schema(app, gnb_cfg.metrics_cfg.executors_metrics_cfg);
  app_services::configure_cli11_with_app_resource_usage_config_schema(app, gnb_cfg.metrics_cfg.rusage_config);
  app_services::configure_cli11_with_metrics_appconfig_schema(app, gnb_cfg.metrics_cfg.metrics_service_cfg);

#ifdef DPDK_FOUND
  // HAL section.
  gnb_cfg.hal_config.emplace();
  configure_cli11_with_hal_appconfig_schema(app, *gnb_cfg.hal_config);
#else
  app.failure_message([](const CLI::App* application, const CLI::Error& e) -> std::string {
    if (std::string(e.what()).find("INI was not able to parse hal.++") == std::string::npos) {
      return CLI::FailureMessage::simple(application, e);
    }

    return "Invalid configuration detected, 'hal' section is present but the application was built without DPDK "
           "support\n" +
           CLI::FailureMessage::simple(application, e);
  });
#endif

  // Remote control section.
  configure_cli11_with_remote_control_appconfig_schema(app, gnb_cfg.remote_control_config);
}

void srsran::autoderive_gnb_parameters_after_parsing(CLI::App& app, gnb_appconfig& config)
{
#ifdef DPDK_FOUND
  manage_hal_optional(app, config);
#endif
}

void srsran::autoderive_supported_tas_for_amf_from_du_cells(const du_high_unit_config& du_hi_cfg,
                                                            cu_cp_unit_config&         cu_cp_cfg)
{
  // If no cells are found in DU configuration.
  if (du_hi_cfg.cells_cfg.empty()) {
    return;
  }

  // Clear supported TAs.
  srslog::fetch_basic_logger("CONFIG").debug(
      "{} supported TAs will be derived from DU cell config",
      cu_cp_cfg.amf_config.amf.is_default_supported_tas ? "No supported TAs configured," : "--no-core configured,");
  cu_cp_cfg.amf_config.amf.supported_tas.clear();
  cu_cp_cfg.amf_config.amf.is_default_supported_tas = false;

  // Derive supported TAs from DU cell configuration.
  for (const auto& cell : du_hi_cfg.cells_cfg) {
    cu_cp_unit_supported_ta_item supported_ta;
    supported_ta.tac = cell.cell.tac;
    supported_ta.plmn_list.push_back({cell.cell.plmn, {cu_cp_unit_plmn_item::tai_slice_t{1}}});
    cu_cp_cfg.amf_config.amf.supported_tas.push_back(supported_ta);
  }
}
