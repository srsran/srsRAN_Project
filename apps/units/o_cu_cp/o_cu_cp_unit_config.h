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

#include "cu_cp/cu_cp_unit_config.h"
#include "e2/o_cu_cp_e2_config.h"

namespace srsran {

/// O-RAN CU-CP application unit configuration.
struct o_cu_cp_unit_config {
  cu_cp_unit_config cucp_cfg;
  o_cu_cp_e2_config e2_cfg;
};

} // namespace srsran
