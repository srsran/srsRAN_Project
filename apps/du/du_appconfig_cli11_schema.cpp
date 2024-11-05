/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_appconfig_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/services/logger/logger_appconfig_cli11_schema.h"
#include "apps/services/worker_manager/worker_manager_cli11_schema.h"
#include "du_appconfig.h"
#include "srsran/adt/interval.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_metrics_args(CLI::App& app, srs_du::metrics_appconfig& metrics_params)
{
  app.add_option("--addr", metrics_params.addr, "Metrics address.")->capture_default_str()->check(CLI::ValidIPV4);
  app.add_option("--port", metrics_params.port, "Metrics UDP port.")
      ->capture_default_str()
      ->check(CLI::Range(0, 65535));
}

static void configure_cli11_e2_args(CLI::App& app, e2_appconfig& e2_params)
{
  add_option(app, "--enable_du_e2", e2_params.enable_du_e2, "Enable DU E2 agent")->capture_default_str();
  add_option(app, "--addr", e2_params.ip_addr, "RIC IP address")->capture_default_str();
  add_option(app, "--port", e2_params.port, "RIC port")->capture_default_str()->check(CLI::Range(20000, 40000));
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

static void configure_cli11_f1ap_args(CLI::App& app, srs_du::f1ap_appconfig& f1c_params)
{
  app.add_option("--cu_cp_addr", f1c_params.cu_cp_address, "CU-CP F1-C address to connect to")->capture_default_str();
  app.add_option(
         "--bind_addr", f1c_params.bind_address, "DU F1-C bind address. If left empty, implicit bind is performed")
      ->capture_default_str();
}

static void configure_cli11_f1u_args(CLI::App& app, srs_du::nru_appconfig& f1u_params)
{
  app.add_option("--queue_size", f1u_params.pdu_queue_size, "F1-U PDU queue size")->capture_default_str();
  app.add_option(
         "--bind_addr", f1u_params.bind_address, "DU F1-U bind address. If left empty, implicit bind is performed")
      ->capture_default_str();
  app.add_option("--ext_addr",
                 f1u_params.ext_addr,
                 "External IP address that is advertised to receive F1-U packets from the CU-UP");
  app.add_option(
         "--pool_threshold", f1u_params.pool_threshold, "Pool occupancy threshold after which packets are dropped")
      ->capture_default_str();
}

static void configure_cli11_hal_args(CLI::App& app, std::optional<hal_appconfig>& config)
{
  config.emplace();

  add_option(app, "--eal_args", config->eal_args, "EAL configuration parameters used to initialize DPDK");
}

void srsran::configure_cli11_with_du_appconfig_schema(CLI::App& app, du_appconfig& du_cfg)
{
  add_option(app, "--du_multicell_enabled", du_cfg.du_multicell_enabled, "DU multicell enabled flag")
      ->capture_default_str();

  // Loggers section.
  configure_cli11_with_logger_appconfig_schema(app, du_cfg.log_cfg);

  // Buffer pool section.
  configure_cli11_with_buffer_pool_appconfig_schema(app, du_cfg.buffer_pool_config);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, du_cfg.expert_execution_cfg);

  // F1-C section.
  CLI::App* f1ap_subcmd = app.add_subcommand("f1ap", "F1AP interface configuration")->configurable();
  configure_cli11_f1ap_args(*f1ap_subcmd, du_cfg.f1ap_cfg);

  // F1-U section.
  CLI::App* nru_subcmd = app.add_subcommand("nru", "NR-U interface configuration")->configurable();
  configure_cli11_f1u_args(*nru_subcmd, du_cfg.nru_cfg);

  // Metrics section.
  CLI::App* metrics_subcmd = app.add_subcommand("metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, du_cfg.metrics_cfg);

  // E2 section.
  CLI::App* e2_subcmd = add_subcommand(app, "e2", "E2 parameters")->configurable();
  configure_cli11_e2_args(*e2_subcmd, du_cfg.e2_cfg);

  // HAL section.
  CLI::App* hal_subcmd = add_subcommand(app, "hal", "HAL configuration")->configurable();
  configure_cli11_hal_args(*hal_subcmd, du_cfg.hal_config);
}

void srsran::autoderive_du_parameters_after_parsing(CLI::App& app, du_appconfig& parsed_cfg) {}
