/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "udp_cli11_schema.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_udp_args(CLI::App& app, udp_appconfig& udp_params)
{
  add_option(app, "--max_rx_msgs", udp_params.rx_max_mmsg, "Maximum amount of messages RX in a single syscall")
      ->capture_default_str();
  add_option(app,
             "--pool_threshold",
             udp_params.pool_occupancy_threshold,
             "Pool accupancy threshold after which packets are dropped")
      ->capture_default_str();
}

void srsran::configure_cli11_with_udp_config_schema(CLI::App& app, udp_appconfig& config)
{
  CLI::App* udp_subcmd = add_subcommand(app, "udp", "UDP parameters")->configurable();
  configure_cli11_udp_args(*udp_subcmd, config);
}
