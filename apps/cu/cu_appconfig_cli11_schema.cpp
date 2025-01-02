/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_appconfig_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/services/logger/logger_appconfig_cli11_schema.h"
#include "apps/services/worker_manager/worker_manager_cli11_schema.h"
#include "cu_appconfig.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_f1ap_args(CLI::App& app, srs_cu::cu_f1ap_appconfig& f1ap_params)
{
  add_option(app, "--bind_addr", f1ap_params.bind_addr, "F1-C bind address")->capture_default_str();
}

void srsran::configure_cli11_with_cu_appconfig_schema(CLI::App& app, cu_appconfig& cu_cfg)
{
  // Logging section.
  configure_cli11_with_logger_appconfig_schema(app, cu_cfg.log_cfg);

  // Buffer pool section.
  configure_cli11_with_buffer_pool_appconfig_schema(app, cu_cfg.buffer_pool_config);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, cu_cfg.expert_execution_cfg);

  // F1AP section.
  CLI::App* cu_cp_subcmd = add_subcommand(app, "cu_cp", "CU-UP parameters")->configurable();
  CLI::App* f1ap_subcmd  = add_subcommand(*cu_cp_subcmd, "f1ap", "F1AP parameters")->configurable();
  configure_cli11_f1ap_args(*f1ap_subcmd, cu_cfg.f1ap_cfg);

  // NR-U section.
  CLI::App* cu_up_subcmd = add_subcommand(app, "cu_up", "CU-UP parameters")->configurable();
  CLI::App* f1u_subcmd   = add_subcommand(*cu_up_subcmd, "f1u", "F1-U parameters")->configurable();
  configure_cli11_f1u_sockets_args(*f1u_subcmd, cu_cfg.f1u_cfg);
}
