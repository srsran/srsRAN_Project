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

#include "srsran/cu_up/cu_up_configuration.h"

namespace srsran {

struct cu_up_unit_config;

/// Converts and returns the given gnb application configuration to a CU-UP configuration.
srs_cu_up::cu_up_configuration generate_cu_up_config(const cu_up_unit_config& config);

/// Converts and returns the given gnb application QoS configuration to a CU-UP configuration.
std::map<five_qi_t, srs_cu_up::cu_up_qos_config> generate_cu_up_qos_config(const cu_up_unit_config& cu_up_config);

} // namespace srsran
