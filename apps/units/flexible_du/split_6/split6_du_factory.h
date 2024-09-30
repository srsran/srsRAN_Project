/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "apps/units/flexible_du/du_unit.h"
#include <memory>
#include <vector>

namespace srsran {

namespace fapi {
class fapi_adaptor;
} // namespace fapi

struct split6_du_unit_config;

/// Helper function to build the DU Split 6 with the given arguments.
du_unit create_du_split6(const split6_du_unit_config&                     du_unit_cfg,
                         const du_unit_dependencies&                      du_dependencies,
                         std::vector<std::unique_ptr<fapi::fapi_adaptor>> fapi_adaptors);

} // namespace srsran
