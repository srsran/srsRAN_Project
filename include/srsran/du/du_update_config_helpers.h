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

/// \brief Checks whether the provided DU cell configuration is valid.
///
/// \param cell_cfg DU cell configuration.
/// \return in case an invalid parameter is detected, returns a string containing an error message.
unsigned update_prach_frequency_start(const pucch_builder_params& user_params, unsigned bwp_size);

} // namespace config_helpers
} // namespace srsran
