/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
create_default_sched_ue_creation_request(const cell_config_builder_params&    params      = {},
                                         const std::initializer_list<lcid_t>& lcid_to_cfg = {});

/// Create a UE creation request with no UE-dedicated config.
sched_ue_creation_request_message
create_empty_spcell_cfg_sched_ue_creation_request(const cell_config_builder_params& params = {});

} // namespace sched_config_helper
} // namespace srsran
