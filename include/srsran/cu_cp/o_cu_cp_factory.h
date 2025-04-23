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

#include "srsran/cu_cp/o_cu_cp.h"
#include <memory>

namespace srsran {
namespace srs_cu_cp {

struct o_cu_cp_config;
struct o_cu_cp_dependencies;

std::unique_ptr<o_cu_cp> create_o_cu_cp(const o_cu_cp_config& config, const o_cu_cp_dependencies& dependencies);

} // namespace srs_cu_cp
} // namespace srsran
