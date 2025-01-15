/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
