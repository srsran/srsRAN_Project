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

#include "hal_cli11_schema.h"
#include "hal_appconfig.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_hal_args(CLI::App& app, hal_appconfig& config)
{
  add_option(app, "--eal_args", config.eal_args, "EAL configuration parameters used to initialize DPDK");
}

void srsran::configure_cli11_with_hal_appconfig_schema(CLI::App& app, hal_appconfig& config)
{
  CLI::App* hal_subcmd = add_subcommand(app, "hal", "HAL configuration")->configurable();
  configure_cli11_hal_args(*hal_subcmd, config);
}

bool srsran::is_hal_section_present(CLI::App& app)
{
  auto subcmd = app.get_subcommand("hal");
  return subcmd->count_all() != 0;
}
