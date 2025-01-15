/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du_cell_config.h"
#include "srsran/support/config/validator_result.h"

namespace srsran {
namespace srs_du {

/// \brief Checks whether the provided DU cell configuration is valid.
///
/// \param cell_cfg DU cell configuration.
/// \return in case an invalid parameter is detected, returns a string containing an error message.
validator_result is_du_cell_config_valid(const du_cell_config& cell_cfg);

} // namespace srs_du
} // namespace srsran
