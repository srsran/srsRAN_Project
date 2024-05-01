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

#include "srsran//du_low/du_low_wrapper_config.h"
#include "srsran/du/du_high_wrapper_config.h"

namespace srsran {

/// DU wrapper configuration.
struct du_wrapper_config {
  du_high_wrapper_config du_high_cfg;
  du_low_wrapper_config  du_low_cfg;
};

} // namespace srsran
