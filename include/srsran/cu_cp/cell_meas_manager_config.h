/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "cu_cp_types.h"

namespace srsran {

namespace srs_cu_cp {

/// \brief Cell manager configuration.
struct cell_meas_manager_cfg {
  std::map<nr_cell_id_t, std::vector<nr_cell_id_t>> neighbor_cell_list; ///< List of neighbor cells
  optional<cu_cp_cond_event_a3> a3_event_config; // A3 event config is currently the only supported event.
};

/// \brief Validates configuration. Returns true if config is valid, false otherwise.
bool is_valid_configuration(const cell_meas_manager_cfg& cfg);

} // namespace srs_cu_cp

} // namespace srsran
