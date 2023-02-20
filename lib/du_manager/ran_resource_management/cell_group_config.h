/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
