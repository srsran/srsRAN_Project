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

#include <yaml-cpp/yaml.h>

namespace srsran {

struct o_cu_cp_e2_config;

/// Fills the given node with the O-RAN CU-CP E2 configuration values.
void fill_o_cu_cp_e2_config_in_yaml_schema(YAML::Node& node, const o_cu_cp_e2_config& config);

} // namespace srsran
