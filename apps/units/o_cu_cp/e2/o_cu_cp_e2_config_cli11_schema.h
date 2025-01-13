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

#include "srsran/adt/span.h"
#include "CLI/CLI11.hpp"

namespace srsran {

struct o_cu_cp_e2_config;

/// Configures the given CLI11 application with the O-RAN CU-CP E2 configuration schema.
void configure_cli11_with_o_cu_cp_e2_config_schema(CLI::App& app, o_cu_cp_e2_config& unit_cfg);

/// Auto derive O-RAN CU-CP E2 parameters after the parsing.
void autoderive_o_cu_cp_e2_parameters_after_parsing(o_cu_cp_e2_config& unit_cfg);

} // namespace srsran
