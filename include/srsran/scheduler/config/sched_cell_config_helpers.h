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

#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {
namespace config_helpers {

/// Builds the list of PUCCH guardbands.
std::vector<sched_grid_resource> build_pucch_guardbands_list(const uplink_config& ul_cfg);

} // namespace config_helpers
} // namespace srsran
