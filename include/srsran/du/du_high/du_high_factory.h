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

#include "srsran/du/du_high/du_high.h"
#include "srsran/du/du_high/du_high_configuration.h"

namespace srsran {

/// Create a DU-high instance, which comprises MAC, RLC and F1 layers.
std::unique_ptr<du_high> make_du_high(const srs_du::du_high_configuration& du_hi_cfg);

} // namespace srsran
