/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_o_du_unit_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_cli11_schema.h"
#include "split6_o_du_unit_config.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_fapi_args(CLI::App& app, fapi_unit_config& config)
{
  app.add_option("--l2_nof_slots_ahead", config.l2_nof_slots_ahead, "Number of slots the L2 is running ahead of the L1")
      ->capture_default_str()
      ->check(CLI::Range(0, 5));
}

void srsran::configure_cli11_with_split6_o_du_unit_config_schema(CLI::App& app, split6_o_du_unit_config& config)
{
  configure_cli11_with_o_du_high_config_schema(app, config.odu_high_cfg);

  // FAPI section.
  CLI::App* fapi_subcmd = add_subcommand(app, "fapi", "FAPI configuration")->configurable();
  configure_cli11_fapi_args(*fapi_subcmd, config.odu_high_cfg.fapi_cfg);
}

void srsran::autoderive_split6_o_du_parameters_after_parsing(CLI::App& app, split6_o_du_unit_config& config)
{
  autoderive_o_du_high_parameters_after_parsing(app, config.odu_high_cfg);
}
