/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/du_manager/du_manager.h"

namespace srsgnb {
namespace srs_du {

/// This struct stores the accumulated CellGroupConfig.
struct cell_group_config {};

cell_group_config make_initial_ue_cell_group_config();

cell_group_config make_reconf_ue_cell_group_config(const f1ap_ue_config_update_request& request);

} // namespace srs_du
} // namespace srsgnb