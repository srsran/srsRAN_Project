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

#include "apps/units/o_cu_up/cu_up/cu_up_unit_config.h"
#include "apps/units/o_cu_up/e2/o_cu_up_e2_config.h"

namespace srsran {

/// O-RAN CU-UP application unit configuration.
struct o_cu_up_unit_config {
  cu_up_unit_config cu_up_cfg;
  o_cu_up_e2_config e2_cfg;
};

} // namespace srsran
