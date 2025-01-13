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

#include "du_appconfig_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/services/hal/hal_cli11_schema.h"
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

void srsran::configure_cli11_with_du_appconfig_schema(CLI::App& app, du_appconfig& du_cfg)
{
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

  // HAL section.
  du_cfg.hal_config.emplace();
  configure_cli11_with_hal_appconfig_schema(app, *du_cfg.hal_config);
}

static void manage_hal_optional(CLI::App& app, du_appconfig& du_cfg)
{
  if (!is_hal_section_present(app)) {
    du_cfg.hal_config.reset();
  }
}

void srsran::autoderive_du_parameters_after_parsing(CLI::App& app, du_appconfig& du_cfg)
{
  manage_hal_optional(app, du_cfg);
}
