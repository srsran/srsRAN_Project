/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/expected.h"
#include "srsran/du/du_cell_config.h"

namespace srsran {
namespace config_helpers {

/// \brief Compute the PRACH frequency start as a function of the PUCCH guardbands.
///
/// This function computes the PRACH frequency start so as it won't collide with the PUCCH resources. As per TS 38.331,
/// PRACH frequency start is \c msg1-FrequencyStart.
///
/// \param user_params parameters passed by the user for the generation the PUCCH resource list.
/// \param bwp_size size of the BWP in RBs.
/// \return PRACH frequency start.
unsigned compute_prach_frequency_start(const pucch_builder_params& user_params, unsigned bwp_size, bool is_long_prach);

} // namespace config_helpers
} // namespace srsran
