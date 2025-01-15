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

#include "srsran/du/du_low/du_low.h"
#include <memory>

namespace srsran {
namespace srs_du {

struct du_low_config;

/// Creates and returns the DU low.
std::unique_ptr<du_low> make_du_low(const du_low_config& config);

} // namespace srs_du
} // namespace srsran
