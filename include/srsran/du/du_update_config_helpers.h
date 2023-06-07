/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
