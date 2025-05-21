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

#include "du_high_config.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/du/du_high/du_srb_config.h"
#include <map>

namespace srsran {

namespace srs_du {
struct du_high_configuration;
}

struct du_high_unit_config;
struct worker_manager_config;

/// Converts and returns the given gnb application configuration to a DU cell configuration.
std::vector<srs_du::du_cell_config> generate_du_cell_config(const du_high_unit_config& config);

/// Generates and fills the given DU high configuration from the given DU high unit configuration.
void generate_du_high_config(srs_du::du_high_configuration& du_hi_cfg, const du_high_unit_config& du_high_unit_cfg);

/// Augments RLC parameters based on NTN configuration.
void ntn_augment_rlc_parameters(const ntn_config& ntn_cfg, std::map<srb_id_t, srs_du::du_srb_config>& srb_cfgs);

/// Fills the DU high worker manager parameters of the given worker manager configuration.
void fill_du_high_worker_manager_config(worker_manager_config&     config,
                                        const du_high_unit_config& unit_cfg,
                                        bool                       is_blocking_mode_enabled = false);

} // namespace srsran
