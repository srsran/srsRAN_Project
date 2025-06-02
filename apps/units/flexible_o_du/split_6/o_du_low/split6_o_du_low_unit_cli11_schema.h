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

#include "CLI/CLI11.hpp"

namespace srsran {

struct split6_o_du_low_unit_config;

/// Configures the given CLI11 application with the O-RAN DU low Split 6 unit configuration schema.
void configure_cli11_with_split6_o_du_low_unit_config_schema(CLI::App& app, split6_o_du_low_unit_config& config);

/// Auto derive O-RAN DU low Split 6 parameters after the parsing.
void autoderive_split6_o_du_low_parameters_after_parsing(CLI::App& app, split6_o_du_low_unit_config& config);

} // namespace srsran
