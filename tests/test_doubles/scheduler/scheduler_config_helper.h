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

#include "srsran/scheduler/config/cell_config_builder_params.h"
#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {
namespace sched_config_helper {

/// Create a default cell configuration request.
sched_cell_configuration_request_message
make_default_sched_cell_configuration_request(const cell_config_builder_params& params = {});

/// Create a default UE-dedicated SPCell cell configuration.
cell_config_dedicated create_test_initial_ue_spcell_cell_config(const cell_config_builder_params& params = {});

/// Create a default UE creation request.
sched_ue_creation_request_message
create_default_sched_ue_creation_request(const cell_config_builder_params& params      = {},
                                         span<const lcid_t>                lcid_to_cfg = {});
sched_ue_creation_request_message
create_default_sched_ue_creation_request(const cell_config_builder_params&    params,
                                         const std::initializer_list<lcid_t>& lcid_to_cfg);

/// Create a UE creation request with no UE-dedicated config.
sched_ue_creation_request_message
create_empty_spcell_cfg_sched_ue_creation_request(const cell_config_builder_params& params = {});

} // namespace sched_config_helper
} // namespace srsran
