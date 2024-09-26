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

#include "apps/units/flexible_du/du_high/du_high_config.h"
#include "apps/units/flexible_du/du_low/du_low_config.h"
#include "apps/units/flexible_du/fapi/fapi_config.h"
#include "apps/units/flexible_du/split_7_2/helpers/ru_ofh_config.h"

namespace srsran {

/// Split 7.2 DU unit configuration.
struct split_7_2_du_unit_config {
  /// DU high configuration.
  du_high_parsed_config du_high_cfg;
  /// DU low configuration.
  du_low_unit_config du_low_cfg;
  /// FAPI configuration.
  fapi_unit_config fapi_cfg;
  /// Radio Unit configuration.
  ru_ofh_unit_parsed_config ru_cfg;
};

} // namespace srsran