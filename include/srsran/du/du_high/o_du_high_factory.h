/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/du/du_high/o_du_high.h"
#include <memory>

namespace srsran {
namespace srs_du {

struct o_du_high_config;
struct o_du_high_dependencies;

/// Creates an O-RAN DU high with the given configuration and dependencies.
std::unique_ptr<o_du_high> make_o_du_high(const o_du_high_config& config, o_du_high_dependencies&& dependencies);

} // namespace srs_du
} // namespace srsran
