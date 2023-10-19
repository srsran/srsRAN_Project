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

#include "srsran/mac/cell_configuration.h"

namespace srsran {

struct du_cell_config;

/// Derives MAC Cell Configuration from DU Cell Configuration.
mac_cell_creation_request make_mac_cell_config(du_cell_index_t                                 cell_index,
                                               const du_cell_config&                           du_cfg,
                                               std::vector<byte_buffer>                        bcch_dl_sch_payloads,
                                               const sched_cell_configuration_request_message& sched_cell_cfg);

} // namespace srsran
