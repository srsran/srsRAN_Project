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
#include "apps/helpers/hal/hal_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_schema.h"
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

static void manage_hal_optional(CLI::App& app, gnb_appconfig& gnb_cfg)
{
  if (!is_hal_section_present(app)) {
    gnb_cfg.hal_config.reset();
  }
}

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

  // Buffer pool section.
  configure_cli11_with_buffer_pool_appconfig_schema(app, gnb_cfg.buffer_pool_config);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, gnb_cfg.expert_execution_cfg);

  // Metrics section.
  CLI::App* metrics_subcmd = app.add_subcommand("metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, gnb_cfg.metrics_cfg);
  app_services::configure_cli11_with_app_resource_usage_config_schema(app, gnb_cfg.metrics_cfg.rusage_config);
  app_services::configure_cli11_with_metrics_appconfig_schema(app, gnb_cfg.metrics_cfg.metrics_service_cfg);

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

void srsran::autoderive_supported_tas_for_amf_from_du_cells(const du_high_unit_config& du_hi_cfg,
                                                            cu_cp_unit_config&         cu_cp_cfg)
{
  // If no cells are found in DU configuration.
  if (du_hi_cfg.cells_cfg.empty()) {
    return;
  }

  // Clear supported TAs.
  srslog::fetch_basic_logger("CONFIG").debug(
      "{} supported TAs will be derived from DU cell config\n",
      cu_cp_cfg.amf_config.amf.is_default_supported_tas ? "No supported TAs configured," : "--no-core configured,");
  cu_cp_cfg.amf_config.amf.supported_tas.clear();
  cu_cp_cfg.amf_config.amf.is_default_supported_tas = false;
  cu_cp_cfg.amf_config.amf.supported_tas.resize(du_hi_cfg.cells_cfg.size());

  // Derive supported TAs from DU cell configuration.
  for (const auto& cell : du_hi_cfg.cells_cfg) {
    cu_cp_unit_supported_ta_item supported_ta;
    supported_ta.tac = cell.cell.tac;
    supported_ta.plmn_list.push_back({cell.cell.plmn, {cu_cp_unit_plmn_item::tai_slice_t{1}}});
    cu_cp_cfg.amf_config.amf.supported_tas.push_back(supported_ta);
  }
}
