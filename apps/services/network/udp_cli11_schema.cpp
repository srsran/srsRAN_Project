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
  add_option(app, "--max_rx_msgs", udp_params.rx_max_msgs, "Maximum amount of messages RX in a single syscall")
      ->capture_default_str();
  add_option(
      app, "--pool_threshold", udp_params.pool_threshold, "Pool accupancy threshold after which packets are dropped")
      ->capture_default_str();
  add_option(app, "--dscp", udp_params.dscp, "Differentiated Services Code Point value.")
      ->capture_default_str()
      ->check(CLI::Range(0, 63));
}

void srsran::configure_cli11_with_udp_config_schema(CLI::App& app, udp_appconfig& config)
{
  add_option(app, "--ext_addr", config.ext_addr, "External IP address that is advertised for receiving UDP packets.")
      ->check(CLI::ValidIPV4 | CLI::IsMember({"auto"}));

  CLI::App* udp_subcmd = add_subcommand(app, "udp", "UDP parameters")->configurable();
  configure_cli11_udp_args(*udp_subcmd, config);
}
