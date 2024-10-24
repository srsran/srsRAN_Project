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

#include "srsran/du/du_high/o_du_high.h"
#include <memory>

namespace srsran {
namespace srs_du {

struct o_du_high_config;
struct o_du_high_dependencies;

/// Creates an ORAN DU high with the given configuration and dependencies.
std::unique_ptr<o_du_high> make_o_du_high(const o_du_high_config& config, o_du_high_dependencies&& dependencies);

} // namespace srs_du
} // namespace srsran
