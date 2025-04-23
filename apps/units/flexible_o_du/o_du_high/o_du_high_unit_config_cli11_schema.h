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

#pragma once

#include "du_high/du_high_config_cli11_schema.h"
#include "e2/o_du_high_e2_config_cli11_schema.h"
#include "fapi/fapi_config_cli11_schema.h"
#include "o_du_high_unit_config.h"
#include "CLI/CLI11.hpp"

namespace srsran {

/// Configures the given CLI11 application with the O-DU high configuration schema.
inline void configure_cli11_with_o_du_high_config_schema(CLI::App& app, o_du_high_unit_config& unit_cfg)
{
  configure_cli11_with_du_high_config_schema(app, unit_cfg.du_high_cfg);
  configure_cli11_with_fapi_config_schema(app, unit_cfg.fapi_cfg);
  configure_cli11_with_o_du_high_e2_config_schema(app, unit_cfg.e2_cfg);
}

/// Auto derive O-DU high parameters after the parsing.
inline void autoderive_o_du_high_parameters_after_parsing(CLI::App& app, o_du_high_unit_config& unit_cfg)
{
  autoderive_du_high_parameters_after_parsing(app, unit_cfg.du_high_cfg.config);
  autoderive_o_du_high_e2_parameters_after_parsing(unit_cfg.e2_cfg);
}

} // namespace srsran
