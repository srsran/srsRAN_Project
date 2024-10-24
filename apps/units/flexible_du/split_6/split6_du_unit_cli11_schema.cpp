/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_du_unit_cli11_schema.h"
#include "apps/units/flexible_du/o_du_high/o_du_high_unit_config_cli11_schema.h"
#include "split6_du_unit_config.h"

using namespace srsran;

void srsran::configure_cli11_with_split6_du_unit_config_schema(CLI::App& app, split6_du_unit_config& config)
{
  configure_cli11_with_o_du_high_config_schema(app, config.odu_high_cfg);
}

void srsran::autoderive_split6_du_parameters_after_parsing(CLI::App& app, split6_du_unit_config& config)
{
  autoderive_o_du_high_parameters_after_parsing(app, config.odu_high_cfg);
}
