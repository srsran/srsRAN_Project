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

#include "srsran/e2/e2ap_configuration_helpers.h"

namespace srsran {

struct o_cu_cp_e2_config;
struct worker_manager_config;

/// Converts and returns the given O-RAN CU-CP configuration to a E2 configuration.
e2ap_configuration generate_e2_config(const o_cu_cp_e2_config& cu_cp, gnb_id_t gnb_id);

/// Fills the O-RAN CU CP E2 worker manager parameters of the given worker manager configuration.
void fill_o_cu_cp_e2_worker_manager_config(worker_manager_config& config, const o_cu_cp_e2_config& unit_cfg);

} // namespace srsran
