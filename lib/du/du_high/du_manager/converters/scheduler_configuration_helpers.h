/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {
namespace srs_du {

struct du_cell_config;
struct du_ue_context;
struct du_ue_resource_config;

/// Derive Scheduler SI Scheduling Configuration from DU Cell Configuration.
std::optional<si_scheduling_config> make_si_scheduling_info_config(const du_cell_config&    du_cfg,
                                                                   span<const units::bytes> si_message_lens);

/// Derives Scheduler Cell Configuration from DU Cell Configuration.
sched_cell_configuration_request_message
make_sched_cell_config_req(du_cell_index_t                            cell_index,
                           const srs_du::du_cell_config&              du_cfg,
                           units::bytes                               sib1_len,
                           const std::optional<si_scheduling_config>& si_sched_cfg);

/// Create scheduler UE Configuration Request based on DU UE configuration context.
sched_ue_config_request create_scheduler_ue_config_request(const du_ue_context&         ue_ctx,
                                                           const du_ue_resource_config& ue_res_cfg);

} // namespace srs_du
} // namespace srsran
