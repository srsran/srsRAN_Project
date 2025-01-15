/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du_cell_config.h"
#include "srsran/mac/mac_cell_group_config.h"

namespace srsran {
namespace config_helpers {

mac_cell_group_config make_initial_mac_cell_group_config(const srs_du::mac_cell_group_params& mcg_params = {});

} // namespace config_helpers
} // namespace srsran
