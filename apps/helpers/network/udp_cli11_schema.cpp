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

#include "udp_cli11_schema.h"
#include "udp_appconfig.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_udp_args(CLI::App& app, udp_appconfig& udp_params)
{
  add_option(app, "--max_rx_msgs", udp_params.rx_max_msgs, "Maximum amount of messages RX in a single syscall")
      ->capture_default_str();
  add_option(app, "--tx_qsize", udp_params.tx_qsize, "Size of TX queue used for batching SDUs.")->capture_default_str();
  add_option(app, "--max_tx_msgs", udp_params.tx_max_msgs, "Maximum amount of messages TX in a single syscall")
      ->capture_default_str();
  add_option(app, "--max_tx_segments", udp_params.tx_max_segments, "Maximum amount of segments TX in a single SDU")
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
