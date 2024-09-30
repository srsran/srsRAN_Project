/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "split6_du_unit_cli11_schema.h"
#include "apps/units/flexible_du/du_high/du_high_config_cli11_schema.h"
#include "apps/units/flexible_du/fapi/fapi_config_cli11_schema.h"
#include "split6_du_unit_config.h"

using namespace srsran;

void srsran::configure_cli11_with_split6_du_unit_config_schema(CLI::App& app, split6_du_unit_config& config)
{
  configure_cli11_with_du_high_config_schema(app, config.du_high_cfg);
  configure_cli11_with_fapi_config_schema(app, config.fapi_cfg);
}

void srsran::autoderive_split6_du_parameters_after_parsing(CLI::App& app, split6_du_unit_config& config)
{
  autoderive_du_high_parameters_after_parsing(app, config.du_high_cfg.config);
}
