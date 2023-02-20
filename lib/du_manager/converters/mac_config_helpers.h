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

#include "srsgnb/mac/cell_configuration.h"

namespace srsgnb {

struct du_cell_config;

/// Derives MAC Cell Configuration from DU Cell Configuration.
mac_cell_creation_request make_mac_cell_config(du_cell_index_t                                 cell_index,
                                               const du_cell_config&                           du_cfg,
                                               byte_buffer                                     bcch_dl_sch_payload,
                                               const sched_cell_configuration_request_message& sched_cell_cfg);

} // namespace srsgnb
