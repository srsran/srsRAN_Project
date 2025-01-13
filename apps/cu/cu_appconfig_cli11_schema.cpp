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

#include "cu_appconfig_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/services/logger/logger_appconfig_cli11_schema.h"
#include "apps/services/worker_manager/worker_manager_cli11_schema.h"
#include "cu_appconfig.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/config_parsers.h"

using namespace srsran;

static void configure_cli11_f1ap_args(CLI::App& app, srs_cu::cu_f1ap_appconfig& f1ap_params)
{
  add_option(app, "--bind_addr", f1ap_params.bind_addr, "F1-C bind address")->capture_default_str();
}

static void configure_cli11_f1u_socket_args(CLI::App& app, srs_cu::cu_f1u_socket_appconfig& f1u_cfg)
{
  add_option(app,
             "--bind_addr",
             f1u_cfg.bind_addr,
             "Default local IP address interfaces bind to, unless a specific bind address is specified")
      ->check(CLI::ValidIPV4);

  configure_cli11_with_udp_config_schema(app, f1u_cfg.udp_config);
}

static void configure_cli11_f1u_args(CLI::App& app, srs_cu::cu_f1u_appconfig& f1u_params)
{
  // Add option for multiple sockets, for usage with different slices, 5QIs or parallization.
  auto sock_lambda = [&f1u_params](const std::vector<std::string>& values) {
    // Prepare the radio bearers
    f1u_params.f1u_socket_cfg.resize(values.size());

    // Format every F1-U socket configuration.
    for (unsigned i = 0, e = values.size(); i != e; ++i) {
      CLI::App subapp("NG-U socket parameters", "NG-U socket config, item #" + std::to_string(i));
      subapp.config_formatter(create_yaml_config_parser());
      subapp.allow_config_extras(CLI::config_extras_mode::capture);
      configure_cli11_f1u_socket_args(subapp, f1u_params.f1u_socket_cfg[i]);
      std::istringstream ss(values[i]);
      subapp.parse_from_stream(ss);
    }
  };
  add_option_cell(app, "--socket", sock_lambda, "Configures UDP/IP socket parameters of the F1-U interface");
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
  CLI::App* f1u_subcmd   = add_subcommand(*cu_up_subcmd, "f1u", "NR-U parameters")->configurable();
  configure_cli11_f1u_args(*f1u_subcmd, cu_cfg.f1u_cfg);
}
