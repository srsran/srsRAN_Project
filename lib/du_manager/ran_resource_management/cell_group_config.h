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

#pragma once

#include "../converters/rlc_config_helpers.h"
#include "srsran/mac/mac_cell_group_config.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/physical_cell_group.h"
#include "srsran/rlc/rlc_config.h"
#include "srsran/scheduler/config/serving_cell_config.h"

namespace srsran {
namespace srs_du {

/// This struct stores the accumulated CellGroupConfig.
struct cell_group_config {
  std::vector<rlc_bearer_config>        rlc_bearers;
  mac_cell_group_config                 mcg_cfg;
  physical_cell_group_config            pcg_cfg;
  slotted_vector<cell_config_dedicated> cells;
};

} // namespace srs_du
} // namespace srsran
