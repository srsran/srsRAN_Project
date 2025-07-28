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
#include <optional>

namespace srsran {

struct ntn_config;

/// Helper function to configure NTN capabilities.
void configure_cli11_ntn_args(CLI::App& app, ntn_config& config);

/// Helper function to configure advanced NTN capabilities.
void configure_cli11_advanced_ntn_args(CLI::App& app, ntn_config& config);

/// Helper function to configure per-cell NTN capabilities.
void configure_cli11_cell_ntn_args(CLI::App& app, std::optional<ntn_config>& cell_ntn_params);

} // namespace srsran
