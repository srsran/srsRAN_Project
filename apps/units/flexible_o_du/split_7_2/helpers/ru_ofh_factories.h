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

#include "ru_ofh_config.h"
#include "srsran/ru/ru.h"
#include <memory>

namespace srsran {
struct flexible_o_du_ru_config;
struct flexible_o_du_ru_dependencies;

/// Creates an Open Fronthaul Radio Unit using the given configuration.
std::unique_ptr<radio_unit> create_ofh_radio_unit(const ru_ofh_unit_config&            ru_cfg,
                                                  const flexible_o_du_ru_config&       ru_config,
                                                  const flexible_o_du_ru_dependencies& ru_dependencies);

} // namespace srsran
