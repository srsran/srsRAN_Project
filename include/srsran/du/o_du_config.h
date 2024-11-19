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

#include "srsran/du/du_high/o_du_high_config.h"
#include "srsran/du/du_low/o_du_low_config.h"

namespace srsran {
namespace srs_du {

/// O-RAN DU configuration.
struct o_du_config {
  o_du_high_config du_high_cfg;
  o_du_low_config  du_low_cfg;
};

/// O-RAN DU dependencies.
struct o_du_dependencies {
  o_du_high_dependencies du_high_deps;
};

} // namespace srs_du
} // namespace srsran
