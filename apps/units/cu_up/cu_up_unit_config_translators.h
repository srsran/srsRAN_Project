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

#include "srsran/cu_up/cu_up_configuration.h"

namespace srsran {

struct cu_up_unit_config;
struct worker_manager_config;

/// Converts and returns the given gnb application configuration to a CU-UP configuration.
srs_cu_up::cu_up_configuration generate_cu_up_config(const cu_up_unit_config& config);

/// Converts and returns the given gnb application QoS configuration to a CU-UP configuration.
std::map<five_qi_t, srs_cu_up::cu_up_qos_config> generate_cu_up_qos_config(const cu_up_unit_config& cu_up_config);

/// Fills the CU-UP worker manager parameters of the given worker manager configuration.
void fill_cu_up_worker_manager_config(worker_manager_config& config, const cu_up_unit_config& unit_cfg);

e2ap_configuration generate_e2_config(const cu_up_unit_config& cu_cp);

} // namespace srsran
