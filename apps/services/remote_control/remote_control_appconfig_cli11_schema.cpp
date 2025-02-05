/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "apps/services/remote_control/remote_control_appconfig_cli11_schema.h"
#include "apps/services/remote_control/remote_control_appconfig.h"

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
}
