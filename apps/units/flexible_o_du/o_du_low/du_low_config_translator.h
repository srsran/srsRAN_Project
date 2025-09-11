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

#include "o_du_low_unit_factory_config.h"
#include "srsran/adt/span.h"
#include "srsran/du/du_low/o_du_low_config.h"

namespace srsran {

namespace srs_du {
struct du_cell_config;
struct du_low_config;
} // namespace srs_du

struct du_low_unit_config;
struct worker_manager_config;

/// Generates O-DU low configuration from the given parameters.
void generate_o_du_low_config(srs_du::o_du_low_config&                        out_config,
                              const du_low_unit_config&                       du_low_unit_cfg,
                              span<const o_du_low_unit_config::du_low_config> cells);

/// Fills the DU low worker manager parameters of the given worker manager configuration.
void fill_du_low_worker_manager_config(worker_manager_config&    config,
                                       const du_low_unit_config& unit_cfg,
                                       unsigned                  is_blocking_mode_active,
                                       span<const unsigned>      nof_dl_antennas,
                                       span<const unsigned>      nof_ul_antennas);

} // namespace srsran
