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

#include "srsran/scheduler/config/pucch_builder_params.h"
#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {

struct pucch_builder_params;
struct serving_cell_config;
struct dl_config_common;

namespace config_helpers {

/// Builds the list of PUCCH guardbands.
std::vector<sched_grid_resource> build_pucch_guardbands_list(const pucch_builder_params& user_params,
                                                             unsigned                    bwp_size);

unsigned compute_tot_nof_monitored_pdcch_candidates_per_slot(const serving_cell_config& ue_cell_cfg,
                                                             const dl_config_common&    dl_cfg_common);
} // namespace config_helpers
} // namespace srsran
