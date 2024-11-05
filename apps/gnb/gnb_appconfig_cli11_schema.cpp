/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gnb_appconfig_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/services/logger/logger_appconfig_cli11_schema.h"
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
}

static void configure_cli11_e2_args(CLI::App& app, e2_appconfig& e2_params)
{
  add_option(app, "--enable_du_e2", e2_params.enable_du_e2, "Enable DU E2 agent")->capture_default_str();
  add_option(app, "--enable_cu_e2", e2_params.enable_cu_e2, "Enable CU E2 agent")->capture_default_str();
  add_option(app, "--addr", e2_params.ip_addr, "RIC IP address")->capture_default_str();
  add_option(app, "--port", e2_params.port, "RIC port")->check(CLI::Range(20000, 40000))->capture_default_str();
  add_option(app, "--bind_addr", e2_params.bind_addr, "Local IP address to bind for RIC connection")
      ->capture_default_str()
      ->check(CLI::ValidIPV4);
  add_option(app, "--sctp_rto_initial", e2_params.sctp_rto_initial, "SCTP initial RTO value")->capture_default_str();
  add_option(app, "--sctp_rto_min", e2_params.sctp_rto_min, "SCTP RTO min")->capture_default_str();
  add_option(app, "--sctp_rto_max", e2_params.sctp_rto_max, "SCTP RTO max")->capture_default_str();
  add_option(app, "--sctp_init_max_attempts", e2_params.sctp_init_max_attempts, "SCTP init max attempts")
      ->capture_default_str();
  add_option(app, "--sctp_max_init_timeo", e2_params.sctp_max_init_timeo, "SCTP max init timeout")
      ->capture_default_str();
  add_option(app, "--e2sm_kpm_enabled", e2_params.e2sm_kpm_enabled, "Enable KPM service module")->capture_default_str();
  add_option(app, "--e2sm_rc_enabled", e2_params.e2sm_rc_enabled, "Enable RC service module")->capture_default_str();
}

static void configure_cli11_hal_args(CLI::App& app, std::optional<hal_appconfig>& config)
{
  config.emplace();

  add_option(app, "--eal_args", config->eal_args, "EAL configuration parameters used to initialize DPDK");
}

static void manage_hal_optional(CLI::App& app, gnb_appconfig& gnb_cfg)
{
  // Clean the HAL optional.
  if (auto subcmd = app.get_subcommand("hal"); subcmd->count_all() == 0) {
    gnb_cfg.hal_config.reset();
    // As HAL configuration is optional, disable the command when it is not present in the configuration.
    subcmd->disabled();
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

  add_option(app, "--du_multicell_enabled", gnb_parsed_cfg.du_multicell_enabled, "DU multicell enabled flag")
      ->capture_default_str();

  // Loggers section.
  configure_cli11_with_logger_appconfig_schema(app, gnb_cfg.log_cfg);

  // Buffer pool section.
  configure_cli11_with_buffer_pool_appconfig_schema(app, gnb_cfg.buffer_pool_config);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, gnb_cfg.expert_execution_cfg);

  // Metrics section.
  CLI::App* metrics_subcmd = app.add_subcommand("metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, gnb_cfg.metrics_cfg);

  // E2 section.
  CLI::App* e2_subcmd = add_subcommand(app, "e2", "E2 parameters")->configurable();
  configure_cli11_e2_args(*e2_subcmd, gnb_cfg.e2_cfg);

  // HAL section.
  CLI::App* hal_subcmd = add_subcommand(app, "hal", "HAL configuration")->configurable();
  configure_cli11_hal_args(*hal_subcmd, gnb_cfg.hal_config);
}

void srsran::autoderive_gnb_parameters_after_parsing(CLI::App& app, gnb_appconfig& config)
{
  manage_hal_optional(app, config);
}
