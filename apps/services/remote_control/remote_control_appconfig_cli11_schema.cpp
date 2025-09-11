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

#include "apps/services/remote_control/remote_control_appconfig_cli11_schema.h"
#include "apps/services/remote_control/remote_control_appconfig.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_remote_control_args(CLI::App& app, remote_control_appconfig& config)
{
  app.add_option("--enabled", config.enabled, "Enables the Remote Control Server")->always_capture_default();
  app.add_option("--bind_addr", config.bind_addr, "Remote Control Server bind address")->capture_default_str();
  app.add_option("--port", config.port, "Port where the remote control server listens for incoming connections")
      ->capture_default_str()
      ->check(CLI::Range(0, 65535));
}

void srsran::configure_cli11_with_remote_control_appconfig_schema(CLI::App& app, remote_control_appconfig& config)
{
  // Remote control section.
  CLI::App* remote_control_subcmd =
      app.add_subcommand("remote_control", "Remote control configuration")->configurable();
  configure_cli11_remote_control_args(*remote_control_subcmd, config);
  // Metrics section.
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  add_option(*metrics_subcmd, "--enable_json", config.enable_metrics_subscription, "Enable JSON metrics reporting")
      ->always_capture_default();
}
