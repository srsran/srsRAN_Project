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

struct e2_config;

/// \brief Configures the given CLI11 application with the E2 application configuration schema.
///
/// \param[out] app CLI11 application to configure.
/// \param[out] config E2 configuration that stores the parameters.
/// \param[in] option_name Option name for the E2 unit property.
/// \param[out] option_description Option description for the E2 unit property.
void configure_cli11_with_e2_config_schema(CLI::App&          app,
                                           e2_config&         config,
                                           const std::string& option_name,
                                           const std::string& option_description);

} // namespace srsran
