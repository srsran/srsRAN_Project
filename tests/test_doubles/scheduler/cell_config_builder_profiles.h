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

#include "srsran/scheduler/config/cell_config_builder_params.h"

namespace srsran {
namespace cell_config_builder_profiles {

/// Create cell build parameters for a TDD band.
cell_config_builder_params tdd(subcarrier_spacing   scs = subcarrier_spacing::kHz30,
                               bs_channel_bandwidth bw  = bs_channel_bandwidth::MHz20);

/// Create cell build parameters for a FDD band.
cell_config_builder_params fdd();

} // namespace cell_config_builder_profiles
} // namespace srsran
