/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DU_CELL_CONFIG_HELPERS_H
#define SRSGNB_DU_CELL_CONFIG_HELPERS_H

#include "du_cell_config.h"

namespace srsgnb {

namespace du_config_helpers {

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
du_cell_config make_default_du_cell_config();

} // namespace du_config_helpers

} // namespace srsgnb

#endif // SRSGNB_DU_CELL_CONFIG_HELPERS_H
