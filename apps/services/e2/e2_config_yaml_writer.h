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

#include <yaml-cpp/yaml.h>

namespace srsran {

struct e2_config;

/// Fills the E2 configuration in the given YAML node.
void fill_e2_config_in_yaml_schema(YAML::Node node, const e2_config& config);

} // namespace srsran
