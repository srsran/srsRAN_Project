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

#include "srsran/e2/e2ap_configuration.h"

namespace srsran {

struct e2_config;
struct o_cu_up_e2_config;
struct worker_manager_config;

/// Generate E2AP configuration from the given E2 configuration.
e2ap_configuration generate_e2_config(const e2_config& config);

/// Fills the O-RAN CU UP E2 worker manager parameters of the given worker manager configuration.
void fill_o_cu_up_e2_worker_manager_config(worker_manager_config& config, const o_cu_up_e2_config& unit_cfg);

} // namespace srsran
