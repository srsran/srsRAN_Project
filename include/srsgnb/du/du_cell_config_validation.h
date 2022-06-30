/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DU_CELL_CONFIG_VALIDATION_H
#define SRSGNB_DU_CELL_CONFIG_VALIDATION_H

#include "srsgnb/adt/expected.h"
#include "srsgnb/du/du_cell_config.h"

namespace srsgnb {

/// \brief Checks whether the provided DU cell configuration is valid.
///
/// \param cell_cfg DU cell configuration.
/// \return in case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> is_du_cell_config_valid(const du_cell_config& cell_cfg);

} // namespace srsgnb

#endif // SRSGNB_DU_CELL_CONFIG_VALIDATION_H
