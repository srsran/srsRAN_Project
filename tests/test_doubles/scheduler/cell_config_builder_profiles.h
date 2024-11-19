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

#include "srsran/scheduler/config/cell_config_builder_params.h"

namespace srsran {
namespace cell_config_builder_profiles {

/// Create cell build parameters for a TDD band.
cell_config_builder_params tdd(subcarrier_spacing scs = subcarrier_spacing::kHz30);

/// Create cell build parameters for a FDD band.
cell_config_builder_params fdd();

} // namespace cell_config_builder_profiles
} // namespace srsran
