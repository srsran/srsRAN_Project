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

#include "srsran/du/o_du.h"
#include "srsran/du/o_du_config.h"
#include <memory>

namespace srsran {
namespace srs_du {

/// Instantiates an O-RAN Distributed Unit (O-DU) object with the given configuration and dependencies.
std::unique_ptr<o_du> make_o_du(std::unique_ptr<o_du_high> odu_hi, std::unique_ptr<o_du_low> odu_lo);

} // namespace srs_du
} // namespace srsran
