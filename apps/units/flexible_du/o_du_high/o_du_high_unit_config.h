/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/units/flexible_du/o_du_high/du_high/du_high_config.h"
#include "apps/units/flexible_du/o_du_high/fapi/fapi_config.h"

namespace srsran {

/// O-DU high unit configuration.
struct o_du_high_unit_config {
  du_high_parsed_config du_high_cfg;
  fapi_unit_config      fapi_cfg;
};

} // namespace srsran
