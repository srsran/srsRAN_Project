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

#include "srsran/cu_up/o_cu_up.h"
#include "srsran/cu_up/o_cu_up_config.h"
#include <memory>

namespace srsran {
namespace srs_cu_up {

/// O-RAN CU-UP interface with the given configuration and dependencies.
std::unique_ptr<o_cu_up> create_o_cu_up(const o_cu_up_config& config, o_cu_up_dependencies&& dependencies);

} // namespace srs_cu_up
} // namespace srsran
